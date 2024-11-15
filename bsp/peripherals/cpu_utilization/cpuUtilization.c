/**
 * @file bsp_cpu.c
 * @brief FreeRTOS 统计CPU使用率头文件
 * @details 包含FreeRTOS统计CPU利用率的实现
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "globalConfig.h"

volatile uint32_t CPU_RunTime = 0;

static void CPU_TMR0_IRQHandler(void)
{
    CPU_RunTime++;
    TMR0_ClearStatus(CPU_TMR0_UNIT, CPU_TMR0_CH_FLAG);
}

/**
 * @brief 初始化CPU使用率统计
 * @details 初始化CPU使用率统计
 * @param 无
 * @return 无
 */
void configCpuTmr0(void)
{
    stc_tmr0_init_t stcTmr0Init;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* 使能 timer0 时钟 */
    FCG_Fcg2PeriphClockCmd(CPU_TMR0_CLK, ENABLE);

    /* 配置 timer0 */
    (void)TMR0_StructInit(&stcTmr0Init);
    stcTmr0Init.u32ClockSrc = TMR0_CLK_SRC_XTAL32;
    stcTmr0Init.u32ClockDiv = TMR0_CLK_DIV1;
    stcTmr0Init.u32Func = TMR0_FUNC_CMP;
    stcTmr0Init.u16CompareValue = CPU_TMR0_CMP_VALUE;
    (void)TMR0_Init(CPU_TMR0_UNIT, CPU_TMR0_CH, &stcTmr0Init);

    /* 异步时钟源，写入TMR0寄存器需要等待三个异步时钟。 */
    DDL_DelayMS(1U);
    TMR0_IntCmd(CPU_TMR0_UNIT, CPU_TMR0_CH_INT, ENABLE);

    /* 配置中断 */
    stcIrqSignConfig.enIntSrc = CPU_TMR0_INT_SRC;
    stcIrqSignConfig.enIRQn = CPU_TMR0_IRQn;
    stcIrqSignConfig.pfnCallback = &CPU_TMR0_IRQHandler;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
    NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);
}

/**
 * @brief 开始统计CPU使用率
 * @details 开始统计CPU使用率
 * @param 无
 * @return 无
 */
void startUtilization(void)
{
    TMR0_Start(CPU_TMR0_UNIT, CPU_TMR0_CH);
    DDL_DelayMS(1U);
}
