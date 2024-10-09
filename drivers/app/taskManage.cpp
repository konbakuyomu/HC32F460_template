/**
 * @file  taskManage.cpp
 * @brief 任务管理实现
 * @details 包含任务创建和管理的具体实现
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "taskManage.h"
#include "variable.h"

namespace TaskManager
{
    TaskCreator *TaskCreator::create()
    {
        return new TaskCreator();
    }

    void TaskCreator::destroy()
    {
        delete this;
    }

    void TaskCreator::createTasks()
    {
        taskENTER_CRITICAL();

        createTask();
        createQueues();
        createEventGroups();
        createTimers();

        taskEXIT_CRITICAL();
    }

    void TaskCreator::createTask()
    {
        BaseType_t xReturn = xTaskCreate(LEDTask, "LEDTask", configMINIMAL_STACK_SIZE, nullptr, 3, &LEDTask_Handle);
        if (xReturn != pdPASS) {
            // 处理错误
        }
        xReturn = xTaskCreate(TestTask, "TestTask", configMINIMAL_STACK_SIZE, nullptr, 3, &TestTask_Handle);
        if (xReturn != pdPASS) {
            // 处理错误
        }
    }

    void TaskCreator::createQueues()
    {
        // 实现创建队列的逻辑
    }

    void TaskCreator::createEventGroups()
    {
        // 实现创建事件组的逻辑
    }

    void TaskCreator::createTimers()
    {
        // 实现创建定时器的逻辑
    }

    void TaskCreator::LEDTask(void *pvParameters)
    {
        // 实现LED任务的逻辑
        for (;;) {
            BSP_LED_Toggle(LED_BLUE);
            DDL_Printf("Hello World\n");
            vTaskDelay(500);
        }
    }

    void TaskCreator::TestTask(void *pvParameters)
    {
        // 实现测试任务的逻辑
        for (;;) {
            BSP_LED_Toggle(LED_RED);
            vTaskDelay(500);
        }
    }

} // namespace TaskManager

extern "C" {
void AppTaskCreate(void)
{
    /* 开启CPU使用率统计 */
    Start_CPU_Utilization();

    /* 创建任务 */
    auto *taskCreator = TaskManager::TaskCreator::create();
    taskCreator->createTasks();
    taskCreator->destroy();
    vTaskDelete(NULL);
}
}