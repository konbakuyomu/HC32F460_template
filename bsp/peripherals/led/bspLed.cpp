/**
 * @file bsp_led.cpp
 * @brief LED底层驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-11-04
 */

#include "globalConfig.h"

namespace BSP_LED
{

std::unique_ptr<LED> LED::mLed;

/**
 * @brief 添加LED配置
 * @param name 名称
 * @param config 配置
 */
void LED::addLed(const std::string &name, const LedConfig &config) { ledMap[name] = config; }

/**
 * @brief 初始化LED
 */
void LED::init()
{
    mLed = std::make_unique<LED>();

    LedConfig led_config_1 = {
        .port = GPIO_PORT_D,
        .pin = GPIO_PIN_12,
        .isActiveLow = true,
    };
    mLed->addLed("Normal", led_config_1);

    LedConfig led_config_2 = {
        .port = GPIO_PORT_D,
        .pin = GPIO_PIN_11,
        .isActiveLow = true,
    };
    mLed->addLed("Measure", led_config_2);

    LedConfig led_config_3 = {
        .port = GPIO_PORT_D,
        .pin = GPIO_PIN_10,
        .isActiveLow = true,
    };
    mLed->addLed("Pollute", led_config_3);

    LedConfig led_config_4 = {
        .port = GPIO_PORT_D,
        .pin = GPIO_PIN_09,
        .isActiveLow = true,
    };
    mLed->addLed("Fault", led_config_4);

    // GPIO初始化结构体
    stc_gpio_init_t stcGpioInit;

    for (const auto &led : mLed->ledMap) {
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
    auto it = mLed->ledMap.find(name);

    if (it != mLed->ledMap.end()) {
        const auto &led = it->second;
        led.isActiveLow ? GPIO_ResetPins(led.port, led.pin) : GPIO_SetPins(led.port, led.pin);
    }
}

/**
 * @brief 关闭LED
 * @param name 名称
 */
void LED::turnOff(const std::string &name)
{
    auto it = mLed->ledMap.find(name);

    if (it != mLed->ledMap.end()) {
        const auto &led = it->second;
        led.isActiveLow ? GPIO_SetPins(led.port, led.pin) : GPIO_ResetPins(led.port, led.pin);
    }
}

/**
 * @brief 切换LED状态
 * @param name 名称
 */
void LED::toggle(const std::string &name)
{
    auto it = mLed->ledMap.find(name);

    if (it != mLed->ledMap.end()) {
        const auto &led = it->second;
        GPIO_TogglePins(led.port, led.pin);
    }
}

} // namespace BSP_LED
