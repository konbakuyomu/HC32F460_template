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

MotorController::MotorController() : currentState(IDLE), retryCount(0), maxRetryCount(99) {}

void MotorController::init() { mMotor = std::make_unique<MotorController>(); }

MotorController *MotorController::getInstance() { return mMotor.get(); }

void MotorController::startOpen()
{
    retryCount = 0;
    if (currentState == IDLE || currentState == OPEN_TIMEOUT || currentState == OPEN_FAILED) {
        motorForward();
        currentState = OPENING;
        xTimerStart(MotorTimeoutTimer, 0);
    }
}

void MotorController::startClose()
{
    retryCount = 0;
    if (currentState == IDLE || currentState == CLOSE_TIMEOUT || currentState == CLOSE_FAILED) {
        motorReverse();
        currentState = CLOSING;
        xTimerStart(MotorTimeoutTimer, 0);
    }
}

void MotorController::retryOpen()
{
    if (currentState == OPENING)
        currentState = OPEN_TIMEOUT;
    motorForward();
    xTimerReset(MotorTimeoutTimer, 0);
}

void MotorController::retryClose()
{
    if (currentState == CLOSING)
        currentState = CLOSE_TIMEOUT;
    motorReverse();
    xTimerReset(MotorTimeoutTimer, 0);
}

void MotorController::updateStatus(EventBits_t xEventGroupBits)
{
    // 检查 进门电机事件
    if (xEventGroupBits & EVENT_DOOR_ENTRY_MOTOR) {
        HAL::toggleLed("Normal");
        if (currentState == OPENING || currentState == OPEN_TIMEOUT) {
            motorStop();
            xTimerStop(MotorTimeoutTimer, 0);
            currentState = IDLE;
        }
    }

    // 检查 出门电机事件
    if (xEventGroupBits & EVENT_DOOR_EXIT_MOTOR) {
        if (currentState == CLOSING || currentState == CLOSE_TIMEOUT) {
            motorStop();
            xTimerStop(MotorTimeoutTimer, 0);
            currentState = IDLE;
        }
    }
}

MotorState MotorController::getState() { return currentState; }

void MotorController::motorForward()
{
    // 调用您的电机正转函数
    HAL::setPwmDutyCycle("PWM1", 0.9f);
    HAL::startPwm("PWM1");
    HAL::setPwmDutyCycle("PWM2", 0.0f);
    HAL::startPwm("PWM2");
}

void MotorController::motorReverse()
{
    // 调用您的电机反转函数
    HAL::setPwmDutyCycle("PWM2", 0.9f);
    HAL::startPwm("PWM2");
    HAL::setPwmDutyCycle("PWM1", 0.0f);
    HAL::startPwm("PWM1");
}

void MotorController::motorStop()
{
    // 调用您的电机停止函数
    HAL::stopPwm("PWM1");
    HAL::stopPwm("PWM2");
}

void MotorController::motorTimeoutCallback(TimerHandle_t xTimer)
{
    MotorController *controller = (MotorController *)pvTimerGetTimerID(xTimer);
    controller->motorStop();

    if (controller->retryCount < controller->maxRetryCount) {
        controller->retryCount++;
        if (controller->currentState == OPENING || controller->currentState == OPEN_TIMEOUT)
            controller->retryOpen(); // 再次尝试开门
        else if (controller->currentState == CLOSING || controller->currentState == CLOSE_TIMEOUT)
            controller->retryClose(); // 再次尝试关门
    } else {
        controller->currentState =
            (controller->currentState == OPENING) ? OPEN_FAILED : CLOSE_FAILED;
        controller->notifyFailed();
    }
}

void MotorController::notifyFailed() { HAL::turnOnLed("Fault"); }
} // namespace DRV_MOTOR
