/**
 * @file ledTask.cpp
 * @brief LED任务类
 * @version 1.0
 * @date 2024-11-22
 */
#include "globalConfig.h"

namespace APP
{

std::unique_ptr<LedTask> LedTask::mInstance;

/**
 * @brief LED任务
 * @param pvParameters 参数
 */
void LedTask::task(void *pvParameters)
{
    EventBits_t xEventGroupBits;
    static constexpr EventBits_t LED_BITS =
        EVENT_ALARM_LED_ON | EVENT_ALARM_LED_OFF | EVENT_FAULT_LED_ON | EVENT_FAULT_LED_OFF;

    // 初始化LED
    initLeds();

    // 清除初始化期间可能设置的所有事件位
    auto motorController = DRV::MotorController::getInstance();
    EventGroupHandle_t motorEventGroupHandle = motorController->getEventGroupHandle();
    xEventGroupClearBits(motorEventGroupHandle, LED_BITS);

    // 开启LED控制
    DRV::LedController::getInstance()->startBlink();

    for (;;) {
        xEventGroupBits = xEventGroupWaitBits(motorEventGroupHandle, LED_BITS,
                                              pdTRUE,  // 清除触发的事件位
                                              pdFALSE, // 任一事件触发即可
                                              portMAX_DELAY);

        handleLedEvents(xEventGroupBits);
    }
}

/**
 * @brief 初始化LED
 */
void LedTask::initLeds()
{
    HAL::turnOnLed("Normal");
    HAL::turnOnLed("Measure");
}

/**
 * @brief 处理LED事件
 * @param events 事件
 */
void LedTask::handleLedEvents(EventBits_t events)
{
    DRV::LedController::getInstance()->updateState(events);
}

} // namespace APP
