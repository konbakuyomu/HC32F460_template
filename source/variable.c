/**
 * @file  variable.c
 * @brief ȫ�ֱ�������
 * @details ����FreeRTOS��������ȫ�ֱ����Ķ���
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "variable.h"

/*-------------------- FreeRTOS --------------------*/
/* ���������� */
TaskHandle_t AppTaskCreate_Handle = NULL;
TaskHandle_t LEDTask_Handle = NULL;
TaskHandle_t TestTask_Handle = NULL;
