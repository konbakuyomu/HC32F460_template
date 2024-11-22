/**
 * @file uartTask.hpp
 * @brief Uart任务类
 * @version 1.0
 * @date 2024-11-22
 */
#pragma once
#ifdef __cplusplus

#include "baseTask.hpp"

namespace APP
{

typedef struct {
    uint8_t *data;
    size_t length;
} stc_uart_tx_frame_t;

class UartTask : public BaseTask
{
  public:
    // 公共接口函数
    static UartTask *getInstance()
    {
        if (!mInstance) {
            mInstance = std::make_unique<UartTask>();
        }
        return mInstance.get();
    }
    static void txTask(void *pvParameters);
    static void rxTask(void *pvParameters);

    // 构造和析构函数
    UartTask() = default;
    virtual ~UartTask() = default;

    // 禁止拷贝和赋值
    UartTask(const UartTask &) = delete;
    UartTask &operator=(const UartTask &) = delete;

  private:
    // 私有成员变量
    static std::unique_ptr<UartTask> mInstance;
    static constexpr uint32_t MAX_RETRY_COUNT = 3;

    // 私有成员函数
    static void handleTxFrame(const stc_uart_tx_frame_t &frame);
    static void handleRxData(uint32_t length);
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

TaskHandle_t UartTask_GetUartRxTaskHandle(void);
TaskHandle_t UartTask_GetUartTxTaskHandle(void);

#ifdef __cplusplus
}
#endif

