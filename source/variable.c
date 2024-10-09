/**
 * @file  variable.c
 * @brief 全局变量定义
 * @details 包含FreeRTOS任务句柄等全局变量的定义
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "variable.h"

/*-------------------- FreeRTOS --------------------*/
/* 创建任务句柄 */
TaskHandle_t AppTaskCreate_Handle = NULL;
TaskHandle_t LEDTask_Handle = NULL;
TaskHandle_t TestTask_Handle = NULL;
