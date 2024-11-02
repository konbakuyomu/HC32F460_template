/**
 * @file taskManage.h
 * @brief 任务管理头文件
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-09-13
 *
 * @details 包含任务创建和管理相关的类声明和定义
 *
 */

#pragma once

#ifdef __cplusplus

#include <memory>
#include <algorithm>
#include <array>
#include <string>
#include <cstring>
#include <unordered_map>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"

namespace TaskManager
{
    class TaskCreator
    {
    public:
        static std::unique_ptr<TaskCreator> create();
        void createTasks();
        TaskCreator()          = default;
        virtual ~TaskCreator() = default;

    private:
        TaskCreator(const TaskCreator &)            = delete;
        TaskCreator &operator=(const TaskCreator &) = delete;

        void HAL_init();
        void createTask();
        void createQueues();
        void createEventGroups();
        void createTimers();

        static void LEDTask(void *pvParameters);
        static void UartTask(void *pvParameters);
        static void CANTXTask(void *pvParameters);
        static void CANRXTask(void *pvParameters);
    };

}
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 创建应用程序任务
 *
 * 此函数用于创建应用程序中的各种任务。
 */
void AppTaskCreate(void);

#ifdef __cplusplus
}
#endif
