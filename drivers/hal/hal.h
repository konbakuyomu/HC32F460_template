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

    /* PWM 配置 */
    static void pwm_init() { get()->_pwm_init(); }
    virtual void _pwm_init() {}
    static void pwm_start(size_t index) { get()->_pwm_start(index); }
    virtual void _pwm_start(size_t index) {}
    static void pwm_stop(size_t index) { get()->_pwm_stop(index); }
    virtual void _pwm_stop(size_t index) {}
    static void pwm_set_duty_cycle(size_t index, float duty_cycle) { get()->_pwm_set_duty_cycle(index, duty_cycle); }
    virtual void _pwm_set_duty_cycle(size_t index, float duty_cycle) {}
    static void pwm_set_frequency(size_t index, uint32_t frequency) { get()->_pwm_set_frequency(index, frequency); }
    virtual void _pwm_set_frequency(size_t index, uint32_t frequency) {}
    static void pwm_set_direction(size_t index, uint8_t direction) { get()->_pwm_set_direction(index, direction); }
    virtual void _pwm_set_direction(size_t index, uint8_t direction) {}
    static uint8_t pwm_get_direction(size_t index) { return get()->_pwm_get_direction(index); }
    virtual uint8_t _pwm_get_direction(size_t index) { return 0; }

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

/* 函数定义 ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief 启动PWM
     * @param index PWM索引
     */
    void PWM_start(void *value, size_t index);

    /**
     * @brief 停止PWM
     * @param index PWM索引
     */
    void PWM_stop(void *value, size_t index);

    /**
     * @brief 设置PWM占空比
     * @param index PWM索引
     * @param duty_cycle 占空比
     */
    void PWM_setDutyCycle(void *value, size_t index);

    /**
     * @brief 设置PWM频率
     * @param index PWM索引
     * @param frequency 频率
     */
    void PWM_setFrequency(void *value, size_t index);

    /**
     * @brief 设置PWM方向
     * @param index PWM索引
     * @param direction 方向
     */
    void PWM_setDirection(void *value, size_t index);

    /**
     * @brief 获取PWM方向
     * @param index PWM索引
     * @return 方向
     */
    void PWM_getDirection(void *value, size_t index);

#ifdef __cplusplus
}
#endif
