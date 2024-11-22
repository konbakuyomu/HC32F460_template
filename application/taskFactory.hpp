/**
 * @file taskFactory.hpp
 * @brief 任务工厂类
 * @version 1.0
 * @date 2024-11-22
 */
#pragma once
#ifdef __cplusplus

#include "baseTask.hpp"

namespace APP
{

class TaskFactory
{
  public:
    static bool createAllTasks();

  private:
    static bool createTask(TaskFunction_t taskFunction, const char *taskName,
                           TaskHandle_t *taskHandle);

    static void handleTaskCreationError(const char *taskName);
};

} // namespace APP

#endif
