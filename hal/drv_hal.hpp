/**
 * @file drv_hal.h
 * @brief 硬件驱动层实现
 * @version 1.0
 * @date 2024-10-28
 */

#pragma once
#ifdef __cplusplus

#include "hal.hpp"

class DRV_HAL : public HAL
{
  public:
    void delayMillisImpl(unsigned long _mill) override;
    unsigned long getTickImpl() override;

  public:
    void initLedImpl() override;
    void turnOnLedImpl(const std::string &name) override;
    void turnOffLedImpl(const std::string &name) override;
    void toggleLedImpl(const std::string &name) override;

  public:
    void initPwmImpl() override;
    void startPwmImpl(const std::string &name) override;
    void stopPwmImpl(const std::string &name) override;
    void setPwmDutyCycleImpl(const std::string &name, float dutyCycle) override;
    void setPwmFrequencyImpl(const std::string &name, uint32_t frequency) override;
    void setPwmDirectionImpl(const std::string &name, uint8_t direction) override;
    uint8_t getPwmDirectionImpl(const std::string &name) override;

  public:
    void initCanImpl() override;
    void sendCanImpl(void *value) override;

  public:
    void initUartImpl() override;
    void sendUartImpl(void *data, size_t length) override;

  public:
    void initEirqImpl() override;
};

#endif
