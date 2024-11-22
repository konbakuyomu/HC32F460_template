/**
 * @file drv_uart.cpp
 * @brief 串口驱动层实现
 * @version 1.0
 * @date 2024-11-08
 */

#include "globalConfig.h"

namespace DRV
{
// 初始化单例指针为空
std::unique_ptr<UARTDriver> UARTDriver::m_usart = nullptr;

/**
 * @brief 构造函数
 */
UARTDriver::UARTDriver() { createTxQueue(); }

/**
 * @brief 析构函数
 */
UARTDriver::~UARTDriver()
{
    if (m_uartTxQueue != nullptr) {
        vQueueDelete(m_uartTxQueue);
        m_uartTxQueue = nullptr;
    }
}

/**
 * @brief 创建发送队列
 */
void UARTDriver::createTxQueue()
{
    if (m_uartTxQueue == nullptr) {
        m_uartTxQueue = xQueueCreate(10, sizeof(stc_uart_tx_frame_t));
        if (m_uartTxQueue == nullptr) {
            // 处理队列创建失败
            DDL_Printf("Failed to create UART TX queue\n");
        }
    }
}

/**
 * @brief 发送数据到队列
 * @param data 数据
 * @param length 数据长度
 * @return 是否发送成功
 */
bool UARTDriver::sendToQueue(const uint8_t *data, size_t length)
{
    if (m_uartTxQueue == nullptr || data == nullptr || length == 0) {
        return false;
    }

    stc_uart_tx_frame_t txFrame;
    txFrame.data = const_cast<uint8_t *>(data); // 注意：确保数据在发送完成前保持有效
    txFrame.length = length;

    BaseType_t result = xQueueSend(m_uartTxQueue, &txFrame, portMAX_DELAY);
    return (result == pdPASS);
}

/**
 * @brief 发送人员站立状态
 */
void UARTDriver::sendPersonStandingStatus(en_pin_state_t pin_state)
{
    // 清空发送缓冲区
    std::fill(hostSendBuffer, hostSendBuffer + HOST_BUFFER_LENGTH, 0);

    // 填充发送缓冲区
    hostSendBuffer[0] = 0xDD;
    hostSendBuffer[1] = 0xDD;
    hostSendBuffer[2] = 0x0B;
    hostSendBuffer[3] = 0x00;
    hostSendBuffer[4] = 0x00;
    hostSendBuffer[5] = static_cast<uint8_t>(Command::ReportRealTimeData);
    hostSendBuffer[6] = pin_state ? 0x00 : 0x01;

    // CRC校验
    unsigned short crc = calculateChecksum(hostSendBuffer, 7);
    hostSendBuffer[7] = crc & 0xFF;
    hostSendBuffer[8] = (crc >> 8) & 0xFF;
    hostSendBuffer[9] = 0xFF;
    hostSendBuffer[10] = 0xFF;

    // 发送队列
    stc_uart_tx_frame_t stcTx;
    stcTx.data = hostSendBuffer;
    stcTx.length = 11;

    // 使用成员队列发送
    if (getInstance()->m_uartTxQueue != nullptr) {
        xQueueSend(getInstance()->m_uartTxQueue, &stcTx, portMAX_DELAY);
    }
}

/**
 * @brief 注册UART功能回调函数
 */
void UARTDriver::initUARTHandlers()
{
    // 注册入口栏杆控制回调函数
    registerReceiveCallback(static_cast<uint8_t>(Command::ControlEntranceGate),
                            [this](uint8_t data) { handleEntranceBarrier(data); });

    // 注册出口闸机控制回调函数
    registerReceiveCallback(static_cast<uint8_t>(Command::ControlExitGate),
                            [this](uint8_t data) { handleExitGate(data); });

    // 注册报警灯控制回调函数
    registerReceiveCallback(static_cast<uint8_t>(Command::ControlAlarmLight),
                            [this](uint8_t data) { handleAlarmLight(data); });

    // 注册故障灯控制回调函数
    registerReceiveCallback(static_cast<uint8_t>(Command::ControlFaultLight),
                            [this](uint8_t data) { handleFaultLight(data); });
}

/**
 * @brief 计算校验和
 * @param buf 数据缓冲区
 * @param len 数据长度
 * @return 校验和
 */
unsigned short UARTDriver::calculateChecksum(const uint8_t *buf, size_t len)
{
    uint16_t crc = 0xFFFF; // 初始值为0xFFFF

    for (size_t i = 0; i < len; ++i) {
        crc ^= static_cast<uint16_t>(buf[i]);

        for (uint8_t j = 0; j < 8; ++j) {
            if (crc & 1) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

void UARTDriver::processReceivedData(uint8_t *receivedBuffer, size_t receivedSize)
{
    // 将接收到的数据复制到std::array中
    std::array<uint8_t, 11> dataArray = {0};
    if (receivedSize >= dataArray.size()) {
        std::copy(receivedBuffer, receivedBuffer + dataArray.size(), dataArray.begin());
    } else {
        DDL_Printf("数据长度不足，无法解析。\n");
        return;
    }

    // 现在使用dataArray进行操作
    // 检查帧头是否正确（帧头为0xDD 0xDD）
    if (dataArray[0] != 0xDD || dataArray[1] != 0xDD) {
        DDL_Printf("帧头错误，丢弃数据。\n");
        return;
    }

    // 计算并验证CRC校验值
    unsigned short receivedCrc =
        dataArray[dataArray.size() - 4] | (dataArray[dataArray.size() - 3] << 8);
    unsigned short calculatedCrc = calculateChecksum(dataArray.data(), dataArray.size() - 4);
    if (receivedCrc != calculatedCrc) {
        DDL_Printf("CRC校验失败，丢弃数据。\n");
        return;
    }

    // 校验长度
    size_t calculatedSize = dataArray[2] | (dataArray[3] << 8);
    if (receivedSize != calculatedSize) {
        DDL_Printf("数据长度错误，丢弃数据。\n");
        return;
    }

    // 寻找对应的回调函数
    auto it = m_callbacks.find(dataArray[5]);
    if (it != m_callbacks.end()) {
        it->second(dataArray[6]);
    }
}

/**
 * @brief 注册接收回调函数
 * @param command 命令
 * @param callback 回调函数
 */
void UARTDriver::registerReceiveCallback(uint8_t command, std::function<void(uint8_t)> callback)
{
    m_callbacks[command] = std::move(callback);
}

/**
 * @brief 处理入口栏杆控制
 * @param data 数据
 */
void UARTDriver::handleEntranceBarrier(uint8_t data)
{
    auto motorController = DRV::MotorController::getInstance();
    EventGroupHandle_t motorEventGroupHandle = motorController->getEventGroupHandle();
    data == 0x01 ? xEventGroupSetBits(motorEventGroupHandle, EVENT_ENTRY_MOTOR_ON)
                 : xEventGroupSetBits(motorEventGroupHandle, EVENT_ENTRY_MOTOR_OFF);
}

/**
 * @brief 处理出口闸机控制
 * @param data 数据
 */
void UARTDriver::handleExitGate(uint8_t data)
{
    auto motorController = DRV::MotorController::getInstance();
    EventGroupHandle_t motorEventGroupHandle = motorController->getEventGroupHandle();
    data == 0x01 ? xEventGroupSetBits(motorEventGroupHandle, EVENT_EXIT_MOTOR_ON)
                 : xEventGroupSetBits(motorEventGroupHandle, EVENT_EXIT_MOTOR_OFF);
}

/**
 * @brief 处理报警灯控制
 * @param data 数据
 */
void UARTDriver::handleAlarmLight(uint8_t data)
{
    auto motorController = DRV::MotorController::getInstance();
    EventGroupHandle_t motorEventGroupHandle = motorController->getEventGroupHandle();
    data == 0x01 ? xEventGroupSetBits(motorEventGroupHandle, EVENT_ALARM_LED_ON)
                 : xEventGroupSetBits(motorEventGroupHandle, EVENT_ALARM_LED_OFF);
}

/**
 * @brief 处理故障灯控制
 * @param data 数据
 */
void UARTDriver::handleFaultLight(uint8_t data)
{
    auto motorController = DRV::MotorController::getInstance();
    EventGroupHandle_t motorEventGroupHandle = motorController->getEventGroupHandle();
    data == 0x01 ? xEventGroupSetBits(motorEventGroupHandle, EVENT_FAULT_LED_ON)
                 : xEventGroupSetBits(motorEventGroupHandle, EVENT_FAULT_LED_OFF);
}

} // namespace DRV
