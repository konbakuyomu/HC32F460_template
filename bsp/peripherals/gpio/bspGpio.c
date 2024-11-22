/**
 * @file bsp_gpio.c
 * @brief 通用输入输出GPIO底层驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-11-05
 */

#include "globalConfig.h"

/**
 * @brief 初始化GPIO
 */
void initBspGpio(void)
{
    // GPIO初始化结构体
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinDrv = PIN_HIGH_DRV;
    (void)GPIO_Init(ExitMotorControlPort, ExitMotorControlPin, &stcGpioInit);
}
