/**
 * @file baseTask.hpp
 * @brief 基础任务类
 * @version 1.0
 * @date 2024-11-22
 */
#pragma once
#ifdef __cplusplus

#include <memory>
#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

namespace APP
{

class BaseTask
{
  protected:
    static constexpr uint32_t STACK_SIZE = configMINIMAL_STACK_SIZE;
    static constexpr uint32_t TASK_PRIORITY = 3;

    // 任务状态
    enum class TaskState { INIT, RUNNING, SUSPENDED, ERROR } state{TaskState::INIT};

  public:
    virtual ~BaseTask() = default;

    // 任务状态检查
    bool isRunning() const { return state == TaskState::RUNNING; }
    TaskHandle_t task1Handle{nullptr};
    TaskHandle_t task2Handle{nullptr};
};

} // namespace APP

#endif
