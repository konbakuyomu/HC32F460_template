/**
 * @file taskFactory.cpp
 * @brief 任务工厂类
 * @version 1.0
 * @date 2024-11-22
 */
#include "globalConfig.h"

namespace APP
{

/**
 * @brief 创建所有任务
 * @return 是否创建成功
 */
bool TaskFactory::createAllTasks()
{
    bool success = true;

    // 获取任务实例
    auto ledTask = LedTask::getInstance();
    auto canTask = CANTask::getInstance();
    auto motorTask = MotorTask::getInstance();
    auto uartTask = UartTask::getInstance();

    // 直接使用类成员指针创建任务
    success &= createTask(LedTask::task, "LEDTask", &(ledTask->task1Handle));
    success &= createTask(UartTask::rxTask, "UartRXTask", &(uartTask->task1Handle));
    success &= createTask(UartTask::txTask, "UartTXTask", &(uartTask->task2Handle));
    success &= createTask(MotorTask::task, "MotorTask", &(motorTask->task1Handle));
    success &= createTask(CANTask::rxTask, "CANRXTask", &(canTask->task1Handle));
    success &= createTask(CANTask::txTask, "CANTXTask", &(canTask->task2Handle));

    return success;
}

/**
 * @brief 创建任务
 * @param taskFunction 任务函数
 * @param taskName 任务名称
 * @param taskHandle 任务句柄
 * @return 是否创建成功
 */
bool TaskFactory::createTask(TaskFunction_t taskFunction, const char *taskName,
                             TaskHandle_t *taskHandle)
{
    BaseType_t result =
        xTaskCreate(taskFunction, taskName, configMINIMAL_STACK_SIZE, nullptr, 3, taskHandle);

    if (result != pdPASS) {
        handleTaskCreationError(taskName);
        return false;
    }

    return true;
}

/**
 * @brief 处理任务创建失败
 * @param taskName 任务名称
 */
void TaskFactory::handleTaskCreationError(const char *taskName)
{
    // 处理任务创建失败
    // 可以添加日志记录或其他错误处理逻辑
}

} // namespace APP
