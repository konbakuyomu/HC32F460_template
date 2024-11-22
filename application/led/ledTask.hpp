/**
 * @file ledTask.hpp
 * @brief LED任务类
 * @version 1.0
 * @date 2024-11-22
 */
#pragma once
#ifdef __cplusplus

#include "baseTask.hpp"

namespace APP
{

class LedTask : public BaseTask
{
  public:
    // 公共接口函数
    static LedTask *getInstance()
    {
        if (!mInstance) {
            mInstance = std::make_unique<LedTask>();
        }
        return mInstance.get();
    }
    static void task(void *pvParameters);

    // 构造和析构函数
    LedTask() = default;
    virtual ~LedTask() = default;

    // 禁止拷贝和赋值
    LedTask(const LedTask &) = delete;
    LedTask &operator=(const LedTask &) = delete;

  private:
    // 私有成员变量
    static std::unique_ptr<LedTask> mInstance;

    // 私有成员函数
    static void initLeds();
    static void handleLedEvents(EventBits_t events);
};

} // namespace APP

#endif
