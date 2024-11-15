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
    static std::unique_ptr<MotorController> mMotor;

  public:
    MotorController();
    void startOpen();
    void startClose();
    void retryOpen();
    void retryClose();
    void updateStatus(EventBits_t xEventGroupBits);
    MotorState getState();
    void notifyFailed();

  public:
    static void init();
    static MotorController *getInstance();
    static void motorTimeoutCallback(TimerHandle_t xTimer);

  private:
    MotorState currentState; // 当前状态
    int retryCount;          // 当前重试次数
    const int maxRetryCount; // 最大重试次数

  private:
    void motorForward();
    void motorReverse();
    void motorStop();
};
} // namespace DRV

#endif
