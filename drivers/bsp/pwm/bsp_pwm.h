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
    #include <map>
    #include <memory>
    #include <string>
    #include <vector>


/* 类定义 ------------------------------------------------------------------*/
namespace BSP_PWM {

    /**
     * @brief PWM类
     * @note 该类用于控制PWM的硬件参数
     */
    class bsp_pwm
    {
      private:
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

        std::map<std::string, PWMConfig> m_pwm_map; // 存储PWM配置的容器
        static std::unique_ptr<bsp_pwm> m_pwm;

      public:
        bsp_pwm() = default;
        ~bsp_pwm() = default;

      public:
        void add_pwm(const std::string &name, const PWMConfig &config);

      public:
        static void init();
        static void pwm_start(const std::string &name);
        static void pwm_stop(const std::string &name);
        static void pwm_set_duty_cycle(const std::string &name, float duty_cycle);
        static void pwm_set_frequency(const std::string &name, uint32_t frequency);
        static uint8_t pwm_get_direction(const std::string &name);
        static void pwm_set_direction(const std::string &name, uint8_t direction);
    };

} // namespace BSP_PWM
#endif
