/**
 * @file drv_can.h
 * @brief CAN驱动层C++实现
 * @version 1.0
 * @date 2024-11-12
 */

#pragma once
#ifdef __cplusplus

#include "FreeRTOS.h"
#include "event_groups.h"
#include "hal.hpp"
#include "queue.h"
#include "timers.h"

namespace DRV
{
/**
 * @class CANDriver
 * @brief 封装CAN驱动的C++类，实现单例模式和回调机制
 */
class CANDriver
{
  public:
    // 枚举定义
    enum class CANID : uint32_t {
        TEST1 = 0xDD01,
        TEST2 = 0xDD02,
        TEST3 = 0xDD03,
        TEST4 = 0xDD04,
    };

    // 添加队列相关方法
    void createTxQueue();
    QueueHandle_t getTxQueue() const { return m_canTxQueue; }
    bool sendToQueue(const void *value);

    // 构造和析构函数
    CANDriver();
    ~CANDriver();

    // 禁止拷贝和赋值
    CANDriver(const CANDriver &) = delete;
    CANDriver &operator=(const CANDriver &) = delete;

    // 公共接口函数
    static CANDriver *getInstance()
    {
        if (!mInstance) {
            mInstance = std::make_unique<CANDriver>();
        }
        return mInstance.get();
    }
    void initCANHandlers();
    void processReceivedData();

  private:
    // 私有成员变量
    static std::unique_ptr<CANDriver> mInstance;
    std::array<stc_can_rx_frame_t, 50> mRxFrames;
    std::map<uint32_t, std::function<void(const stc_can_rx_frame_t &)>> mCallbacks;
    QueueHandle_t m_canTxQueue{nullptr};

    // 私有成员函数
    void registerReceiveCallback(uint32_t canId,
                                 std::function<void(const stc_can_rx_frame_t &)> callback);
    bool initReceiveBuffer();
    void processCANReceivedFrame(const stc_can_rx_frame_t &rxData);
    void handleTest1(const stc_can_rx_frame_t &frame);
    void handleTest2(const stc_can_rx_frame_t &frame);
};
} // namespace DRV

#endif