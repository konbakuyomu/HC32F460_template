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

#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <memory>
#include <numeric>
#include <string>
#include <unordered_map>

class TaskCreator
{
  public:
    static std::unique_ptr<TaskCreator> create();
    static void AppTaskCreate(void);
    void createTasks();
    TaskCreator() = default;
    virtual ~TaskCreator() = default;

  private:
    TaskCreator(const TaskCreator &) = delete;
    TaskCreator &operator=(const TaskCreator &) = delete;

    void HAL_init();
    void DRV_init();
    void createTask();
    void createQueues();
    void createEventGroups();
    void createTimers();

    static void LEDTask(void *pvParameters);
    static void MotorTask(void *pvParameters);
    static void UartTask(void *pvParameters);
    static void CANTXTask(void *pvParameters);
    static void CANRXTask(void *pvParameters);
};

#endif
