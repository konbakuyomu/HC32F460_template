/**
 * @file drv_motor.h
 * @brief 电机驱动层实现
 * @version 1.0
 * @date 2024-11-05
 */

#pragma once
#ifdef __cplusplus

#include "FreeRTOS.h"
#include "event_groups.h"
#include "hal.hpp"
#include "timers.h"

namespace DRV
{
enum MotorState {
    IDLE,          // 空闲
    OPENING,       // 正在开门
    CLOSING,       // 正在关门
    OPEN_TIMEOUT,  // 开门超时
    CLOSE_TIMEOUT, // 关门超时
    OPEN_FAILED,   // 开门失败
    CLOSE_FAILED   // 关门失败
};

class MotorController
{
  private:
    static std::unique_ptr<MotorController> mMotor; // 单例

  public:
    MotorController();
    void updateStatus(EventBits_t xEventGroupBits);
    static MotorController *getInstance();
    void resetFromEmergency();

  public:
    MotorState getEntranceGateState();
    void startOpenEntranceGate();
    void startCloseEntranceGate();
    void retryOpenEntranceGate();
    void retryCloseEntranceGate();
    void handleEntranceGateFailure();
    static void entranceGateTimeoutCallback(TimerHandle_t xTimer);

  private:
    MotorState entranceGateCurrentState; // 进门电机当前状态
    int entranceGateRetryCount;          // 进门电机当前重试次数
    const int entranceGateMaxRetryCount; // 进门电机最大重试次数
    void entranceMotorForward();
    void entranceMotorReverse();
    void entranceMotorStop();
    void handleExitGateFailure();

  public:
    MotorState getExitGateState();
    void initExitGate();
    void startOpenExitGate();
    void startCloseExitGate();

  private:
    MotorState exitGateCurrentState; // 出门电机当前状态
};
} // namespace DRV

#endif
