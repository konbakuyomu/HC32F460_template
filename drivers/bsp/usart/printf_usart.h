/**
 * @file bsp_usart.h
 * @brief USART驱动头文件
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-09-13
 *
 * @details 包含USART驱动相关的声明和定义
 *
 */

#ifndef __PRINTF_USART_H
#define __PRINTF_USART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "hc32_ll.h"
#include "hc32f460.h"
#include "stdlib.h"
#include <string.h>

/* USART3 ------------------------------------------------------------------*/

/* USART配置 */
#define USART_BAUDRATE (115200UL) /**< USART波特率 */

/* USART引脚定义 */
#define USART_RX_PORT      (GPIO_PORT_B)  /**< USART接收端口 */
#define USART_RX_PIN       (GPIO_PIN_09)  /**< USART接收引脚 */
#define USART_RX_GPIO_FUNC (GPIO_FUNC_33) /**< USART接收引脚功能 */
#define USART_TX_PORT      (GPIO_PORT_E)  /**< USART发送端口 */
#define USART_TX_PIN       (GPIO_PIN_06)  /**< USART发送引脚 */
#define USART_TX_GPIO_FUNC (GPIO_FUNC_32) /**< USART发送引脚功能 */

/* USART控制引脚定义 */
#define CONTROL_PORT (GPIO_PORT_E)                               /**< USART控制端口 */
#define CONTROL_PIN  (GPIO_PIN_05)                               /**< USART控制引脚 */
#define CONTROL_TX() (GPIO_ResetPins(CONTROL_PORT, CONTROL_PIN)) /**< 设置USART为发送模式 */
#define CONTROL_RX() (GPIO_SetPins(CONTROL_PORT, CONTROL_PIN))   /**< 设置USART为接收模式 */

/* USART单元定义 */
#define USART_UNIT         (CM_USART3)                                          /**< USART单元 */
#define USART_FCG_ENABLE() (FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART3, ENABLE)) /**< 使能USART时钟 */

    /**
     * @brief USART配置函数
     *
     * @param vpDevice USART设备指针
     * @param u32Param USART参数（通常是波特率）
     * @return int32_t 配置结果，LL_OK表示成功
     */
    int32_t USART_Config(void *vpDevice, uint32_t u32Param);

/* USART4 ------------------------------------------------------------------*/

/* USART配置 */
#define USART4_BAUDRATE     (115200UL) /**< USART波特率 */
#define USART4_TIMEOUT_BITS (2000U)    /**< USART超时时间 */

/* USART引脚定义 */
#define USART4_RX_PORT      (GPIO_PORT_E)  /**< USART接收端口 */
#define USART4_RX_PIN       (GPIO_PIN_02)  /**< USART接收引脚 */
#define USART4_RX_GPIO_FUNC (GPIO_FUNC_37) /**< USART接收引脚功能 */
#define USART4_TX_PORT      (GPIO_PORT_E)  /**< USART发送端口 */
#define USART4_TX_PIN       (GPIO_PIN_03)  /**< USART发送引脚 */
#define USART4_TX_GPIO_FUNC (GPIO_FUNC_36)

/* USART DMA 配置 */
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

#define USART4_TX_DMA_UNIT         (CM_DMA2)
#define USART4_TX_DMA_CH           (DMA_CH0)
#define USART4_TX_DMA_FCG_ENABLE() (FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA2, ENABLE))
#define USART4_TX_DMA_TRIG_SEL     (AOS_DMA2_0)
#define USART4_TX_DMA_TRIG_EVT_SRC (EVT_SRC_USART4_TI)
#define USART4_TX_DMA_TC_INT       (DMA_INT_TC_CH0)
#define USART4_TX_DMA_TC_FLAG      (DMA_FLAG_TC_CH0)
#define USART4_TX_DMA_TC_IRQn      (INT001_IRQn)
#define USART4_TX_DMA_TC_INT_SRC   (INT_SRC_DMA2_TC0)

/* USART超时接收时钟配置 */
#define TMR0_UNIT_USART4         (CM_TMR0_2)
#define TMR0_CH_USART4           (TMR0_CH_B)
#define TMR0_USART4_FCG_ENABLE() (FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMR0_2, ENABLE))

/* USART单元定义 */
#define USART4_UNIT         (CM_USART4)
#define USART4_FCG_ENABLE() (FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART4, ENABLE))

/* USART中断定义 */
#define USART4_TX_CPLT_IRQn    (INT002_IRQn)
#define USART4_TX_CPLT_INT_SRC (INT_SRC_USART4_TCI) // USART4发送完成中断

#define USART4_RX_ERR_IRQn    (INT003_IRQn)
#define USART4_RX_ERR_INT_SRC (INT_SRC_USART4_EI) // USART4接收错误中断

#define USART4_RX_TIMEOUT_IRQn    (INT004_IRQn)
#define USART4_RX_TIMEOUT_INT_SRC (INT_SRC_USART4_RTO) // USART4接收超时中断

    void USART4_DMA_TX_Start(void *TxBuf, size_t m_u16TxLen);
    void USART4_init(void);

    void USART4_RxTimeout_IrqCallback(void);
    void USART4_RxError_IrqCallback(void);
    void USART4_RX_DMA_TC_IrqCallback(void);
    void USART4_TX_DMA_TC_IrqCallback(void);
    void USART4_TxComplete_IrqCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_USART_H */