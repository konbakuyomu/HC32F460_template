/**
 * @file uartTask.cpp
 * @brief Uart任务类
 * @version 1.0
 * @date 2024-11-22
 */
#include "globalConfig.h"

namespace APP
{

std::unique_ptr<UartTask> UartTask::mInstance;

/**
 * @brief Uart发送任务
 * @param pvParameters 参数
 */
void UartTask::txTask(void *pvParameters)
{
    auto uartDriver = DRV::UARTDriver::getInstance();
    QueueHandle_t txQueue = uartDriver->getTxQueue();

    if (txQueue == nullptr) {
        vTaskDelete(nullptr);
        return;
    }

    stc_uart_tx_frame_t tx_frame;
    bool firstRun = true;

    for (;;) {
        while (xQueueReceive(txQueue, &tx_frame, portMAX_DELAY) == pdTRUE) {
            if (!firstRun) {
                ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            } else {
                firstRun = false;
            }

            handleTxFrame(tx_frame);
        }
    }
}

/**
 * @brief Uart接收任务
 * @param pvParameters 参数
 */
void UartTask::rxTask(void *pvParameters)
{
    uint32_t notificationValue;

    for (;;) {
        if (xTaskNotifyWait(0, ULONG_MAX, &notificationValue, portMAX_DELAY) == pdPASS) {
            handleRxData(notificationValue);
        }
    }
}

/**
 * @brief 处理Uart发送数据
 * @param frame 发送数据结构体
 */
void UartTask::handleTxFrame(const stc_uart_tx_frame_t &frame)
{
    HAL::sendUart(frame.data, frame.length);
}

/**
 * @brief 处理Uart接收数据
 * @param length 接收数据长度
 */
void UartTask::handleRxData(uint32_t length)
{
    uint16_t receivedLength = static_cast<uint16_t>(length);
    auto uartDriver = DRV::UARTDriver::getInstance();
    uartDriver->processReceivedData(hostReceiveBuffer, receivedLength);
}

} // namespace APP

#ifdef __cplusplus
extern "C" {
#endif

TaskHandle_t UartTask_GetUartRxTaskHandle(void)
{
    return APP::UartTask::getInstance()->task1Handle;
}

TaskHandle_t UartTask_GetUartTxTaskHandle(void)
{
    return APP::UartTask::getInstance()->task2Handle;
}

#ifdef __cplusplus
}
#endif

