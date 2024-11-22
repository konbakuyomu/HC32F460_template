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
#define EntranceSensorExtInt (EXTINT_CH05)
#define EntranceSensorIRQn   (INT032_IRQn)
#define EntranceSensorIntSrc (INT_SRC_PORT_EIRQ5)
#define EntranceSensorPort   (GPIO_PORT_D)
#define EntranceSensorPin    (GPIO_PIN_05)

// 出口感应
#define ExitSensorExtInt (EXTINT_CH04)
#define ExitSensorIRQn   (INT034_IRQn)
#define ExitSensorIntSrc (INT_SRC_PORT_EIRQ4)
#define ExitSensorPort   (GPIO_PORT_D)
#define ExitSensorPin    (GPIO_PIN_04)

// 进门电机开启到位
#define EntranceMotorOpenToPositionExtInt (EXTINT_CH02)
#define EntranceMotorOpenToPositionIRQn   (INT033_IRQn)
#define EntranceMotorOpenToPositionIntSrc (INT_SRC_PORT_EIRQ2)
#define EntranceMotorOpenToPositionPort   (GPIO_PORT_D)
#define EntranceMotorOpenToPositionPin    (GPIO_PIN_02)

// 进门电机关闭到位
#define EntranceMotorCloseToPositionExtInt (EXTINT_CH01)
#define EntranceMotorCloseToPositionIRQn   (INT035_IRQn)
#define EntranceMotorCloseToPositionIntSrc (INT_SRC_PORT_EIRQ1)
#define EntranceMotorCloseToPositionPort   (GPIO_PORT_D)
#define EntranceMotorCloseToPositionPin    (GPIO_PIN_01)

/* 函数声明 ------------------------------------------------------------------*/
void initBspEirq(void);

#ifdef __cplusplus
}
#endif
