/**
 * @file drv_motor.cpp
 * @brief 电机驱动层实现
 * @version 1.0
 * @date 2024-11-05
 */

#include "globalConfig.h"

namespace DRV
{

std::unique_ptr<MotorController> MotorController::mMotor;

/**
 * @brief 电机控制器构造函数
 * @details 初始化电机控制器
 */
MotorController::MotorController()
    : entranceGateCurrentState(IDLE), entranceGateRetryCount(0), entranceGateMaxRetryCount(99),
      exitGateCurrentState(IDLE)
{
}

/**
 * @brief 更新电机状态
 * @details 根据事件组标志位更新电机状态
 */
void MotorController::updateStatus(EventBits_t xEventGroupBits)
{
    /* 进门感应事件 */
    if (xEventGroupBits & EVENT_DOOR_ENTRY_SENSOR) {
    }

    /* 紧急停止事件 */
    if (xEventGroupBits & EVENT_EMERGENCY_STOP) {
        // 停止进门电机
        if (entranceGateCurrentState == OPENING || entranceGateCurrentState == OPEN_TIMEOUT) {
            entranceMotorStop();
            xTimerStop(MotorTimeoutTimer, 0);
            entranceGateCurrentState = OPEN_FAILED;
        } else if (entranceGateCurrentState == CLOSING ||
                   entranceGateCurrentState == CLOSE_TIMEOUT) {
            entranceMotorStop();
            xTimerStop(MotorTimeoutTimer, 0);
            entranceGateCurrentState = CLOSE_FAILED;
        }

        // 停止出门电机(只需要拉高控制脚电平)
        if (exitGateCurrentState == OPENING) {
            PullUpExitMotorControlPin();
            exitGateCurrentState = OPEN_FAILED;
        } else if (exitGateCurrentState == CLOSING) {
            PullUpExitMotorControlPin();
            exitGateCurrentState = CLOSE_FAILED;
        }
    }
}

/**
 * @brief 获取电机控制器实例
 * @details 获取电机控制器实例
 */
MotorController *MotorController::getInstance()
{
    if (!mMotor) {
        mMotor = std::make_unique<MotorController>();
    }
    return mMotor.get();
}

/**
 * @brief 从紧急状态复位
 * @details 从紧急状态复位
 */
void MotorController::resetFromEmergency()
{
    entranceGateCurrentState = IDLE;
    exitGateCurrentState = IDLE;
}

/**
 * @brief 获取进门电机状态
 * @details 获取进门电机状态
 */
MotorState MotorController::getEntranceGateState() { return entranceGateCurrentState; }

/**
 * @brief 启动开门
 * @details 启动开门
 */
void MotorController::startOpenEntranceGate()
{
    entranceGateRetryCount = 0;
    if (entranceGateCurrentState == IDLE || entranceGateCurrentState == OPEN_TIMEOUT ||
        entranceGateCurrentState == OPEN_FAILED) {
        entranceMotorForward();
        entranceGateCurrentState = OPENING;
        xTimerStart(MotorTimeoutTimer, 0);
    }
}

/**
 * @brief 启动关门
 * @details 启动关门
 */
void MotorController::startCloseEntranceGate()
{
    entranceGateRetryCount = 0;
    if (entranceGateCurrentState == IDLE || entranceGateCurrentState == CLOSE_TIMEOUT ||
        entranceGateCurrentState == CLOSE_FAILED) {
        entranceMotorReverse();
        entranceGateCurrentState = CLOSING;
        xTimerStart(MotorTimeoutTimer, 0);
    }
}

/**
 * @brief 再次尝试开门
 * @details 再次尝试开门
 */
void MotorController::retryOpenEntranceGate()
{
    if (entranceGateCurrentState == OPENING)
        entranceGateCurrentState = OPEN_TIMEOUT;
    entranceMotorForward();
    xTimerReset(MotorTimeoutTimer, 0);
}

/**
 * @brief 再次尝试关门
 * @details 再次尝试关门
 */
void MotorController::retryCloseEntranceGate()
{
    if (entranceGateCurrentState == CLOSING)
        entranceGateCurrentState = CLOSE_TIMEOUT;
    entranceMotorReverse();
    xTimerReset(MotorTimeoutTimer, 0);
}

/**
 * @brief 进门电机超时回调函数
 * @details 进门电机超时回调函数
 */
void MotorController::entranceGateTimeoutCallback(TimerHandle_t xTimer)
{
    MotorController *controller = (MotorController *)pvTimerGetTimerID(xTimer);
    controller->entranceMotorStop();

    if (controller->entranceGateRetryCount < controller->entranceGateMaxRetryCount) {
        controller->entranceGateRetryCount++;
        if (controller->entranceGateCurrentState == OPENING ||
            controller->entranceGateCurrentState == OPEN_TIMEOUT)
            controller->retryOpenEntranceGate(); // 再次尝试开门
        else if (controller->entranceGateCurrentState == CLOSING ||
                 controller->entranceGateCurrentState == CLOSE_TIMEOUT)
            controller->retryCloseEntranceGate(); // 再次尝试关门
    } else {
        controller->entranceGateCurrentState =
            (controller->entranceGateCurrentState == OPENING) ? OPEN_FAILED : CLOSE_FAILED;
        controller->handleEntranceGateFailure();
    }
}

/**
 * @brief 进门电机正转
 * @details 进门电机正转
 */
void MotorController::entranceMotorForward()
{
    // 调用您的电机正转函数
    HAL::setPwmDutyCycle("PWM1", 0.9f);
    HAL::startPwm("PWM1");
    HAL::setPwmDutyCycle("PWM2", 0.0f);
    HAL::startPwm("PWM2");
}

/**
 * @brief 进门电机反转
 * @details 进门电机反转
 */
void MotorController::entranceMotorReverse()
{
    // 调用您的电机反转函数
    HAL::setPwmDutyCycle("PWM2", 0.9f);
    HAL::startPwm("PWM2");
    HAL::setPwmDutyCycle("PWM1", 0.0f);
    HAL::startPwm("PWM1");
}

/**
 * @brief 进门电机停止
 * @details 进门电机停止
 */
void MotorController::entranceMotorStop()
{
    // 调用您的电机停止函数
    HAL::stopPwm("PWM1");
    HAL::stopPwm("PWM2");
}

/**
 * @brief 处理进门电机失败
 * @details 处理进门电机失败
 */
void MotorController::handleEntranceGateFailure() { HAL::turnOnLed("Fault"); }

/**
 * @brief 获取出门电机状态
 * @details 获取出门电机状态
 */
MotorState MotorController::getExitGateState() { return exitGateCurrentState; }

/**
 * @brief 初始化出门电机
 * @details 初始化出门电机
 */
void MotorController::initExitGate()
{
    // 1. 开机等待20s，等待关门电机第一次堵转(即检测开门极限)
    exitGateCurrentState = OPENING;
    HAL::delayMillis(20000);
    // 2. 拉低电平 100ms 随后拉高电平并等待20s，等待关门电机第二次堵转(即检测关门极限)
    exitGateCurrentState = CLOSING;
    PullDownExitMotorControlPin();
    HAL::delayMillis(100);
    PullUpExitMotorControlPin();
    HAL::delayMillis(20000);
    // 3. 设置出门电机状态为空闲
    exitGateCurrentState = IDLE;
}

void MotorController::startOpenExitGate()
{
    if (exitGateCurrentState == IDLE) {
        exitGateCurrentState = OPENING;
        PullDownExitMotorControlPin();
        HAL::delayMillis(100);
        PullUpExitMotorControlPin();
        HAL::delayMillis(20000);
        exitGateCurrentState = IDLE;
    }
}

/**
 * @brief 启动关门
 * @details 启动关门
 */
void MotorController::startCloseExitGate()
{
    if (exitGateCurrentState == IDLE) {
        exitGateCurrentState = CLOSING;
        PullDownExitMotorControlPin();
        HAL::delayMillis(100);
        PullUpExitMotorControlPin();
        HAL::delayMillis(20000);
        exitGateCurrentState = IDLE;
    }
}

} // namespace DRV
