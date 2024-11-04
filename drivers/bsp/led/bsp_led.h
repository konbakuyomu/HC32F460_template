/**
 * @file bsp_led.h
 * @brief 硬件LED初始化
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-11-04
 */

#pragma once

#ifdef __cplusplus
    #include "hal.h"
    #include "hc32_ll.h"
    #include <map>
    #include <string>
    #include <vector>

namespace BSP_LED {

    class LED
    {
      private:
        struct LedConfig {
            uint8_t port;      // LED端口
            uint16_t pin;      // LED引脚
            bool isActiveLow;  // 是否为低电平有效
        };

        std::map<std::string, LedConfig> ledMap; // 存储LED配置的容器
        static std::unique_ptr<LED> m_led;

      public:
        LED() = default;
        ~LED() = default;

      public:
        void addLed(const std::string &name, const LedConfig &config);

      public:
        static void init();
        static void turnOn(const std::string &name);
        static void turnOff(const std::string &name);
        static void toggle(const std::string &name);
    };

} // namespace BSP_LED

#endif
