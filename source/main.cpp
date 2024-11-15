/**
 * @file  main.c
 * @brief 主程序入口
 * @details 包含主函数和系统初始化
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "globalConfig.h"

/**
 * @brief 系统初始化
 * @param 无
 * @retval 无
 * @note hc32f460_system_init()函数用于初始化系统时钟、外部时钟源、CPU使用率统计和串口printf
 */
static void hc32f460SystemInit(void)
{
    /* 将系统时钟配置为200MHz */
    BSP_CLK_Init();
    /* 外部时钟源32kb初始化 */
    BSP_XTAL32_Init();
    /* CPU使用率统计初始化 */
    configCpuTmr0();
    /* 开启CPU使用率统计 */
    startUtilization();
    /* 串口printf初始化 -> PB9:USART4_RX  PE6:USART4_TX */
    LL_PrintfInit(USART_UNIT, USART_BAUDRATE, usartConfig);
}

/**
 * @brief  主函数
 * @details 初始化系统，创建任务，启动调度器
 * @retval int
 * @note   正常情况下不会返回，会一直运行在FreeRTOS调度器中
 */
int main(void)
{
    /* 对指定的外设寄存器解锁（解锁后才能写入） */
    LL_PERIPH_WE(LL_PERIPH_ALL);
    /* 系统初始化 */
    hc32f460SystemInit();
    /* 定义一个创建信息返回值，默认为pdPASS */
    BaseType_t xReturn = pdPASS;

    /* 创建任务创建任务 */
    xReturn =
        xTaskCreate((TaskFunction_t)TaskCreator::AppTaskCreate, (const char *)"AppTaskCreate", (uint16_t)256,
                    (void *)NULL, (UBaseType_t)3, (TaskHandle_t *)&appTaskCreateHandle);

    /* 启动任务调度 */
    if (pdPASS == xReturn) {
        vTaskStartScheduler();
    } else
        return -1;
    /* 正常不会执行到这里 */
    for (;;) {
    }
}
