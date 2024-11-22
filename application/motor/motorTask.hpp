/**
 * @file motorTask.hpp
 * @brief 电机任务类
 * @version 1.0
 * @date 2024-11-22
 */
#pragma once
#ifdef __cplusplus

#include "baseTask.hpp"

namespace APP
{

class MotorTask : public BaseTask
{
  public:
    // 公共接口函数
    static MotorTask *getInstance()
    {
        if (!mInstance) {
            mInstance = std::make_unique<MotorTask>();
        }
        return mInstance.get();
    }
    static void task(void *pvParameters);

    // 构造和析构函数
    MotorTask() = default;
    virtual ~MotorTask() = default;

    // 禁止拷贝和赋值
    MotorTask(const MotorTask &) = delete;
    MotorTask &operator=(const MotorTask &) = delete;

  private:
    // 私有成员变量
    static std::unique_ptr<MotorTask> mInstance;
    // 私有成员函数
    static void handleMotorEvents(EventBits_t events);
};

} // namespace APP

#endif
