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
{
    initTimers();
    initEventGroup();
}

/**
 * @brief 电机控制器析构函数
 * @details 清理定时器
 */
MotorController::~MotorController()
{
    // 清理定时器
    if (entranceGate.timeoutTimer) {
        xTimerDelete(entranceGate.timeoutTimer, portMAX_DELAY);
    }
    if (exitGate.phaseTimer) {
        xTimerDelete(exitGate.phaseTimer, portMAX_DELAY);
    }
    if (exitGate.closeTimer) {
        xTimerDelete(exitGate.closeTimer, portMAX_DELAY);
    }

    // 清理事件组
    if (motorControlEventGroupHandle) {
        vEventGroupDelete(motorControlEventGroupHandle);
    }
}

/**
 * @brief 创建定时器
 * @details 创建定时器并传入this指针作为timer ID
 */
TimerHandle_t MotorController::createTimer(const char *name, TickType_t period,
                                           BaseType_t autoReload, TimerCallbackFunction_t callback)
{
    return xTimerCreate(name, period, autoReload,
                        this, // 传入this指针作为timer ID
                        callback);
}

/**
 * @brief 初始化定时器
 * @details 初始化进门电机超时检测定时器、出门电机阶段定时器和出门电机关闭定时器
 */
void MotorController::initTimers()
{
    // 创建进门电机超时检测定时器
    entranceGate.timeoutTimer = createTimer("EntranceTimeout", ENTRANCE_TIMEOUT_PERIOD, pdFALSE,
                                            entranceGateTimeoutCallback);

    // 创建出门电机相关定时器
    exitGate.phaseTimer =
        createTimer("ExitPhase", EXIT_PHASE_PERIOD, pdFALSE, exitGatePhaseTimerCallback);

    // 创建出门电机关闭定时器
    exitGate.closeTimer =
        createTimer("DoorClose", DOOR_CLOSE_PERIOD, pdFALSE, exitGateTimeoutCallback);
}

/**
 * @brief 初始化事件组
 * @details 初始化电机控制事件组
 */
void MotorController::initEventGroup() { motorControlEventGroupHandle = xEventGroupCreate(); }

/**
 * @brief 更新电机状态
 * @details 根据事件组标志位更新电机状态
 */
void MotorController::updateStatus(EventBits_t xEventGroupBits)
{
    /* 进门感应事件 */
    if (xEventGroupBits & EVENT_DOOR_ENTRY_SENSOR) {
        if (!entranceGate.isLevelDetecting) {
            entranceGate.isLevelDetecting = true;

            // 读取初始电平
            en_pin_state_t initialLevel = GPIO_ReadInputPins(EntranceSensorPort, EntranceSensorPin);
            uint8_t stableCount = 0;

            // 连续采样5次，每次间隔20ms
            for (uint8_t i = 0; i < 5; i++) {
                vTaskDelay(pdMS_TO_TICKS(20));
                en_pin_state_t currentLevel =
                    GPIO_ReadInputPins(EntranceSensorPort, EntranceSensorPin);

                if (currentLevel == initialLevel) {
                    stableCount++;
                } else {
                    // 如果电平变化，重置计数并更新初始电平
                    stableCount = 1;
                    initialLevel = currentLevel;
                }
            }

            // 如果有4次以上稳定读数，则认为电平真实有效
            if (stableCount >= 4) {
                DRV::UARTDriver::sendPersonStandingStatus(initialLevel);
            }

            entranceGate.isLevelDetecting = false;
        }
    }

    /* 出门感应事件 */
    if (xEventGroupBits & EVENT_DOOR_EXIT_SENSOR) {
        xTimerStart(exitGate.closeTimer, 0);
    }

    /* 进门电机开启到位事件 */
    if (xEventGroupBits & EVENT_ENTRY_MOTOR_OPEN_TO_POSITION) {
        if (entranceGate.currentState == OPENING || entranceGate.currentState == OPEN_TIMEOUT) {
            entranceMotorStop();
            xTimerStop(entranceGate.timeoutTimer, 0);
            entranceGate.currentState = OPEN_SUCCESS;
        }
    }

    /* 进门电机关闭到位事件 */
    if (xEventGroupBits & EVENT_ENTRY_MOTOR_CLOSE_TO_POSITION) {
        if (entranceGate.currentState == CLOSING || entranceGate.currentState == CLOSE_TIMEOUT) {
            entranceMotorStop();
            xTimerStop(entranceGate.timeoutTimer, 0);
            entranceGate.currentState = CLOSE_SUCCESS;
        }
    }

    /* 进门电机开启事件 */
    if (xEventGroupBits & EVENT_ENTRY_MOTOR_ON) {
        startOpenEntranceGate();
    }

    /* 进门电机关闭事件 */
    if (xEventGroupBits & EVENT_ENTRY_MOTOR_OFF) {
        startCloseEntranceGate();
    }

    /* 出门电机开启事件 */
    if (xEventGroupBits & EVENT_EXIT_MOTOR_ON) {
        startOpenExitGate();
    }

    /* 出门电机关闭事件 */
    if (xEventGroupBits & EVENT_EXIT_MOTOR_OFF) {
        startCloseExitGate();
    }

    /* 紧急停止事件(备用) */
    if (xEventGroupBits & EVENT_EMERGENCY_STOP) {
        // 停止进门电机
        if (entranceGate.currentState == OPENING || entranceGate.currentState == OPEN_TIMEOUT) {
            entranceMotorStop();
            xTimerStop(entranceGate.timeoutTimer, 0);
            entranceGate.currentState = OPEN_FAILED;
        } else if (entranceGate.currentState == CLOSING ||
                   entranceGate.currentState == CLOSE_TIMEOUT) {
            entranceMotorStop();
            xTimerStop(entranceGate.timeoutTimer, 0);
            entranceGate.currentState = CLOSE_FAILED;
        }

        // 停止出门电机(只需要拉高控制脚电平)
        if (exitGate.currentState == OPENING) {
            PullUpExitMotorControlPin();
            exitGate.currentState = OPEN_FAILED;
        } else if (exitGate.currentState == CLOSING) {
            PullUpExitMotorControlPin();
            exitGate.currentState = CLOSE_FAILED;
        }
    }
}

/**
 * @brief 从紧急状态复位
 * @details 备用
 */
void MotorController::resetFromEmergency()
{
    entranceGate.currentState = IDLE;
    exitGate.currentState = IDLE;
}

/**
 * @brief 获取进门电机状态
 * @details 返回进门电机状态
 */
MotorState MotorController::getEntranceGateState() { return entranceGate.currentState; }

/**
 * @brief 启动开门
 * @details 如果当前不是开门状态，则启动开门
 */
void MotorController::startOpenEntranceGate()
{
    entranceGate.retryCount = 0;
    if (entranceGate.currentState != OPENING) {
        entranceMotorForward();
        entranceGate.currentState = OPENING;
        xTimerStart(entranceGate.timeoutTimer, 0);
    }
}

/**
 * @brief 启动关门
 * @details 如果当前不是关门状态，则启动关门
 */
void MotorController::startCloseEntranceGate()
{
    entranceGate.retryCount = 0;
    if (entranceGate.currentState != CLOSING) {
        entranceMotorReverse();
        entranceGate.currentState = CLOSING;
        xTimerStart(entranceGate.timeoutTimer, 0);
    }
}

/**
 * @brief 再次尝试开门
 * @details 如果当前正在开门，则设置为开门超时状态，并再次尝试开门并启动软件定时器
 */
void MotorController::retryOpenEntranceGate()
{
    if (entranceGate.currentState == OPENING)
        entranceGate.currentState = OPEN_TIMEOUT;
    entranceMotorForward();
    xTimerReset(entranceGate.timeoutTimer, 0);
}

/**
 * @brief 再次尝试关门
 * @details 如果当前正在关门，则设置为关门超时状态，并再次尝试关门并启动软件定时器
 */
void MotorController::retryCloseEntranceGate()
{
    if (entranceGate.currentState == CLOSING)
        entranceGate.currentState = CLOSE_TIMEOUT;
    entranceMotorReverse();
    xTimerReset(entranceGate.timeoutTimer, 0);
}

/**
 * @brief 进门电机超时回调函数
 * @details 如果当前正在开门，则设置为开门超时状态，并再次尝试开门
 * @details 如果当前正在关门，则设置为关门超时状态，并再次尝试关门
 */
void MotorController::entranceGateTimeoutCallback(TimerHandle_t xTimer)
{
    MotorController *controller = (MotorController *)pvTimerGetTimerID(xTimer);
    controller->entranceMotorStop();

    if (controller->entranceGate.retryCount < controller->entranceGate.maxRetryCount) {
        controller->entranceGate.retryCount++;
        if (controller->entranceGate.currentState == OPENING ||
            controller->entranceGate.currentState == OPEN_TIMEOUT)
            controller->retryOpenEntranceGate(); // 再次尝试开门
        else if (controller->entranceGate.currentState == CLOSING ||
                 controller->entranceGate.currentState == CLOSE_TIMEOUT)
            controller->retryCloseEntranceGate(); // 再次尝试关门
    } else {
        controller->entranceGate.currentState =
            (controller->entranceGate.currentState == OPENING) ? OPEN_FAILED : CLOSE_FAILED;
        controller->handleEntranceGateFailure();
    }
}

/**
 * @brief 进门电机正转
 * @details 设置PWM1占空比为0.9，PWM2为0，启动PWM1和PWM2
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
 * @details 设置PWM2占空比为0.9，PWM1为0，启动PWM2和PWM1
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
 * @details 关闭PWM1和PWM2
 */
void MotorController::entranceMotorStop()
{
    // 调用您的电机停止函数
    HAL::stopPwm("PWM1");
    HAL::stopPwm("PWM2");
}

/**
 * @brief 处理出门电机阶段
 * @details 根据当前阶段处理出门电机
 */
void MotorController::handleExitGatePhase()
{
    switch (exitGate.phase) {
    case PHASE_INIT_WAIT_FIRST:
        // 第一次20s等待结束，开始下拉1.5s
        exitGate.phase = PHASE_INIT_PULL_DOWN;
        exitGate.currentState = CLOSING;
        PullDownExitMotorControlPin();
        xTimerChangePeriod(exitGate.phaseTimer, pdMS_TO_TICKS(1500), 0);
        xTimerStart(exitGate.phaseTimer, 0);
        break;

    case PHASE_INIT_PULL_DOWN:
        // 下拉1.5s结束，拉高并等待第二个20s
        exitGate.phase = PHASE_INIT_WAIT_SECOND;
        PullUpExitMotorControlPin();
        xTimerChangePeriod(exitGate.phaseTimer, pdMS_TO_TICKS(20000), 0);
        xTimerStart(exitGate.phaseTimer, 0);
        break;

    case PHASE_INIT_WAIT_SECOND:
        // 初始化完成
        exitGate.phase = PHASE_IDLE;
        exitGate.currentState = IDLE;
        break;

    case PHASE_OPEN_PULL_DOWN:
    case PHASE_CLOSE_PULL_DOWN:
        // 下拉1.5s结束，拉高并等待20s
        exitGate.phase =
            (exitGate.phase == PHASE_OPEN_PULL_DOWN) ? PHASE_OPEN_WAIT : PHASE_CLOSE_WAIT;
        PullUpExitMotorControlPin();
        xTimerChangePeriod(exitGate.phaseTimer, pdMS_TO_TICKS(20000), 0);
        xTimerStart(exitGate.phaseTimer, 0);
        break;

    case PHASE_OPEN_WAIT:
    case PHASE_CLOSE_WAIT:
        // 等待结束，回到空闲状态
        exitGate.phase = PHASE_IDLE;
        exitGate.currentState = IDLE;
        break;

    default:
        break;
    }
}

/**
 * @brief 处理进门电机失败
 * @details 打开故障指示灯
 */
void MotorController::handleEntranceGateFailure() { HAL::turnOnLed("Fault"); }

/**
 * @brief 获取出门电机状态
 * @details 返回出门电机状态
 */
MotorState MotorController::getExitGateState() { return exitGate.currentState; }

/**
 * @brief 初始化出门电机
 * @details 先等待20s，等待关门电机第一次堵转(即检测开门极限)，随后拉低电平1.5s
 * @details 随后拉高电平并等待20s，等待关门电机第二次堵转(即检测关门极限)
 */
void MotorController::initExitGate()
{
    exitGate.phase = PHASE_INIT_WAIT_FIRST;
    exitGate.currentState = OPENING;
    // 启动第一个20s定时
    xTimerChangePeriod(exitGate.phaseTimer, pdMS_TO_TICKS(20000), 0);
    xTimerStart(exitGate.phaseTimer, 0);
}

/**
 * @brief 启动开门
 * @details 拉低出门电机控制脚电平1.5s，随后拉高电平并等待20s
 */
void MotorController::startOpenExitGate()
{
    if (exitGate.currentState == IDLE) {
        exitGate.currentState = OPENING;
        exitGate.phase = PHASE_OPEN_PULL_DOWN;
        PullDownExitMotorControlPin();
        xTimerChangePeriod(exitGate.phaseTimer, pdMS_TO_TICKS(1500), 0);
        xTimerStart(exitGate.phaseTimer, 0);
    }
}

/**
 * @brief 启动关门
 * @details 拉低出门电机控制脚电平1.5s，随后拉高电平并等待20s
 */
void MotorController::startCloseExitGate()
{
    if (exitGate.currentState == IDLE) {
        exitGate.currentState = CLOSING;
        exitGate.phase = PHASE_CLOSE_PULL_DOWN;
        PullDownExitMotorControlPin();
        xTimerChangePeriod(exitGate.phaseTimer, pdMS_TO_TICKS(1500), 0);
        xTimerStart(exitGate.phaseTimer, 0);
    }
}

/**
 * @brief 出门电机超时回调函数
 * @details 关闭出门电机，并打开进门电机
 */
void MotorController::exitGateTimeoutCallback(TimerHandle_t xTimer)
{
    auto motorController = getInstance();
    xEventGroupSetBits(motorController->getEventGroupHandle(), EVENT_EXIT_MOTOR_OFF);
    xEventGroupSetBits(motorController->getEventGroupHandle(), EVENT_ENTRY_MOTOR_ON);
}

/**
 * @brief 出门电机阶段定时器回调函数
 * @details 处理出门电机阶段
 */
void MotorController::exitGatePhaseTimerCallback(TimerHandle_t xTimer)
{
    MotorController *controller = (MotorController *)pvTimerGetTimerID(xTimer);
    controller->handleExitGatePhase();
}
} // namespace DRV

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 获取电机控制事件组句柄
 * @return 电机控制事件组句柄
 */
EventGroupHandle_t Motor_GetEventGroupHandle()
{
    return DRV::MotorController::getInstance()->getEventGroupHandle();
}

#ifdef __cplusplus
}
#endif
