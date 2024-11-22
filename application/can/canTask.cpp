/**
 * @file canTask.cpp
 * @brief CAN任务类
 * @version 1.0
 * @date 2024-11-22
 */
#include "globalConfig.h"

namespace APP
{

std::unique_ptr<CANTask> CANTask::mInstance;

/**
 * @brief CAN发送任务
 * @param pvParameters 参数
 */
void CANTask::txTask(void *pvParameters)
{
    // 获取CAN驱动实例
    auto canDriver = DRV::CANDriver::getInstance();
    QueueHandle_t canTxQueue = canDriver->getTxQueue();

    if (canTxQueue == nullptr) {
        handleQueueError();
        return;
    }

    stc_can_tx_frame_t tx_frame;
    bool firstRun = true;

    for (;;) {
        // 等待发送队列消息
        while (xQueueReceive(canTxQueue, &tx_frame, TX_WAIT_TIMEOUT) == pdTRUE) {
            // 等待上一次发送完成（如果需要）
            if (!waitForTxComplete(firstRun)) {
                continue; // 发送超时或错误，继续下一次循环
            }

            // 处理发送帧
            handleTxFrame(tx_frame);
        }
    }
}

/**
 * @brief CAN接收任务
 * @param pvParameters 参数
 */
void CANTask::rxTask(void *pvParameters)
{
    uint32_t notificationValue;

    for (;;) {
        // 等待接收完成通知
        if (xTaskNotifyWait(0, ULONG_MAX, &notificationValue, RX_WAIT_TIMEOUT) == pdPASS) {
            handleRxMessages(notificationValue);
        }
    }
}

/**
 * @brief 处理发送帧
 * @param frame 发送帧
 */
void CANTask::handleTxFrame(const stc_can_tx_frame_t &frame)
{
    // 使能发送中断
    CAN_IntCmd(CM_CAN, CAN_INT_PTB_TX, ENABLE);

    // 填充发送缓冲区
    (void)CAN_FillTxFrame(CM_CAN, CAN_TX_BUF_PTB, &frame);

    // 启动发送
    CAN_StartTx(CM_CAN, CAN_TX_REQ_PTB);
}

/**
 * @brief 等待发送完成
 * @param firstRun 是否第一次运行
 * @return 是否发送完成
 */
bool CANTask::waitForTxComplete(bool &firstRun)
{
    if (!firstRun) {
        // 等待发送完成通知
        if (ulTaskNotifyTake(pdTRUE, TX_WAIT_TIMEOUT) == 0) {
            // 发送超时处理
            return false;
        }
    } else {
        firstRun = false;
    }
    return true;
}

/**
 * @brief 处理接收消息
 * @param messageCount 消息数量
 */
void CANTask::handleRxMessages(uint32_t messageCount)
{
    auto canDriver = DRV::CANDriver::getInstance();

    // 处理所有累积的消息
    while (messageCount > 0) {
        canDriver->processReceivedData();
        messageCount--;
    }
}

/**
 * @brief 处理队列错误
 */
void CANTask::handleQueueError()
{
    // 处理队列错误，可以添加日志记录
    // 删除任务
    vTaskDelete(nullptr);
}

} // namespace APP

#ifdef __cplusplus
extern "C" {
#endif

TaskHandle_t CANTask_GetCANRxTaskHandle(void)
{
    return APP::CANTask::getInstance()->task1Handle;
}

TaskHandle_t CANTask_GetCANTxTaskHandle(void)
{
    return APP::CANTask::getInstance()->task2Handle;
}

#ifdef __cplusplus
}
#endif
