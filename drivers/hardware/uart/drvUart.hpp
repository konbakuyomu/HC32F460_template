/**
 * @file drv_uart.h
 * @brief 串口驱动层实现
 * @version 1.0
 * @date 2024-11-07
 */

#pragma once
#ifdef __cplusplus

#include "FreeRTOS.h"
#include "event_groups.h"
#include "hal.hpp"
#include "queue.h"
#include "timers.h"

namespace DRV
{
class UARTDriver
{
  public:
    // 枚举定义
    enum class Command : uint8_t {
        ReportRealTimeData = 0x10,
        ControlEntranceGate = 0x20,
        ControlExitGate = 0x21,
        ControlAlarmLight = 0x22,
        ControlFaultLight = 0x23
    };

    enum class DataStatus : uint8_t { Off = 0x00, On = 0x01 };

    // 添加队列相关方法
    void createTxQueue();
    QueueHandle_t getTxQueue() const { return m_uartTxQueue; }
    bool sendToQueue(const uint8_t *data, size_t length);

    // 构造和析构函数
    UARTDriver();
    ~UARTDriver();

    // 禁止拷贝和赋值
    UARTDriver(const UARTDriver &) = delete;
    UARTDriver &operator=(const UARTDriver &) = delete;

    // 静态成员函数
    static UARTDriver *getInstance()
    {
        if (!m_usart) {
            m_usart = std::make_unique<UARTDriver>();
        }
        return m_usart.get();
    }
    static void sendPersonStandingStatus(en_pin_state_t pin_state);
    static unsigned short calculateChecksum(const uint8_t *receivedBuffer, size_t receivedSize);

    // 公共成员函数
    void initUARTHandlers();
    void processReceivedData(uint8_t *buffer, size_t size);

  private:
    // 私有成员函数
    void registerReceiveCallback(uint8_t command, std::function<void(uint8_t)> callback);
    void handleEntranceBarrier(uint8_t data);
    void handleExitGate(uint8_t data);
    void handleAlarmLight(uint8_t data);
    void handleFaultLight(uint8_t data);

    // 私有成员变量
    static std::unique_ptr<UARTDriver> m_usart;
    std::map<uint8_t, std::function<void(uint8_t)>> m_callbacks; // 存储命令和对应的回调函数
    QueueHandle_t m_uartTxQueue{nullptr};                        // 串口发送队列句柄
};

}; // namespace DRV

#endif
