/**
 * @file canTask.hpp
 * @brief CAN任务类
 * @version 1.0
 * @date 2024-11-22
 */
#pragma once
#ifdef __cplusplus

#include "baseTask.hpp"

namespace APP
{

class CANTask : public BaseTask
{
  public:
    // 公共接口函数
    static CANTask *getInstance()
    {
        if (!mInstance) {
            mInstance = std::make_unique<CANTask>();
        }
        return mInstance.get();
    }
    static void txTask(void *pvParameters);
    static void rxTask(void *pvParameters);

    // 构造和析构函数
    CANTask() = default;
    virtual ~CANTask() = default;

    // 禁止拷贝和赋值
    CANTask(const CANTask &) = delete;
    CANTask &operator=(const CANTask &) = delete;

  private:
    // 私有成员变量
    static std::unique_ptr<CANTask> mInstance;

    // CAN发送相关
    static void handleTxFrame(const stc_can_tx_frame_t &frame);
    static bool waitForTxComplete(bool &firstRun);

    // CAN接收相关
    static void handleRxMessages(uint32_t messageCount);

    // 错误处理
    static void handleQueueError();

    // 常量定义
    static constexpr uint32_t TX_WAIT_TIMEOUT = portMAX_DELAY;
    static constexpr uint32_t RX_WAIT_TIMEOUT = portMAX_DELAY;
};

} // namespace APP

#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "hc32_ll.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "timers.h"

TaskHandle_t CANTask_GetCANRxTaskHandle(void);
TaskHandle_t CANTask_GetCANTxTaskHandle(void);

#ifdef __cplusplus
}
#endif
