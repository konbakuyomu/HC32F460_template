/**
 * @file globalConfig.h
 * @brief 全局变量和头文件包含的中转总集
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-09-13
 *
 * @details 本文件作为项目的中央头文件，包含了所有常用的标准库、
 *          驱动库、FreeRTOS相关头文件，以及全局变量的声明。
 *          它起到了一个中转站的作用，方便其他源文件统一包含所需的头文件和全局变量。
 */
#pragma once

/* 纯C ------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

#include "ev_hc32f460_lqfp100_v2_bsp.h"
#include "hc32_ll.h"
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

/* 外部驱动库头文件 */
#include "appManage.h"
#include "beatTask.h"
#include "bmp280.h"
#include "button.h"
#include "keyValueTransation.h"
#include "ringBuffer.h"
#include <wire.h>

/* freertos 头文件 */
#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

/* CPU利用率头文件 */
#include "cpuUtilization.h"

/* bsp 头文件 */
#include "bspCan.h"
#include "bspEirq.h"
#include "bspGpio.h"
#include "bspUart4.h"
#include "printfUart.h"

/* 宏定义 */
#define SYSTEM_PCLK1_FREQUENCY_HZ           (100000000U) // 系统 PCLK1 时钟频率:100 MHz
#define MOTOR_DIRECTION_FORWARD             (0)          // 正转
#define MOTOR_DIRECTION_REVERSE             (1)          // 反转
#define HOST_BUFFER_LENGTH                  (50U)        // 数组长度
#define USART4_CONFIG                       (0)          // USART4配置
#define EVENT_DOOR_ENTRY_SENSOR             (1 << 0)     // 进门感应事件
#define EVENT_DOOR_EXIT_SENSOR              (1 << 1)     // 出门感应事件
#define EVENT_ENTRY_MOTOR_OPEN_TO_POSITION  (1 << 2)     // 进门电机开到位
#define EVENT_ENTRY_MOTOR_CLOSE_TO_POSITION (1 << 3)     // 进门电机关到位
#define EVENT_ENTRY_MOTOR_ON                (1 << 4)     // 进门电机开启
#define EVENT_ENTRY_MOTOR_OFF               (1 << 5)     // 进门电机关闭
#define EVENT_EXIT_MOTOR_ON                 (1 << 6)     // 出门电机开启
#define EVENT_EXIT_MOTOR_OFF                (1 << 7)     // 出门电机关闭
#define EVENT_ALARM_LED_ON                  (1 << 8)     // 报警指示灯开启
#define EVENT_ALARM_LED_OFF                 (1 << 9)     // 报警指示灯关闭
#define EVENT_FAULT_LED_ON                  (1 << 10)    // 故障指示灯开启
#define EVENT_FAULT_LED_OFF                 (1 << 11)    // 故障指示灯关闭
#define EVENT_EMERGENCY_STOP                (1 << 12)    // 紧急停止事件

/* 联合体 ------------------------------------------------------------------*/

/**
 * @brief 整型与字节数组之间的转换联合体
 */
typedef union {
    int32_t value;
    uint8_t bytes[4];
} Int32ByteConverter;

/**
 * @brief 浮点型与字节数组之间的转换联合体
 */
typedef union {
    float value;
    uint8_t bytes[4];
} FloatByteConverter;

/* 结构体 ------------------------------------------------------------------*/

/**
 * @brief Uart发送数据结构体
 */
typedef struct {
    uint8_t *data;
    size_t length;
} stc_uart_tx_frame_t;

/* 全局变量 ----------------------------------------------------------------*/
extern uint8_t hostReceiveBuffer[HOST_BUFFER_LENGTH];
extern uint8_t hostSendBuffer[HOST_BUFFER_LENGTH];

/* 函数 -------------------------------------------------------------------*/
void installInterruptHandler(const stc_irq_signin_config_t *pstcConfig, uint32_t u32Priority);
uint8_t *createDynamicBuffer(uint8_t **buffer, size_t size);

#ifdef __cplusplus
}
#endif

/* C++ ------------------------------------------------------------------*/

/* bsp 头文件 */
#include "bspLed.hpp"
#include "bspPwm.hpp"

/* drv 头文件 */
#include "drvCan.hpp"
#include "drvHal.hpp"
#include "drvLed.hpp"
#include "drvMotor.hpp"
#include "drvUart.hpp"
#include "keyValueMap.hpp"
#include "systemInit.hpp"

/* app 头文件 */
#include "baseTask.hpp"
#include "canTask.hpp"
#include "ledTask.hpp"
#include "motorTask.hpp"
#include "uartTask.hpp"
#include "taskFactory.hpp"

/* 硬件抽象层 头文件 */
#include "hal.hpp"
