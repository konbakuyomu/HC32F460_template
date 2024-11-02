/**
 * @file bsp_pwm.h
 * @brief 电机底层PWM初始化
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-10-11
 */

#pragma once

#ifdef __cplusplus
    #include "hal.h"
    #include "hc32_ll.h"
    #include <memory>
    #include <vector>

/* 类定义 ------------------------------------------------------------------*/
namespace BSP_PWM {

    /**
     * @brief PWM类
     * @note 该类用于控制PWM的硬件参数
     */
    class bsp_pwm
    {
      public:
        /**
         * @brief PWM配置结构体
         * @note 该结构体用于配置PWM的硬件参数
         */
        struct PWMConfig {
            CM_TMRA_TypeDef *tmra_unit; // TMRA定时器单元指针
            uint32_t tmra_clk;          // TMRA定时器时钟源
            uint8_t tmra_channel;       // TMRA定时器通道
            uint8_t tmra_port;          // TMRA定时器端口
            uint16_t tmra_pin;          // TMRA定时器引脚
            uint8_t tmra_func;          // TMRA定时器功能选择
            uint8_t tmra_dir_port;      // TMRA方向控制端口
            uint16_t tmra_dir_pin;      // TMRA方向控制引脚
            uint8_t tmra_dir_direction; // TMRA方向控制引脚方向
            int frequency;              // PWM频率（Hz）
            float duty_cycle;           // PWM占空比（0.0 - 1.0）
        };

      public:
        bsp_pwm(const PWMConfig &config);
        bsp_pwm(void) = default;
        ~bsp_pwm();
        static void _bsp_pwm_init();
        void _bsp_pwm_start();
        void _bsp_pwm_stop();
        void _bsp_pwm_set_duty_cycle(float duty_cycle);
        void _bsp_pwm_set_frequency(uint32_t frequency);
        uint8_t _bsp_pwm_get_direction();
        void _bsp_pwm_set_direction(uint8_t direction);
        static void _bsp_add_pwm(const PWMConfig &config);
        bsp_pwm *_bsp_get_pwm(size_t index);

      public:
        PWMConfig m_config;
        bool m_is_running;
        static std::vector<std::unique_ptr<bsp_pwm>> m_pwms; // 存储PWM对象的容器
    };

} // namespace BSP_PWM
#endif
