/**
 * @file drv_hal.h
 * @brief 硬件驱动层实现
 * @version 1.0
 * @date 2024-10-28
 */

#pragma once

#ifdef __cplusplus

    #include "hal.h"
    #include "bsp_pwm.h"

/* 类定义 ------------------------------------------------------------------*/
class DRV_HAL : public HAL
{
  public:
    void _system_init() override;

    void _led_init() override;
    void _led_turn_on(const std::string &name) override;
    void _led_turn_off(const std::string &name) override;
    void _led_toggle(const std::string &name) override;

  public:
    void _pwm_init() override;
    void _pwm_start(const std::string &name) override;
    void _pwm_stop(const std::string &name) override;
    void _pwm_set_duty_cycle(const std::string &name, float duty_cycle) override;
    void _pwm_set_frequency(const std::string &name, uint32_t frequency) override;
    void _pwm_set_direction(const std::string &name, uint8_t direction) override;
    uint8_t _pwm_get_direction(const std::string &name) override;

  public:
    void _can_init() override;
    void _can_send(void *value) override;

  public:
    void _uart_init() override;
    void _uart_send(void *data, size_t length) override;
};

#endif
