/**
 * @file systemInit.hpp
 * @brief 系统初始化管理头文件
 * @version 1.0
 * @date 2024-09-13
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

/**
 * @class SystemInitializer
 * @brief 封装系统初始化的类，实现单例模式
 */
class SystemInitializer
{
  public:
    // 获取任务句柄相关方法
    TaskHandle_t getTaskHandle() const { return systemInitTaskHandle; }

    // 构造和析构函数
    SystemInitializer();
    virtual ~SystemInitializer() = default;

    // 禁止拷贝和赋值
    SystemInitializer(const SystemInitializer &) = delete;
    SystemInitializer &operator=(const SystemInitializer &) = delete;

    // 公共接口函数
    static SystemInitializer *getInstance()
    {
        if (!mInstance) {
            mInstance = std::make_unique<SystemInitializer>();
        }
        return mInstance.get();
    }
    static void initSystem(void *pvParameters);
    void initializeComponents();
    bool createSystemInitTask();

  private:
    // 私有成员变量
    static std::unique_ptr<SystemInitializer> mInstance;
    TaskHandle_t systemInitTaskHandle{nullptr};

    // 私有成员函数
    void initBspLayer();
    void initDriverLayer();
    static bool createTask(TaskFunction_t taskFunction, const char *taskName,
                           TaskHandle_t *taskHandle);
    static void handleTaskCreationError(const char *taskName);
};

#endif