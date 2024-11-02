/**
 * @file hal.cpp
 * @brief 硬件抽象层实现
 * @version 1.0
 * @date 2024-10-28
 */

#include "variable.h"

HAL *HAL::hal = nullptr;

bool HAL::inject(HAL *_hal)
{
    if (_hal == nullptr)
    {
        return false;
    }

    _hal->init();
    hal = _hal;
    return true;
}

HAL *HAL::get(void)
{
    return hal;
}

extern "C"
{
    void PWM_start(void *value, size_t index)
    {
        HAL::pwm_start(index);
    }

    void PWM_stop(void *value, size_t index)
    {
        HAL::pwm_stop(index);
    }

    void PWM_setDutyCycle(void *value, size_t index)
    {
        float duty_cycle = *(float *)value;
        HAL::pwm_set_duty_cycle(index, duty_cycle);
    }

    void PWM_setFrequency(void *value, size_t index)
    {
        uint32_t frequency = *(uint32_t *)value;
        HAL::pwm_set_frequency(index, frequency);
    }

    void PWM_setDirection(void *value, size_t index)
    {
        uint8_t direction = *(uint8_t *)value;
        HAL::pwm_set_direction(index, direction);
    }

    void PWM_getDirection(void *value, size_t index)
    {
        int8_t direction = HAL::pwm_get_direction(index);
        *(int8_t *)value = direction;
    }

    void USART_SendData(void *data, size_t length)
    {
        HAL::uart_send(data, length);
    }
}

