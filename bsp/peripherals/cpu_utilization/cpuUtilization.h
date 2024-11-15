/**
 * @file bsp_cpu.h
 * @brief FreeRTOS 统计CPU使用率头文件
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-09-13
 *
 * @details 包含FreeRTOS统计CPU利用率的声明和定义
 * @note
 * 根据监控提示，用于统计CPU使用率的高速定时器必须10倍与FreeRTOS的系统中断时间（默认是1ms）。因此定时器的中断时间至少在100us以内，等价于10kHZ频率
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "hc32_ll.h"
#include "stdlib.h"
#include <string.h>


/* 宏定义 ------------------------------------------------------------------*/

/* 统计CPU使用率TMR0定时器定义 */
#define CPU_TMR0_UNIT    (CM_TMR0_1)
#define CPU_TMR0_CLK     (FCG2_PERIPH_TMR0_1)
#define CPU_TMR0_CH      (TMR0_CH_B)
#define CPU_TMR0_CH_INT  (TMR0_INT_CMP_B)
#define CPU_TMR0_CH_FLAG (TMR0_FLAG_CMP_B)
#define CPU_TMR0_INT_SRC (INT_SRC_TMR0_1_CMP_B)
#define CPU_TMR0_IRQn    (INT006_IRQn)
/* Frequency = (Clock freq / div) / (Compare value + 1) */
/* 当前频率 = (32768 / 1) / (2 + 1) = 10.922Hz > 10kHz */
/* 用于统计CPU使用率的高速定时器必须10倍于FreeRTOS的系统中断时间（默认是1ms）。因此定时器的中断时间至少在100us以内，等价于10kHZ频率
 */
#define CPU_TMR0_CMP_VALUE (2UL)

/* 全局变量 ----------------------------------------------------------------*/
extern volatile uint32_t CPU_RunTime;

/* 函数声明 -------------------------------------------------------------------*/
void configCpuTmr0(void);
void startUtilization(void);

#ifdef __cplusplus
}
#endif
