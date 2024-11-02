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
    inline void init() override;
    void _system_init() override;
    void _global_key_value_init() override;

  public:
    void _pwm_init() override;
    void _pwm_start(size_t index) override;
    void _pwm_stop(size_t index) override;
    void _pwm_set_duty_cycle(size_t index, float duty_cycle) override;
    void _pwm_set_frequency(size_t index, uint32_t frequency) override;
    void _pwm_set_direction(size_t index, uint8_t direction) override;
    uint8_t _pwm_get_direction(size_t index) override;

  public:
    void _can_init() override;
    void _can_send(void *value) override;

  public:
    void _uart_init() override;
    void _uart_send(void *data, size_t length) override;

  private:
    std::unique_ptr<BSP_PWM::bsp_pwm> m_pwm;
};

#endif
