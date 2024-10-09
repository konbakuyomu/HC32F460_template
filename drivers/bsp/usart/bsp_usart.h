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

#ifndef __BSP_USART_H
#define __BSP_USART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include "stdlib.h"
#include "hc32_ll.h"

/* USART配置 */
#define USART_BAUDRATE                  (115200UL)                                  /**< USART波特率 */

/* USART引脚定义 */
#define USART_RX_PORT                   (GPIO_PORT_B)                               /**< USART接收端口 */
#define USART_RX_PIN                    (GPIO_PIN_09)                               /**< USART接收引脚 */
#define USART_RX_GPIO_FUNC              (GPIO_FUNC_37)                              /**< USART接收引脚功能 */
#define USART_TX_PORT                   (GPIO_PORT_E)                               /**< USART发送端口 */
#define USART_TX_PIN                    (GPIO_PIN_06)                               /**< USART发送引脚 */
#define USART_TX_GPIO_FUNC              (GPIO_FUNC_36)                              /**< USART发送引脚功能 */

/* USART控制引脚定义 */
#define CONTROL_PORT                    (GPIO_PORT_E)                               /**< USART控制端口 */
#define CONTROL_PIN                     (GPIO_PIN_05)                               /**< USART控制引脚 */
#define CONTROL_TX()                    (GPIO_ResetPins(CONTROL_PORT, CONTROL_PIN)) /**< 设置USART为发送模式 */
#define CONTROL_RX()                    (GPIO_SetPins(CONTROL_PORT, CONTROL_PIN))   /**< 设置USART为接收模式 */

/* USART单元定义 */
#define USART_UNIT                      (CM_USART4)                                 /**< USART单元 */
#define USART_FCG_ENABLE()              (FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART4, ENABLE)) /**< 使能USART时钟 */

/**
 * @brief USART配置函数
 * 
 * @param vpDevice USART设备指针
 * @param u32Param USART参数（通常是波特率）
 * @return int32_t 配置结果，LL_OK表示成功
 */
int32_t USART_Config(void *vpDevice, uint32_t u32Param);


#ifdef __cplusplus
}
#endif

#endif /* __BSP_USART_H */