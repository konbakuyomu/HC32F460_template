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
TaskHandle_t AppTaskCreate_Handle = NULL;
TaskHandle_t LEDTask_Handle = NULL;
TaskHandle_t UartTask_Handle = NULL;
TaskHandle_t CANTXTask_Handle = NULL;
TaskHandle_t CANRXTask_Handle = NULL;
QueueHandle_t xQueue_CanTx = NULL;

/*------------------- 键值对线程锁 -------------------*/
static SemaphoreHandle_t key_value_mutex_handle = NULL;

/*---------------------- 串口 ----------------------*/
uint8_t Host_RxBuf[ARRAY_LENGTH];
uint8_t Host_TxBuf[ARRAY_LENGTH];

/**
 * @brief 获取键值对线程锁
 * @return void
 */
static void key_value_take_key() { xSemaphoreTake(key_value_mutex_handle, portMAX_DELAY); }

/**
 * @brief 释放键值对线程锁
 * @return void
 */
static void key_value_give_key() { xSemaphoreGive(key_value_mutex_handle); }

void key_value_mutex_init(void)
{
    key_value_mutex_handle = xSemaphoreCreateMutex();

    key_value_mutex_cb_t cfg = {
        .mutex_get_cb = key_value_take_key,
        .mutex_give_cb = key_value_give_key,
    };
    key_value_mutex_register(&cfg);
}

void Key_Value_Init(void)
{
    // pwm
    key_value_register(NULL, "PWM_start", PWM_start);
    key_value_register(NULL, "PWM_stop", PWM_stop);
    key_value_register(NULL, "PWM_setDutyCycle", PWM_setDutyCycle);
    key_value_register(NULL, "PWM_setFrequency", PWM_setFrequency);
    key_value_register(NULL, "PWM_setDirection", PWM_setDirection);
    key_value_register(NULL, "PWM_getDirection", PWM_getDirection);

    // uart
    key_value_register(NULL, "USART4_SendData", USART_SendData);
    // key_value_register(NULL, "USART4_SendData", USART4_DMA_TX_Start);
}

void INTC_IrqInstalHandler(const stc_irq_signin_config_t *pstcConfig, uint32_t u32Priority)
{
    if (NULL != pstcConfig)
    {
        (void)INTC_IrqSignIn(pstcConfig);
        NVIC_ClearPendingIRQ(pstcConfig->enIRQn);
        NVIC_SetPriority(pstcConfig->enIRQn, u32Priority);
        NVIC_EnableIRQ(pstcConfig->enIRQn);
    }
}

uint8_t *init_dynamic_buffer(uint8_t **buffer, size_t size)
{
    if (buffer == NULL || size == 0)
    {
        return NULL;
    }

    *buffer = (uint8_t *)mymalloc(size);

    if (*buffer != NULL)
    {
        memset(*buffer, 0, size);
        return *buffer;
    } else
    {
        // 内存分配失败，可以在这里添加错误处理代码
        // 例如：打印错误信息或触发系统重置
        return NULL;
    }
}