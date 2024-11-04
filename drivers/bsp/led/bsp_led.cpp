/**
 * @file bsp_led.cpp
 * @brief 硬件LED初始化
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-11-04
 */

#include "variable.h"

namespace BSP_LED {

    std::unique_ptr<LED> LED::m_led;

    /**
     * @brief 添加LED配置
     * @param name 名称
     * @param config 配置
     */
    void LED::addLed(const std::string &name, const LedConfig &config)
    {
        ledMap[name] = config;
    }

    /**
     * @brief 初始化LED
     */
    void LED::init()
    {
        m_led = std::make_unique<LED>();

        LedConfig led_config_1 = {
            .port = BSP_LED_RED_PORT,
            .pin = BSP_LED_RED_PIN,
            .isActiveLow = false,
        };
        m_led->addLed("LED_RED", led_config_1);

        LedConfig led_config_2 = {
            .port = BSP_LED_GREEN_PORT,
            .pin = BSP_LED_GREEN_PIN,
            .isActiveLow = false,
        };
        m_led->addLed("LED_GREEN", led_config_2);

        LedConfig led_config_3 = {
            .port = BSP_LED_YELLOW_PORT,
            .pin = BSP_LED_YELLOW_PIN,
            .isActiveLow = false,
        };
        m_led->addLed("LED_YELLOW", led_config_3);

        LedConfig led_config_4 = {
            .port = BSP_LED_BLUE_PORT,
            .pin = BSP_LED_BLUE_PIN,
            .isActiveLow = false,
        };
        m_led->addLed("LED_BLUE", led_config_4);

        // GPIO初始化结构体
        stc_gpio_init_t stcGpioInit;

        for (const auto &led : m_led->ledMap)
        {
            // 引脚GPIO注册，默认关闭le
            (void)GPIO_StructInit(&stcGpioInit);
            stcGpioInit.u16PinDir = PIN_DIR_OUT;
            stcGpioInit.u16PinState = led.second.isActiveLow ? PIN_STAT_SET : PIN_STAT_RST;
            (void)GPIO_Init(led.second.port, led.second.pin, &stcGpioInit);
        }
    }

    /**
     * @brief 打开LED
     * @param name 名称
     */
    void LED::turnOn(const std::string &name)
    {
        if (m_led->ledMap.find(name) != m_led->ledMap.end())
        {
            const auto &led = m_led->ledMap[name];
            led.isActiveLow ? GPIO_ResetPins(led.port, led.pin) : GPIO_SetPins(led.port, led.pin);
        }
    }

    /**
     * @brief 关闭LED
     * @param name 名称
     */
    void LED::turnOff(const std::string &name)
    {
        if (m_led->ledMap.find(name) != m_led->ledMap.end())
        {
            const auto &led = m_led->ledMap[name];
            led.isActiveLow ? GPIO_SetPins(led.port, led.pin) : GPIO_ResetPins(led.port, led.pin);
        }
    }

    /**
     * @brief 切换LED状态
     * @param name 名称
     */
    void LED::toggle(const std::string &name)
    {
        if (m_led->ledMap.find(name) != m_led->ledMap.end())
        {
            const auto &led = m_led->ledMap[name];
            GPIO_TogglePins(led.port, led.pin);
        }
    }

} // namespace BSP_LED
