/**
 * @file variable.h
 * @brief 全局变量和头文件包含的中转总集
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-09-13
 *
 * @details 本文件作为项目的中央头文件，包含了所有常用的标准库、
 *          驱动库、FreeRTOS相关头文件，以及全局变量的声明。
 *          它起到了一个中转站的作用，方便其他源文件统一包含所需的头文件和全局变量。
 */

#ifndef __VARIABLE_H
#define __VARIABLE_H

/* bsp 头文件 */
#include "bsp_pwm.h"
#include "bsp_uart.h"
#include "bsp_can.h"
#include "bsp_led.h"

/* drv 头文件 */
#include "drv_hal.h"

/* app 头文件 */
#include "taskManage.h"

/* 硬件抽象层 头文件 */
#include "hal.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "ev_hc32f460_lqfp100_v2_bsp.h"
#include "hc32_ll.h"
#include "main.h"
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 内存池管理头文件 */
#include "malloc.h"

/* CPU利用率头文件 */
#include "bsp_cpu.h"

/* 外部驱动库头文件 */
#include "app_manage.h"
#include "beat_task.h"
#include "bmp280.h"
#include "button.h"
#include "key_value_transation.h"
#include "ring_buffer.h"
#include <WIRE.h>

/* freertos 头文件 */
#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

/* bsp 头文件 */
#include "printf_usart.h"

    /* 宏定义 ------------------------------------------------------------------*/

#define HCLK1_FREQ      (100000000U) // 100 MHz
#define PWM_DIR_FORWARD (0)          // 正转
#define PWM_DIR_REVERSE (1)          // 反转
#define ARRAY_LENGTH    (50U)        // 数组长度
#define PWM_CONFIG_1    (0)          // 配置1
#define PWM_CONFIG_2    (1)          // 配置2
#define PWM_CONFIG_3    (2)          // 配置3
#define PWM_CONFIG_4    (3)          // 配置4
#define USART4_CONFIG   (0)          // USART4配置

    /* 联合体 ------------------------------------------------------------------*/

    /**
     * @brief 浮点数与字节数组之间的转换联合体
     */
    union Float_T4o_4Byte {
        unsigned char array[4];
        float buff;
    };

    /* 结构体 ------------------------------------------------------------------*/

    /* 全局变量 ----------------------------------------------------------------*/
    extern TaskHandle_t AppTaskCreate_Handle;
    extern TaskHandle_t LEDTask_Handle;
    extern TaskHandle_t UartTask_Handle;
    extern TaskHandle_t CANTXTask_Handle;
    extern TaskHandle_t CANRXTask_Handle;
    extern QueueHandle_t xQueue_CanTx;
    extern uint8_t Host_RxBuf[ARRAY_LENGTH];
    extern uint8_t Host_TxBuf[ARRAY_LENGTH];
    /* 函数 -------------------------------------------------------------------*/

    void INTC_IrqInstalHandler(const stc_irq_signin_config_t *pstcConfig, uint32_t u32Priority);
    uint8_t *init_dynamic_buffer(uint8_t **buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif
