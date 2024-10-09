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

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include "hc32_ll.h"
#include "ev_hc32f460_lqfp100_v2_bsp.h"

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
#include "queue.h"
#include "event_groups.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* bsp 头文件 */
#include "bsp_usart.h"

/* 宏定义 ------------------------------------------------------------------*/


/* 联合体 ------------------------------------------------------------------*/
union Float_T4o_4Byte
{
    unsigned char array[4];
    float buff;
};

/* 结构体 ------------------------------------------------------------------*/


/* 全局变量 ----------------------------------------------------------------*/
extern TaskHandle_t AppTaskCreate_Handle;    
extern TaskHandle_t LEDTask_Handle;
extern TaskHandle_t TestTask_Handle;

#ifdef __cplusplus
}
#endif

#endif
