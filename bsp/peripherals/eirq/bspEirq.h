/**
 * @file bsp_eirq.h
 * @brief 外部中断底层驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-11-05
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "hc32_ll.h"

/* 宏定义 --------------------------------------------------------------------*/

#define RX_JM_EXTINT  (EXTINT_CH06)
#define RX_JM_IRQn    (INT032_IRQn)
#define RX_JM_INT_SRC (INT_SRC_PORT_EIRQ6)
#define RX_JM_PORT    (GPIO_PORT_D)
#define RX_JM_PIN     (GPIO_PIN_06)

#define RX_CM_EXTINT  (EXTINT_CH04)
#define RX_CM_IRQn    (INT034_IRQn)
#define RX_CM_INT_SRC (INT_SRC_PORT_EIRQ4)
#define RX_CM_PORT    (GPIO_PORT_D)
#define RX_CM_PIN     (GPIO_PIN_04)

#define RX_SG_EXTINT  (EXTINT_CH05)
#define RX_SG_IRQn    (INT033_IRQn)
#define RX_SG_INT_SRC (INT_SRC_PORT_EIRQ5)
#define RX_SG_PORT    (GPIO_PORT_D)
#define RX_SG_PIN     (GPIO_PIN_05)

#define RX_ZW_EXTINT  (EXTINT_CH03)
#define RX_ZW_IRQn    (INT035_IRQn)
#define RX_ZW_INT_SRC (INT_SRC_PORT_EIRQ3)
#define RX_ZW_PORT    (GPIO_PORT_D)
#define RX_ZW_PIN     (GPIO_PIN_03)

/* 函数声明 ------------------------------------------------------------------*/
void initBspEirq(void);

#ifdef __cplusplus
}
#endif
