/**
 * @file motorTask.cpp
 * @brief 电机任务类
 * @version 1.0
 * @date 2024-11-22
 */
#include "globalConfig.h"

namespace APP
{

std::unique_ptr<MotorTask> MotorTask::mInstance;

void MotorTask::task(void *pvParameters)
{
    EventBits_t xEventGroupBits;
    static constexpr EventBits_t MOTOR_BITS =
        EVENT_DOOR_ENTRY_SENSOR | EVENT_DOOR_EXIT_SENSOR | EVENT_ENTRY_MOTOR_OPEN_TO_POSITION |
        EVENT_ENTRY_MOTOR_CLOSE_TO_POSITION | EVENT_ENTRY_MOTOR_ON | EVENT_ENTRY_MOTOR_OFF |
        EVENT_EXIT_MOTOR_ON | EVENT_EXIT_MOTOR_OFF | EVENT_EMERGENCY_STOP;

    // 清除初始化期间可能设置的所有事件位
    auto motorController = DRV::MotorController::getInstance();
    EventGroupHandle_t motorEventGroupHandle = motorController->getEventGroupHandle();
    xEventGroupClearBits(motorEventGroupHandle, MOTOR_BITS);

    for (;;) {
        xEventGroupBits = xEventGroupWaitBits(motorEventGroupHandle, MOTOR_BITS,
                                              pdTRUE,  // 清除触发的事件位
                                              pdFALSE, // 任一事件触发即可
                                              portMAX_DELAY);

        handleMotorEvents(xEventGroupBits);
    }
}

void MotorTask::handleMotorEvents(EventBits_t events)
{
    DRV::MotorController::getInstance()->updateStatus(events);
}

} // namespace APP
