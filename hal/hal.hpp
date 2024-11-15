/**
 * @brief 硬件抽象层
 * @author
 * @date 2024-10-28
 */

#pragma once
#ifdef __cplusplus

#include "hc32_ll.h"
#include <array>
#include <bitset>
#include <cstdint>
#include <cstring>
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
    static void delayMillis(unsigned long _mill) { get()->delayMillisImpl(_mill); }
    virtual void delayMillisImpl(unsigned long _mill) {}
    static unsigned long getTick() { return get()->getTickImpl(); }
    virtual unsigned long getTickImpl() { return 0; }

    /* 键值对配置 */
    static void initGlobalKeyValue() { get()->initGlobalKeyValueImpl(); }
    virtual void initGlobalKeyValueImpl() {}

    /* LED 配置 */
    static void initLed() { get()->initLedImpl(); }
    virtual void initLedImpl() {}
    static void turnOnLed(const std::string &name) { get()->turnOnLedImpl(name); }
    virtual void turnOnLedImpl(const std::string &name) {}
    static void turnOffLed(const std::string &name) { get()->turnOffLedImpl(name); }
    virtual void turnOffLedImpl(const std::string &name) {}
    static void toggleLed(const std::string &name) { get()->toggleLedImpl(name); }
    virtual void toggleLedImpl(const std::string &name) {}

    /* PWM 配置 */
    static void initPwm() { get()->initPwmImpl(); }
    virtual void initPwmImpl() {}
    static void startPwm(const std::string &name) { get()->startPwmImpl(name); }
    virtual void startPwmImpl(const std::string &name) {}
    static void stopPwm(const std::string &name) { get()->stopPwmImpl(name); }
    virtual void stopPwmImpl(const std::string &name) {}
    static void setPwmDutyCycle(const std::string &name, float dutyCycle)
    {
        get()->setPwmDutyCycleImpl(name, dutyCycle);
    }
    virtual void setPwmDutyCycleImpl(const std::string &name, float dutyCycle) {}
    static void setPwmFrequency(const std::string &name, uint32_t frequency)
    {
        get()->setPwmFrequencyImpl(name, frequency);
    }
    virtual void setPwmFrequencyImpl(const std::string &name, uint32_t frequency) {}
    static void setPwmDirection(const std::string &name, uint8_t direction)
    {
        get()->setPwmDirectionImpl(name, direction);
    }
    virtual void setPwmDirectionImpl(const std::string &name, uint8_t direction) {}
    static uint8_t getPwmDirection(const std::string &name)
    {
        return get()->getPwmDirectionImpl(name);
    }
    virtual uint8_t getPwmDirectionImpl(const std::string &name) { return 0; }

    /* CAN 配置 */
    static void initCan() { get()->initCanImpl(); }
    virtual void initCanImpl() {}
    static void sendCan(void *value) { get()->sendCanImpl(value); }
    virtual void sendCanImpl(void *value) {}

    /* UART 配置 */
    static void initUart() { get()->initUartImpl(); }
    virtual void initUartImpl() {}
    static void sendUart(void *data, size_t length) { get()->sendUartImpl(data, length); }
    virtual void sendUartImpl(void *data, size_t length) {}

    /* EIRQ 配置 */
    static void initEirq() { get()->initEirqImpl(); }
    virtual void initEirqImpl() {}
};

#endif
