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
    EventGroupHandle_t motorEventGroupHandle = Motor_GetEventGroupHandle();

    if (SET == EXTINT_GetExtIntStatus(EntranceSensorExtInt)) {
        // 使用枚举类型设置事件位
        xResult = xEventGroupSetBitsFromISR(motorEventGroupHandle,
                                            (EventBits_t)(EVENT_DOOR_ENTRY_SENSOR),
                                            &xHigherPriorityTaskWoken);

        if (xResult != pdFAIL) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        EXTINT_ClearExtIntStatus(EntranceSensorExtInt);
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
    EventGroupHandle_t motorEventGroupHandle = Motor_GetEventGroupHandle();

    if (SET == EXTINT_GetExtIntStatus(ExitSensorExtInt)) {
        xResult = xEventGroupSetBitsFromISR(motorEventGroupHandle,
                                            (EventBits_t)(EVENT_DOOR_EXIT_SENSOR),
                                            &xHigherPriorityTaskWoken);

        if (xResult != pdFAIL) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        EXTINT_ClearExtIntStatus(ExitSensorExtInt);
    }
}

/**
 * @brief 进门电机开启到位中断回调函数
 * @details 当进门电机开启到位时,设置事件组标志位
 */
static void entranceMotorOpenToPositionIrqCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult;
    EventGroupHandle_t motorEventGroupHandle = Motor_GetEventGroupHandle();

    if (SET == EXTINT_GetExtIntStatus(EntranceMotorOpenToPositionExtInt)) {
        xResult = xEventGroupSetBitsFromISR(motorEventGroupHandle,
                                            (EventBits_t)(EVENT_ENTRY_MOTOR_OPEN_TO_POSITION),
                                            &xHigherPriorityTaskWoken);

        if (xResult != pdFAIL) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        EXTINT_ClearExtIntStatus(EntranceMotorOpenToPositionExtInt);
    }
}

/**
 * @brief 进门电机关闭到位中断回调函数
 * @details 当进门电机关闭到位时,设置事件组标志位
 */
static void entranceMotorCloseToPositionIrqCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult;
    EventGroupHandle_t motorEventGroupHandle = Motor_GetEventGroupHandle();

    if (SET == EXTINT_GetExtIntStatus(EntranceMotorCloseToPositionExtInt)) {
        xResult = xEventGroupSetBitsFromISR(motorEventGroupHandle,
                                            (EventBits_t)(EVENT_ENTRY_MOTOR_CLOSE_TO_POSITION),
                                            &xHigherPriorityTaskWoken);

        if (xResult != pdFAIL) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        EXTINT_ClearExtIntStatus(EntranceMotorCloseToPositionExtInt);
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
    (void)GPIO_Init(ExitSensorPort, ExitSensorPin, &stcGpioInit);
    (void)GPIO_Init(EntranceMotorOpenToPositionPort, EntranceMotorOpenToPositionPin, &stcGpioInit);
    (void)GPIO_Init(EntranceMotorCloseToPositionPort, EntranceMotorCloseToPositionPin,
                    &stcGpioInit);

    /* ExtInt 外部中断初始化 */
    (void)EXTINT_StructInit(&stcExtIntInit);
    stcExtIntInit.u32Edge = EXTINT_TRIG_BOTH;
    stcExtIntInit.u32Filter = EXTINT_FILTER_ON;      // 开启硬件滤波
    stcExtIntInit.u32FilterClock = EXTINT_FCLK_DIV8; // 设置滤波时钟分频
    (void)EXTINT_Init(EntranceSensorExtInt, &stcExtIntInit);
    (void)EXTINT_Init(ExitSensorExtInt, &stcExtIntInit);
    (void)EXTINT_Init(EntranceMotorOpenToPositionExtInt, &stcExtIntInit);
    (void)EXTINT_Init(EntranceMotorCloseToPositionExtInt, &stcExtIntInit);

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

    /* 进门电机开启到位 中断注册 */
    stcIrqSignConfig.enIntSrc = EntranceMotorOpenToPositionIntSrc;
    stcIrqSignConfig.enIRQn = EntranceMotorOpenToPositionIRQn;
    stcIrqSignConfig.pfnCallback = &entranceMotorOpenToPositionIrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    installInterruptHandler(&stcIrqSignConfig, DDL_IRQ_PRIO_DEFAULT);

    /* 进门电机关闭到位 中断注册 */
    stcIrqSignConfig.enIntSrc = EntranceMotorCloseToPositionIntSrc;
    stcIrqSignConfig.enIRQn = EntranceMotorCloseToPositionIRQn;
    stcIrqSignConfig.pfnCallback = &entranceMotorCloseToPositionIrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    installInterruptHandler(&stcIrqSignConfig, DDL_IRQ_PRIO_DEFAULT);
}
