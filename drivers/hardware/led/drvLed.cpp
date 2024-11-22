/**
 * @file drvLed.cpp
 * @brief LED驱动层C++实现
 * @version 1.0
 * @date 2024-11-21
 */

#include "globalConfig.h"

namespace DRV
{

std::unique_ptr<LedController> LedController::instance;

/**
 * @brief LED控制器构造函数
 */
LedController::LedController()
{
    // 创建定时器
    blinkTimer = xTimerCreate("LedBlink", BLINK_PERIOD,
                              pdTRUE, // 自动重载
                              this,   // 传入this指针作为timer ID
                              blinkTimerCallback);

    if (blinkTimer == nullptr) {
        // 处理定时器创建失败的情况
        // 可以抛出异常或记录日志
    }
}

/**
 * @brief LED控制器析构函数
 */
LedController::~LedController()
{
    if (blinkTimer != nullptr) {
        xTimerDelete(blinkTimer, portMAX_DELAY);
    }
}

/**
 * @brief 更新LED状态
 * @param xEventGroupBits 事件组标志位
 */
void LedController::updateState(EventBits_t xEventGroupBits)
{
    /* 控制报警灯开关 */
    if (xEventGroupBits & EVENT_ALARM_LED_ON) {
        state.polluteBlink = true;
    } else if (xEventGroupBits & EVENT_ALARM_LED_OFF) {
        state.polluteBlink = false;
        HAL::turnOffLed("Pollute");
    }

    /* 控制故障灯开关 */
    if (xEventGroupBits & EVENT_FAULT_LED_ON) {
        state.faultBlink = true;
    } else if (xEventGroupBits & EVENT_FAULT_LED_OFF) {
        state.faultBlink = false;
        HAL::turnOffLed("Fault");
    }
}

/**
 * @brief 启动LED闪烁
 */
void LedController::startBlink()
{
    if (blinkTimer != nullptr) {
        xTimerStart(blinkTimer, 0);
    }
}

/**
 * @brief 停止LED闪烁
 */
void LedController::stopBlink()
{
    if (blinkTimer != nullptr) {
        xTimerStop(blinkTimer, 0);
    }
}

/**
 * @brief LED闪烁定时器回调函数
 * @param xTimer 定时器句柄
 */
void LedController::blinkTimerCallback(TimerHandle_t xTimer)
{
    // 从定时器获取对象指针并调用实际的处理函数
    LedController *controller = static_cast<LedController *>(pvTimerGetTimerID(xTimer));
    if (controller != nullptr) {
        controller->handleBlink();
    }
}

/**
 * @brief 实际的LED闪烁处理函数
 */
void LedController::handleBlink()
{
    /* 更新共享的闪烁状态 */
    state.sharedBlinkState = !state.sharedBlinkState;

    /* 控制报警指示灯 */
    if (state.polluteBlink) {
        if (state.sharedBlinkState)
            HAL::turnOnLed("Pollute");
        else
            HAL::turnOffLed("Pollute");
    }

    /* 控制故障指示灯 */
    if (state.faultBlink) {
        if (state.sharedBlinkState)
            HAL::turnOnLed("Fault");
        else
            HAL::turnOffLed("Fault");
    }
}
} // namespace DRV