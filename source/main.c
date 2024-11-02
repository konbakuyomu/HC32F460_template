/**
 * @file  main.c
 * @brief 主程序入口
 * @details 包含主函数和系统初始化
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "variable.h"

/**
 * @brief  时钟初始化
 * @param  无
 * @retval 无
 * @note
 */
static void CLK_Init(void)
{
    /* 将系统时钟配置为200MHz */
    BSP_CLK_Init();
    /* 外部时钟源32kb初始化 */
    BSP_XTAL32_Init();
}

/**
 * @brief  主函数
 * @details 初始化系统，创建任务，启动调度器
 * @param  无
 * @retval int32_t
 *         - 0: 正常退出
 *         - -1: 任务创建失败
 * @note   正常情况下不会返回，会一直运行在FreeRTOS调度器中
 */
int32_t main(void)
{
    /* 对指定的外设寄存器解锁（解锁后才能写入） */
    LL_PERIPH_WE(LL_PERIPH_ALL);
    /* 时钟初始化 */
    CLK_Init();
    /* 定义一个创建信息返回值，默认为pdPASS */
    BaseType_t xReturn = pdPASS;

    /* 创建任务创建任务 */
    xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,          /* 任务入口函数 */
                          (const char *)"AppTaskCreate",          /* 任务名字 */
                          (uint16_t)256,                          /* 任务栈大小 */
                          (void *)NULL,                           /* 任务入口函数参数 */
                          (UBaseType_t)3,                         /* 任务的优先级 */
                          (TaskHandle_t *)&AppTaskCreate_Handle); /* 任务控制块指针 */

    /* 启动任务调度 */
    if (pdPASS == xReturn)
    {
        vTaskStartScheduler(); /* 启动任务，开启调度 */
    } else
        return -1;
    /* 正常不会执行到这里 */
    for (;;)
    {
    }
}
