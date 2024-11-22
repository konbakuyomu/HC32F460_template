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
    CLOSE_FAILED,  // 关门失败
    OPEN_SUCCESS,  // 开门成功
    CLOSE_SUCCESS  // 关门成功
};

enum ExitGatePhase {
    PHASE_IDLE,
    PHASE_INIT_WAIT_FIRST,  // 等待第一次堵转
    PHASE_INIT_PULL_DOWN,   // 初始化时下拉
    PHASE_INIT_WAIT_SECOND, // 等待第二次堵转
    PHASE_OPEN_PULL_DOWN,   // 开门时下拉
    PHASE_OPEN_WAIT,        // 开门等待
    PHASE_CLOSE_PULL_DOWN,  // 关门时下拉
    PHASE_CLOSE_WAIT        // 关门等待
};

class MotorController
{
  public:
    MotorController();
    ~MotorController();

    // 禁用拷贝和赋值
    MotorController(const MotorController &) = delete;
    MotorController &operator=(const MotorController &) = delete;

    // 单例相关
    static MotorController *getInstance()
    {
        if (!mMotor) {
            mMotor = std::make_unique<MotorController>();
        }
        return mMotor.get();
    }

    // 公共接口
    void updateStatus(EventBits_t xEventGroupBits);
    void resetFromEmergency();
    EventGroupHandle_t getEventGroupHandle() const { return motorControlEventGroupHandle; }

    // 入口闸机相关
    MotorState getEntranceGateState();
    void startOpenEntranceGate();
    void startCloseEntranceGate();
    void retryOpenEntranceGate();
    void retryCloseEntranceGate();
    void handleEntranceGateFailure();
    static void entranceGateTimeoutCallback(TimerHandle_t xTimer);

    // 出口闸机相关
    MotorState getExitGateState();
    void initExitGate();
    void startOpenExitGate();
    void startCloseExitGate();
    static void exitGateTimeoutCallback(TimerHandle_t xTimer);
    static void exitGatePhaseTimerCallback(TimerHandle_t xTimer);

  private:
    // 内部类型定义
    struct EntranceGateState {
        MotorState currentState{IDLE};
        int retryCount{0};
        static constexpr int maxRetryCount{99};
        bool isLevelDetecting{false};
        TimerHandle_t timeoutTimer{nullptr}; // 进门电机超时检测定时器
    };

    struct ExitGateState {
        MotorState currentState{IDLE};
        ExitGatePhase phase{PHASE_IDLE};
        TimerHandle_t phaseTimer{nullptr}; // 出门电机阶段定时器
        TimerHandle_t closeTimer{nullptr}; // 出门电机关闭定时器
    };

    // 私有成员变量
    EntranceGateState entranceGate;
    ExitGateState exitGate;
    EventGroupHandle_t motorControlEventGroupHandle{nullptr};
    static std::unique_ptr<MotorController> mMotor;

    // 定时器相关常量
    static constexpr TickType_t ENTRANCE_TIMEOUT_PERIOD = pdMS_TO_TICKS(5000);
    static constexpr TickType_t EXIT_PHASE_PERIOD = pdMS_TO_TICKS(1500);
    static constexpr TickType_t DOOR_CLOSE_PERIOD = pdMS_TO_TICKS(5000);

    // 私有成员函数
    TimerHandle_t createTimer(const char *name, TickType_t period, BaseType_t autoReload,
                              TimerCallbackFunction_t callback);
    void initTimers();
    void initEventGroup();
    void entranceMotorForward();
    void entranceMotorReverse();
    void entranceMotorStop();
    void handleExitGatePhase();
};
} // namespace DRV

#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "hc32_ll.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "timers.h"

EventGroupHandle_t Motor_GetEventGroupHandle(void);

#ifdef __cplusplus
}
#endif

