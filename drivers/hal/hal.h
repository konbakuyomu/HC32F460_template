/**
 * @brief 硬件抽象层
 * @author
 * @date 2024-10-28
 */

#pragma once

#ifdef __cplusplus
    #include "hc32_ll.h"
    #include <array>
    #include <memory>
    #include <string>
    #include <utility>
    #include <vector>

class HAL
{
  private:
    static HAL *hal;
  public:
    static bool inject(HAL *_hal);
    static HAL *get(void);
    virtual ~HAL() = default;

  public:
    virtual void init() {}

    /* 系统配置 */
    static void system_init() { get()->_system_init(); }
    virtual void _system_init() {}

    /* 键值对配置 */
    static void global_key_value_init() { get()->_global_key_value_init(); }
    virtual void _global_key_value_init() {}

    /* LED 配置 */
    static void led_init() { get()->_led_init(); }
    virtual void _led_init() {}
    static void led_turn_on(const std::string &name) { get()->_led_turn_on(name); }
    virtual void _led_turn_on(const std::string &name) {}
    static void led_turn_off(const std::string &name) { get()->_led_turn_off(name); }
    virtual void _led_turn_off(const std::string &name) {}
    static void led_toggle(const std::string &name) { get()->_led_toggle(name); }
    virtual void _led_toggle(const std::string &name) {}

    /* PWM 配置 */
    static void pwm_init() { get()->_pwm_init(); }
    virtual void _pwm_init() {}
    static void pwm_start(const std::string &name) { get()->_pwm_start(name); }
    virtual void _pwm_start(const std::string &name) {}
    static void pwm_stop(const std::string &name) { get()->_pwm_stop(name); }
    virtual void _pwm_stop(const std::string &name) {}
    static void pwm_set_duty_cycle(const std::string &name, float duty_cycle) { get()->_pwm_set_duty_cycle(name, duty_cycle); }
    virtual void _pwm_set_duty_cycle(const std::string &name, float duty_cycle) {}
    static void pwm_set_frequency(const std::string &name, uint32_t frequency) { get()->_pwm_set_frequency(name, frequency); }
    virtual void _pwm_set_frequency(const std::string &name, uint32_t frequency) {}
    static void pwm_set_direction(const std::string &name, uint8_t direction) { get()->_pwm_set_direction(name, direction); }
    virtual void _pwm_set_direction(const std::string &name, uint8_t direction) {}
    static uint8_t pwm_get_direction(const std::string &name) { return get()->_pwm_get_direction(name); }
    virtual uint8_t _pwm_get_direction(const std::string &name) { return 0; }

    /* CAN 配置 */
    static void can_init() { get()->_can_init(); }
    virtual void _can_init() {}
    static void can_send(void *value) { get()->_can_send(value); }
    virtual void _can_send(void *value) {}

    /* UART 配置 */
    static void uart_init() { get()->_uart_init(); }
    virtual void _uart_init() {}
    static void uart_send(void *data, size_t length) { get()->_uart_send(data, length); }
    virtual void _uart_send(void *data, size_t length) {}
};
#endif
