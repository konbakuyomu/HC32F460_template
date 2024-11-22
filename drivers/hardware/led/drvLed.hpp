/**
 * @file drvLed.hpp
 * @brief LED驱动层C++实现
 * @version 1.0
 * @date 2024-11-21
 */
#pragma once
#ifdef __cplusplus

#include "FreeRTOS.h"
#include "event_groups.h"
#include "hal.hpp"
#include "timers.h"

namespace DRV
{
class LedController
{
  public:
    // 构造和析构函数
    LedController();
    ~LedController();

    // 禁用拷贝和赋值
    LedController(const LedController &) = delete;
    LedController &operator=(const LedController &) = delete;

    // 获取单例实例
    static LedController *getInstance()
    {
        if (!instance) {
            instance = std::make_unique<LedController>();
        }
        return instance.get();
    }

    // 公共接口函数
    void updateState(EventBits_t xEventGroupBits);
    void startBlink();
    void stopBlink();

  private:
    // LED控制参数
    struct LedState {
        bool polluteBlink{false};
        bool faultBlink{false};
        bool sharedBlinkState{false};
    };

    // 私有成员变量
    LedState state;
    TimerHandle_t blinkTimer{nullptr};
    static std::unique_ptr<LedController> instance;

    // 定时器相关常量
    static constexpr TickType_t BLINK_PERIOD = pdMS_TO_TICKS(1000);

    // 私有成员函数
    static void blinkTimerCallback(TimerHandle_t xTimer);
    void handleBlink();
};
} // namespace DRV

#endif