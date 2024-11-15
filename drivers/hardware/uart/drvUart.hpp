/**
 * @file drv_uart.h
 * @brief 串口驱动层实现
 * @version 1.0
 * @date 2024-11-07
 */

#pragma once
#ifdef __cplusplus

#include "hal.hpp"

namespace DRV
{
class USART
{
  private:
    static unsigned short calculateChecksum(uint8_t *buf, int len);
    static std::unique_ptr<USART> m_usart;
    static USART *getInstance();
    static void init();

  private:
    // CRC计算相关常量
    static constexpr uint16_t CRC_INIT = 0xFFFF;
    static constexpr uint16_t CRC_POLY = 0xA001; // 0x8005的循环右移16位结果

  public:
    void parseData(void *buf, uint32_t len);
    void sendData(void *buf, uint32_t len);
};

}; // namespace DRV

#endif
