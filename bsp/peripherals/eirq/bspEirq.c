/**
 * @file bsp_eirq.cpp
 * @brief 外部中断底层驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-11-05
 */

#include "globalConfig.h"

/**
 * @brief 进门感应中断回调函数
 * @details 当进门感应触发中断时,设置事件组标志位
 */
static void RX_JM_IrqCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult;

    if (SET == EXTINT_GetExtIntStatus(EXTINT_CH06)) {
        // 使用枚举类型设置事件位
        xResult = xEventGroupSetBitsFromISR(motorControlEventGroupHandle,
                                            (EventBits_t)(EVENT_DOOR_ENTRY_MOTOR),
                                            &xHigherPriorityTaskWoken);

        if (xResult != pdFAIL) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        EXTINT_ClearExtIntStatus(EXTINT_CH07);
    }
}

/**
 * @brief 出门感应中断回调函数
 * @details 当出门感应触发中断时,设置事件组标志位
 */
static void RX_CM_IrqCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult;

    if (SET == EXTINT_GetExtIntStatus(EXTINT_CH04)) {
        xResult =
            xEventGroupSetBitsFromISR(motorControlEventGroupHandle, (EventBits_t)(EVENT_DOOR_EXIT_MOTOR),
                                      &xHigherPriorityTaskWoken);

        if (xResult != pdFAIL) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        EXTINT_ClearExtIntStatus(EXTINT_CH08);
    }
}

/**
 * @brief 身高感应中断回调函数
 * @details 当身高感应触发中断时,设置事件组标志位
 */
static void RX_SG_IrqCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult;

    if (SET == EXTINT_GetExtIntStatus(EXTINT_CH05)) {
        xResult = xEventGroupSetBitsFromISR(
            motorControlEventGroupHandle, (EventBits_t)(EVENT_HEIGHT_SENSOR), &xHigherPriorityTaskWoken);

        if (xResult != pdFAIL) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        EXTINT_ClearExtIntStatus(EXTINT_CH05);
    }
}

/**
 * @brief 占位感应中断回调函数
 * @details 当占位感应触发中断时,设置事件组标志位
 */
static void RX_ZW_IrqCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult;

    if (SET == EXTINT_GetExtIntStatus(EXTINT_CH03)) {
        xResult = xEventGroupSetBitsFromISR(
            motorControlEventGroupHandle, (EventBits_t)(EVENT_OCCUPANCY_SENSOR), &xHigherPriorityTaskWoken);

        if (xResult != pdFAIL) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        EXTINT_ClearExtIntStatus(EXTINT_CH03);
    }
}

/**
 * @brief 外部中断初始化函数
 * @details 初始化GPIO、配置外部中断、注册中断回调函数
 */
void initBspEirq(void)
{
    stc_extint_init_t stcExtIntInit;
    stc_irq_signin_config_t stcIrqSignConfig;
    stc_gpio_init_t stcGpioInit;

    /* GPIO 初始化 */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16ExtInt = PIN_EXTINT_ON;
    (void)GPIO_Init(RX_JM_PORT, RX_JM_PIN, &stcGpioInit);
    (void)GPIO_Init(RX_SG_PORT, RX_SG_PIN, &stcGpioInit);
    (void)GPIO_Init(RX_CM_PORT, RX_CM_PIN, &stcGpioInit);
    (void)GPIO_Init(RX_ZW_PORT, RX_ZW_PIN, &stcGpioInit);

    /* ExtInt 外部中断初始化 */
    (void)EXTINT_StructInit(&stcExtIntInit);
    stcExtIntInit.u32Edge = EXTINT_TRIG_BOTH; // 这里很重要，因为要通过高低电平判断状态
    (void)EXTINT_Init(RX_JM_EXTINT, &stcExtIntInit);
    (void)EXTINT_Init(RX_SG_EXTINT, &stcExtIntInit);
    (void)EXTINT_Init(RX_CM_EXTINT, &stcExtIntInit);
    (void)EXTINT_Init(RX_ZW_EXTINT, &stcExtIntInit);

    /* 进门感应 中断注册 */
    stcIrqSignConfig.enIntSrc = RX_JM_INT_SRC;
    stcIrqSignConfig.enIRQn = RX_JM_IRQn;
    stcIrqSignConfig.pfnCallback = &RX_JM_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    installInterruptHandler(&stcIrqSignConfig, DDL_IRQ_PRIO_DEFAULT);

    /* 出门感应 中断注册 */
    stcIrqSignConfig.enIntSrc = RX_CM_INT_SRC;
    stcIrqSignConfig.enIRQn = RX_CM_IRQn;
    stcIrqSignConfig.pfnCallback = &RX_CM_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    installInterruptHandler(&stcIrqSignConfig, DDL_IRQ_PRIO_DEFAULT);

    /* 身高感应 中断注册 */
    stcIrqSignConfig.enIntSrc = RX_SG_INT_SRC;
    stcIrqSignConfig.enIRQn = RX_SG_IRQn;
    stcIrqSignConfig.pfnCallback = &RX_SG_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    installInterruptHandler(&stcIrqSignConfig, DDL_IRQ_PRIO_DEFAULT);

    /* 占位感应 中断注册 */
    stcIrqSignConfig.enIntSrc = RX_ZW_INT_SRC;
    stcIrqSignConfig.enIRQn = RX_ZW_IRQn;
    stcIrqSignConfig.pfnCallback = &RX_ZW_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    installInterruptHandler(&stcIrqSignConfig, DDL_IRQ_PRIO_DEFAULT);
}
