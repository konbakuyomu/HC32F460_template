/**
 * @file drv_can.cpp
 * @brief CAN驱动层C++实现
 * @version 1.0
 * @date 2024-11-14
 */

#include "globalConfig.h"

namespace DRV
{
// 初始化单例指针为空
std::unique_ptr<CANDriver> CANDriver::mInstance = nullptr;

/**
 * @brief 构造函数
 */
CANDriver::CANDriver()
{
    initReceiveBuffer();
    createTxQueue();
}

/**
 * @brief 创建发送队列
 */
void CANDriver::createTxQueue()
{
    if (m_canTxQueue == nullptr) {
        m_canTxQueue = xQueueCreate(10, sizeof(stc_can_tx_frame_t));
        if (m_canTxQueue == nullptr) {
            // 处理队列创建失败
            DDL_Printf("Failed to create CAN TX queue\n");
        }
    }
}

/**
 * @brief 发送数据到队列
 * @param data 数据
 * @param length 数据长度
 * @return 是否发送成功
 */
bool CANDriver::sendToQueue(const void *value)
{
    if (m_canTxQueue == nullptr || value == nullptr) {
        return false;
    }

    stc_can_tx_frame_t stcTx;
    uint32_t u32ExtendedID = 0xDD01;

    // 使用std::memcpy更安全地复制数据
    std::memcpy(stcTx.au8Data, value, CAN_DLC8);

    stcTx.u32Ctrl = 0x0UL;
    stcTx.IDE = 1UL; // 使用扩展ID
    stcTx.u32ID = u32ExtendedID;
    stcTx.DLC = CAN_DLC8;

    // 将发送帧加入到发送队列
    BaseType_t result = xQueueSend(m_canTxQueue, &stcTx, portMAX_DELAY);
    return (result == pdPASS);
}

/**
 * @brief 析构函数
 */
CANDriver::~CANDriver()
{
    if (m_canTxQueue != nullptr) {
        vQueueDelete(m_canTxQueue);
        m_canTxQueue = nullptr;
    }
}

/**
 * @brief 注册CAN消息处理函数
 */
void CANDriver::initCANHandlers()
{
    registerReceiveCallback(static_cast<uint32_t>(CANID::TEST1),
                            [this](const stc_can_rx_frame_t &frame) { handleTest1(frame); });

    registerReceiveCallback(static_cast<uint32_t>(CANID::TEST2),
                            [this](const stc_can_rx_frame_t &frame) { handleTest2(frame); });
}

/**
 * @brief 初始化接收缓冲区
 * @return 初始化是否成功
 */
bool CANDriver::initReceiveBuffer()
{
    // 使用std::array，无需动态内存分配，直接清零
    mRxFrames.fill(stc_can_rx_frame_t{});
    return true; // std::array初始化总是成功
}

/**
 * @brief 处理接收到的数据
 */
void CANDriver::processReceivedData()
{
    uint8_t u8RxFrameNum = 0U;
    int32_t i32Ret;

    // 清空接收缓冲区
    std::fill(mRxFrames.begin(), mRxFrames.end(), stc_can_rx_frame_t{});

    // 持续获取CAN帧，直到没有更多数据
    do {
        i32Ret = CAN_GetRxFrame(CAN_UNIT, &mRxFrames[u8RxFrameNum]);
        if (i32Ret == LL_OK) {
            u8RxFrameNum++;
            if (u8RxFrameNum >= CAN_RX_FRAME_NUM) {
                u8RxFrameNum = 0; // 缓冲区满了，从头开始覆盖
            }
        }
    } while (i32Ret == LL_OK);

    // 处理所有接收到的帧
    for (uint8_t i = 0; i < u8RxFrameNum; i++) {
        processCANReceivedFrame(mRxFrames[i]);
    }

    // 重新启用CAN接收中断
    CAN_IntCmd(CAN_UNIT, CAN_INT_RX, ENABLE);
}

/**
 * @brief 处理单个接收到的CAN帧
 * @param rxData 接收到的CAN帧数据
 */
void CANDriver::processCANReceivedFrame(const stc_can_rx_frame_t &rxData)
{
    // 查找是否有对应ID的回调函数
    auto it = mCallbacks.find(rxData.u32ID);
    if (it != mCallbacks.end()) {
        // 执行回调函数
        it->second(rxData);
    }
}

/**
 * @brief 注册CAN接收回调函数
 * @param canId CAN消息ID
 * @param callback 处理该ID消息的回调函数
 */
void CANDriver::registerReceiveCallback(uint32_t canId,
                                        std::function<void(const stc_can_rx_frame_t &)> callback)
{
    // 方式1：operator[]
    mCallbacks[canId] = std::move(callback);
    // 总是会工作：如果key存在就覆盖，不存在就新建

    // 方式2：emplace
    // mCallbacks.emplace(canId, std::move(callback));
    // 只有当key不存在时才会插入，返回一个pair
    // pair.second 表示插入是否成功
}

/**
 * @brief 处理TEST1消息
 * @param frame 接收到的CAN帧
 */
void CANDriver::handleTest1(const stc_can_rx_frame_t &frame) { HAL::turnOnLed("blue"); }

/**
 * @brief 处理TEST2消息
 * @param frame 接收到的CAN帧
 */
void CANDriver::handleTest2(const stc_can_rx_frame_t &frame) { HAL::turnOnLed("green"); }
} // namespace DRV