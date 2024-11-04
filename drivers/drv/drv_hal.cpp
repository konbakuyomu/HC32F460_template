/**
 * @file drv_hal.cpp
 * @brief 硬件驱动层实现
 * @version 1.0
 * @date 2024-10-28
 */

#include "variable.h"

void DRV_HAL::_system_init()
{
    /* CPU使用率统计初始化 */
    CPU_TMR0_Config();
    /* 开启CPU使用率统计 */
    Start_CPU_Utilization();
    /* 串口printf初始化 -> PB9:USART4_RX  PE6:USART4_TX */
    LL_PrintfInit(USART_UNIT, USART_BAUDRATE, USART_Config);
}

void DRV_HAL::_led_init()
{
    BSP_LED::LED::init();
}

void DRV_HAL::_led_turn_on(const std::string &name)
{
    BSP_LED::LED::turnOn(name);
}

void DRV_HAL::_led_turn_off(const std::string &name)
{
    BSP_LED::LED::turnOff(name);
}

void DRV_HAL::_led_toggle(const std::string &name)
{
    BSP_LED::LED::toggle(name);
}

void DRV_HAL::_pwm_init()
{
    BSP_PWM::bsp_pwm::init();

    HAL::pwm_start("PWM1");
    HAL::pwm_start("PWM2");
    HAL::pwm_start("PWM3");
    HAL::pwm_start("PWM4");
}

void DRV_HAL::_pwm_start(const std::string &name)
{
    BSP_PWM::bsp_pwm::pwm_start(name);
}

void DRV_HAL::_pwm_stop(const std::string &name)
{
    BSP_PWM::bsp_pwm::pwm_stop(name);
}

void DRV_HAL::_pwm_set_duty_cycle(const std::string &name, float duty_cycle)
{
    BSP_PWM::bsp_pwm::pwm_set_duty_cycle(name, duty_cycle);
}

void DRV_HAL::_pwm_set_frequency(const std::string &name, uint32_t frequency)
{
    BSP_PWM::bsp_pwm::pwm_set_frequency(name, frequency);
}

void DRV_HAL::_pwm_set_direction(const std::string &name, uint8_t direction)
{
    BSP_PWM::bsp_pwm::pwm_set_direction(name, direction);
}

uint8_t DRV_HAL::_pwm_get_direction(const std::string &name)
{
    return BSP_PWM::bsp_pwm::pwm_get_direction(name);
}

void DRV_HAL::_can_init()
{
    BSP_CAN::CAN::_bsp_can_init();
}

void DRV_HAL::_can_send(void *value)
{
    BSP_CAN::CAN::SendData(value);
}

void DRV_HAL::_uart_init()
{
    BSP_USART::USART4::USART4_Init();
}

void DRV_HAL::_uart_send(void *data, size_t length)
{
    BSP_USART::USART4::USART4_SendData(data, length);
}
