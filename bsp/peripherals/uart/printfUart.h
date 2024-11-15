/**
 * @file printf_usart.h
 * @brief 打印串口底层驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-09-13
 *
 * @details 包含打印串口相关的声明和定义
 *
 */

#ifndef __PRINTF_USART_H
#define __PRINTF_USART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hc32_ll.h"
#include "hc32f460.h"
#include "stdlib.h"
#include <string.h>

/* 宏定义 --------------------------------------------------------------------*/

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
#define USART_UNIT (CM_USART3) /**< USART单元 */
#define USART_FCG_ENABLE()                                                                         \
    (FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART3, ENABLE)) /**< 使能USART时钟 */

/* 函数声明 ------------------------------------------------------------------*/
int32_t usartConfig(void *vpDevice, uint32_t u32Param);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_USART_H */