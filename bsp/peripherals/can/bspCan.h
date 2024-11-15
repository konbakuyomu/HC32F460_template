/**
 * @file bsp_can.h
 * @brief CAN底层驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-10-24
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "hc32_ll.h"

/* 宏定义 ------------------------------------------------------------------*/

/* CAN配置 */
#define CAN_UNIT        (CM_CAN)
#define CAN_PERIPH_CLK  (FCG1_PERIPH_CAN)
#define CAN_TX_PORT     (GPIO_PORT_B)
#define CAN_TX_PIN      (GPIO_PIN_04)
#define CAN_TX_PIN_FUNC (GPIO_FUNC_50)
#define CAN_RX_PORT     (GPIO_PORT_B)
#define CAN_RX_PIN      (GPIO_PIN_03)
#define CAN_RX_PIN_FUNC (GPIO_FUNC_51)
#define CAN_INT_PRIO    (DDL_IRQ_PRIO_DEFAULT)
#define CAN_INT_SRC     (INT_SRC_CAN_INT)
#define CAN_INT_IRQn    (INT122_IRQn)

/* CAN 中断类型选择 */
#define CAN_INT_SEL                                                                                \
    (CAN_INT_STB_TX | CAN_INT_PTB_TX | CAN_INT_RX_BUF_WARN | CAN_INT_RX_BUF_FULL |                 \
     CAN_INT_RX_OVERRUN | CAN_INT_RX)

/* 设备ID，扩展帧，过滤器掩码 */
#define DEVICE_ID                       (8U)           // 设备ID，可根据需要修改
#define CAN_ID_TYPE_QUERY_REALTIME_DATA (CAN_ID_EXT)   // 只接受扩展帧
#define CAN_FILTER_SEL                  (CAN_FILTER1)  // 过滤器选择
#define CAN_MASK_QUERY_REALTIME_DATA    (0x0F000000UL) // 过滤器掩码，可以接受扩展帧ID: x082010

/* CAN_RX 接收过滤器 */
#define CAN_ID_QUERY_REALTIME_DATA                                                                 \
    (0x10 << 20 | DEVICE_ID << 16 | 0x2010)                              // 查询探头实时数据指令 ID
#define CAN_ID_SET_PROBE_VOLTAGE (0x20 << 20 | DEVICE_ID << 16 | 0x2010) // 设置探头高压指令 ID
#define CAN_ID_SET_PROBE_THRESHOLD                                                                 \
    (0x30 << 20 | DEVICE_ID << 16 | 0x2010) // 设置探头甄别阈值指令 ID
#define CAN_ID_QUERY_PROBE_VOLTAGE (0x40 << 20 | DEVICE_ID << 16 | 0x2010) // 查询探头高压指令 ID
#define CAN_ID_QUERY_PROBE_THRESHOLD                                                               \
    (0x50 << 20 | DEVICE_ID << 16 | 0x2010) // 查询探头甄别阈值指令 ID

/* CAN_TX 发送 */
#define CAN_ID_REPLY_REALTIME_DATA                                                                 \
    (0x10 << 20 | DEVICE_ID << 16 | 0x1020) // 回复探头实时数据指令 ID
#define CAN_ID_REPLY_PROBE_VOLTAGE (0x40 << 20 | DEVICE_ID << 16 | 0x1020) // 回复探头高压指令 ID
#define CAN_ID_REPLY_PROBE_THRESHOLD                                                               \
    (0x50 << 20 | DEVICE_ID << 16 | 0x1020) // 回复探头甄别阈值指令 ID

/* CAN_TX 发送数据容量 */
#define CAN_TX_IDE (1UL)

/* 接收缓冲区大小 */
#define CAN_RX_FRAME_NUM (50U)

/* 函数定义 ----------------------------------------------------------------*/
void initBspCanImpl(void);

#ifdef __cplusplus
}
#endif
