/**
 * @file bsp_uart.h
 * @brief 串口底层驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-10-11
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "hc32_ll.h"

/* 宏定义 --------------------------------------------------------------------*/

/* 定时器TMR0配置(超时接收) */
#define TMR0_UNIT_USART4         (CM_TMR0_2)
#define TMR0_CH_USART4           (TMR0_CH_B)
#define TMR0_USART4_FCG_ENABLE() (FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMR0_2, ENABLE))

/* 接收 RX DMA 配置 */
#define USART4_RX_DMA_UNIT                (CM_DMA1)
#define USART4_RX_DMA_CH                  (DMA_CH0)
#define USART4_RX_DMA_FCG_ENABLE()        (FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA1, ENABLE))
#define USART4_RX_DMA_TRIG_SEL            (AOS_DMA1_0)
#define USART4_RX_DMA_TRIG_EVT_SRC        (EVT_SRC_USART4_RI)
#define USART4_RX_DMA_RECONF_TRIG_SEL     (AOS_DMA_RC)
#define USART4_RX_DMA_RECONF_TRIG_EVT_SRC (EVT_SRC_AOS_STRG)
#define USART4_RX_DMA_TC_INT              (DMA_INT_TC_CH0)
#define USART4_RX_DMA_TC_FLAG             (DMA_FLAG_TC_CH0)
#define USART4_RX_DMA_TC_IRQn             (INT000_IRQn)
#define USART4_RX_DMA_TC_INT_SRC          (INT_SRC_DMA1_TC0)

/* 发送 TX DMA 配置 */
#define USART4_TX_DMA_UNIT         (CM_DMA2)
#define USART4_TX_DMA_CH           (DMA_CH0)
#define USART4_TX_DMA_FCG_ENABLE() (FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA2, ENABLE))
#define USART4_TX_DMA_TRIG_SEL     (AOS_DMA2_0)
#define USART4_TX_DMA_TRIG_EVT_SRC (EVT_SRC_USART4_TI)
#define USART4_TX_DMA_TC_INT       (DMA_INT_TC_CH0)
#define USART4_TX_DMA_TC_FLAG      (DMA_FLAG_TC_CH0)
#define USART4_TX_DMA_TC_IRQn      (INT001_IRQn)
#define USART4_TX_DMA_TC_INT_SRC   (INT_SRC_DMA2_TC0)

/* RX/TX 引脚配置 */
#define USART4_RX_PORT      (GPIO_PORT_B)
#define USART4_RX_PIN       (GPIO_PIN_06)
#define USART4_RX_GPIO_FUNC (GPIO_FUNC_37)
#define USART4_TX_PORT      (GPIO_PORT_B)
#define USART4_TX_PIN       (GPIO_PIN_07)
#define USART4_TX_GPIO_FUNC (GPIO_FUNC_36)

/* 485控制引脚配置 */
#define USART4_485_EN_PORT     (GPIO_PORT_B)
#define USART4_485_EN_PIN      (GPIO_PIN_05)
#define USART4_485_TX_ENABLE() (GPIO_ResetPins(USART4_485_EN_PORT, USART4_485_EN_PIN))
#define USART4_485_RX_ENABLE() (GPIO_SetPins(USART4_485_EN_PORT, USART4_485_EN_PIN))

/* UART4 中断配置 */
#define USART4_TX_CPLT_IRQn       (INT002_IRQn)
#define USART4_TX_CPLT_INT_SRC    (INT_SRC_USART4_TCI) // USART4发送完成中断
#define USART4_RX_ERR_IRQn        (INT003_IRQn)
#define USART4_RX_ERR_INT_SRC     (INT_SRC_USART4_EI) // USART4接收错误中断
#define USART4_RX_TIMEOUT_IRQn    (INT004_IRQn)
#define USART4_RX_TIMEOUT_INT_SRC (INT_SRC_USART4_RTO) // USART4接收超时中断

/* UART4 配置 */
#define USART4_UNIT         (CM_USART4)
#define USART4_FCG_ENABLE() (FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART4, ENABLE))
#define USART4_BAUDRATE     (115200UL)

/* 函数声明 ------------------------------------------------------------------*/
void USART4_Init();
void USART4_SendData(void *data, size_t length);

#ifdef __cplusplus
}
#endif
