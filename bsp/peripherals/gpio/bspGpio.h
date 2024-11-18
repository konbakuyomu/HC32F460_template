/**
 * @file bspGpio.h
 * @brief 通用GPIO底层驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-11-18
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "hc32_ll.h"

/* 宏定义 --------------------------------------------------------------------*/

// 出口电机控制引脚
#define ExitMotorControlPort          (GPIO_PORT_D)
#define ExitMotorControlPin           (GPIO_PIN_07)
#define PullUpExitMotorControlPin()   (GPIO_SetPins(ExitMotorControlPort, ExitMotorControlPin))
#define PullDownExitMotorControlPin() (GPIO_ResetPins(ExitMotorControlPort, ExitMotorControlPin))

/* 函数声明 ------------------------------------------------------------------*/
void initBspGpio(void);

#ifdef __cplusplus
}
#endif
