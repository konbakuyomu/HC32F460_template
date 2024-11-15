/**
 * @file drv_uart.cpp
 * @brief 串口驱动层实现
 * @version 1.0
 * @date 2024-11-08
 */

#include "globalConfig.h"

namespace DRV
{

void USART::init() { m_usart = std::make_unique<USART>(); }

unsigned short USART::calculateChecksum(uint8_t *buf, int len)
{
    unsigned short crc = 0xFFFF; // 初始值为0xFFFF

    for (uint8_t i = 0; i < len; ++i) {

        unsigned short byte = (buf[i] + 256) % 256;
        crc ^= byte; // 异或操作

        for (unsigned short j = 0; j < 8; ++j) { // 每个字节8位
            if ((crc & 1) != 0) {
                crc >>= 1;
                crc ^= 0xA001; // 0xA001是0x8005循环右移16位的结果
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

} // namespace DRV
