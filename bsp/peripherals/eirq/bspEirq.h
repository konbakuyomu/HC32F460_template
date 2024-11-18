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

// 入口感应
#define EntranceSensorExtInt (EXTINT_CH06)
#define EntranceSensorIRQn   (INT032_IRQn)
#define EntranceSensorIntSrc (INT_SRC_PORT_EIRQ6)
#define EntranceSensorPort   (GPIO_PORT_D)
#define EntranceSensorPin    (GPIO_PIN_06)

// 出口感应
#define ExitSensorExtInt (EXTINT_CH04)
#define ExitSensorIRQn   (INT034_IRQn)
#define ExitSensorIntSrc (INT_SRC_PORT_EIRQ4)
#define ExitSensorPort   (GPIO_PORT_D)
#define ExitSensorPin    (GPIO_PIN_04)

// 身高感应
#define HeightSensorExtInt (EXTINT_CH05)
#define HeightSensorIRQn   (INT033_IRQn)
#define HeightSensorIntSrc (INT_SRC_PORT_EIRQ5)
#define HeightSensorPort   (GPIO_PORT_D)
#define HeightSensorPin    (GPIO_PIN_05)

// 占位感应
#define OccupancySensorExtInt (EXTINT_CH03)
#define OccupancySensorIRQn   (INT035_IRQn)
#define OccupancySensorIntSrc (INT_SRC_PORT_EIRQ3)
#define OccupancySensorPort   (GPIO_PORT_D)
#define OccupancySensorPin    (GPIO_PIN_03)

/* 函数声明 ------------------------------------------------------------------*/
void initBspEirq(void);

#ifdef __cplusplus
}
#endif
