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
static void entranceSensorIrqCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult;

    if (SET == EXTINT_GetExtIntStatus(EXTINT_CH06)) {
        // 使用枚举类型设置事件位
        xResult = xEventGroupSetBitsFromISR(motorControlEventGroupHandle,
                                            (EventBits_t)(EVENT_DOOR_ENTRY_SENSOR),
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
static void exitSensorIrqCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult;

    if (SET == EXTINT_GetExtIntStatus(EXTINT_CH04)) {
        xResult = xEventGroupSetBitsFromISR(motorControlEventGroupHandle,
                                            (EventBits_t)(EVENT_DOOR_EXIT_SENSOR),
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
static void heightSensorIrqCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult;

    if (SET == EXTINT_GetExtIntStatus(EXTINT_CH05)) {
        xResult = xEventGroupSetBitsFromISR(motorControlEventGroupHandle,
                                            (EventBits_t)(EVENT_HEIGHT_SENSOR),
                                            &xHigherPriorityTaskWoken);

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
static void occupancySensorIrqCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult;

    if (SET == EXTINT_GetExtIntStatus(EXTINT_CH03)) {
        xResult = xEventGroupSetBitsFromISR(motorControlEventGroupHandle,
                                            (EventBits_t)(EVENT_OCCUPANCY_SENSOR),
                                            &xHigherPriorityTaskWoken);

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
    (void)GPIO_Init(EntranceSensorPort, EntranceSensorPin, &stcGpioInit);
    (void)GPIO_Init(HeightSensorPort, HeightSensorPin, &stcGpioInit);
    (void)GPIO_Init(ExitSensorPort, ExitSensorPin, &stcGpioInit);
    (void)GPIO_Init(OccupancySensorPort, OccupancySensorPin, &stcGpioInit);

    /* ExtInt 外部中断初始化 */
    (void)EXTINT_StructInit(&stcExtIntInit);
    stcExtIntInit.u32Edge = EXTINT_TRIG_BOTH; // 这里很重要，因为要通过高低电平判断状态
    (void)EXTINT_Init(EntranceSensorExtInt, &stcExtIntInit);
    (void)EXTINT_Init(HeightSensorExtInt, &stcExtIntInit);
    (void)EXTINT_Init(ExitSensorExtInt, &stcExtIntInit);
    (void)EXTINT_Init(OccupancySensorExtInt, &stcExtIntInit);

    /* 进门感应 中断注册 */
    stcIrqSignConfig.enIntSrc = EntranceSensorIntSrc;
    stcIrqSignConfig.enIRQn = EntranceSensorIRQn;
    stcIrqSignConfig.pfnCallback = &entranceSensorIrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    installInterruptHandler(&stcIrqSignConfig, DDL_IRQ_PRIO_DEFAULT);

    /* 出门感应 中断注册 */
    stcIrqSignConfig.enIntSrc = ExitSensorIntSrc;
    stcIrqSignConfig.enIRQn = ExitSensorIRQn;
    stcIrqSignConfig.pfnCallback = &exitSensorIrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    installInterruptHandler(&stcIrqSignConfig, DDL_IRQ_PRIO_DEFAULT);

    /* 身高感应 中断注册 */
    stcIrqSignConfig.enIntSrc = HeightSensorIntSrc;
    stcIrqSignConfig.enIRQn = HeightSensorIRQn;
    stcIrqSignConfig.pfnCallback = &heightSensorIrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    installInterruptHandler(&stcIrqSignConfig, DDL_IRQ_PRIO_DEFAULT);

    /* 占位感应 中断注册 */
    stcIrqSignConfig.enIntSrc = OccupancySensorIntSrc;
    stcIrqSignConfig.enIRQn = OccupancySensorIRQn;
    stcIrqSignConfig.pfnCallback = &occupancySensorIrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    installInterruptHandler(&stcIrqSignConfig, DDL_IRQ_PRIO_DEFAULT);
}
