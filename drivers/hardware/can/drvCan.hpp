/**
 * @file drv_can.h
 * @brief CAN驱动层C++实现
 * @version 1.0
 * @date 2024-11-12
 */

#pragma once
#ifdef __cplusplus

#include "hc32_ll.h"
#include <array>
#include <functional>
#include <map>
#include <memory>
#include <mutex>

namespace DRV
{
/**
 * @class CANDriver
 * @brief 封装CAN驱动的C++类，实现单例模式和回调机制
 */
class CANDriver
{
  public:
    enum class CANID : uint32_t {
        TEST1 = 0xDD01,
        TEST2 = 0xDD02,
        TEST3 = 0xDD03,
        TEST4 = 0xDD04,
    };

  public:
    static CANDriver *getInstance();
    void initCANHandlers();
    void sendData(const void *value);
    void processReceivedData();
    void registerReceiveCallback(uint32_t canId,
                                 std::function<void(const stc_can_rx_frame_t &)> callback);

    CANDriver() { initReceiveBuffer(); }
    ~CANDriver() = default;

  private:
    // 禁止拷贝和赋值
    CANDriver(const CANDriver &) = delete;
    CANDriver &operator=(const CANDriver &) = delete;
    bool initReceiveBuffer();
    void processCANReceivedFrame(const stc_can_rx_frame_t &rxData);
    static std::unique_ptr<CANDriver> mInstance;
    std::array<stc_can_rx_frame_t, 50> mRxFrames;

  private:
    void handleTest1(const stc_can_rx_frame_t &frame);
    void handleTest2(const stc_can_rx_frame_t &frame);

    // 存储CAN ID和对应的回调函数
    std::map<uint32_t, std::function<void(const stc_can_rx_frame_t &)>> mCallbacks;
};
} // namespace DRV

#endif