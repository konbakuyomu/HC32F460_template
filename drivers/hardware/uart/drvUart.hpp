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
#include "timers.h"
#include "hal.hpp"

namespace DRV
{
class UARTDriver
{
  public:
    enum class Command : uint8_t {
        ReportRealTimeData = 0x10,
        ControlEntranceGate = 0x20,
        ControlExitGate = 0x21,
        ControlAlarmLight = 0x22,
        ControlFaultLight = 0x23
    };

    enum class DataStatus : uint8_t { Off = 0x00, On = 0x01 };

  public:
    static UARTDriver *getInstance();
    static void sendPersonStandingStatus(TimerHandle_t xTimer);
    void initUARTHandlers();
    void processReceivedData(uint8_t *buffer, size_t size);
    UARTDriver() = default;
    ~UARTDriver() = default;

  private:
    // 禁止拷贝和赋值
    UARTDriver(const UARTDriver &) = delete;
    UARTDriver &operator=(const UARTDriver &) = delete;
    void registerReceiveCallback(uint8_t command, std::function<void(uint8_t)> callback);
    static unsigned short calculateChecksum(const uint8_t *receivedBuffer, size_t receivedSize);
    static std::unique_ptr<UARTDriver> m_usart;

    // 存储命令和对应的回调函数
    std::map<uint8_t, std::function<void(uint8_t)>> m_callbacks;

  private:
    void handleEntranceBarrier(uint8_t data);
    void handleExitGate(uint8_t data);
    void handleAlarmLight(uint8_t data);
    void handleFaultLight(uint8_t data);
};

}; // namespace DRV

#endif
