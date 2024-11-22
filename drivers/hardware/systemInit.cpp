/**
 * @file systemInit.cpp
 * @brief 系统初始化管理实现
 * @details 包含系统各层初始化的具体实现
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "globalConfig.h"

std::unique_ptr<SystemInitializer> SystemInitializer::mInstance;

/**
 * @brief 构造函数
 */
SystemInitializer::SystemInitializer() : systemInitTaskHandle(nullptr)
{
    // 如果需要其他初始化，可以在这里添加
}

/**
 * @brief 初始化系统
 * @details 此函数用于初始化系统的各个组件
 */
void SystemInitializer::initSystem(void *pvParameters)
{
    /* 注入HAL层 */
    std::unique_ptr<HAL> hal = std::make_unique<DRV_HAL>();
    HAL::inject(hal.get());

    /* 初始化系统组件 */
    auto initializer = SystemInitializer::getInstance();
    initializer->initializeComponents();

    /* 创建任务 */
    APP::TaskFactory::createAllTasks();

    /* 初始化完成，删除初始化任务 */
    vTaskDelete(NULL);
}

/**
 * @brief 初始化所有系统组件
 */
void SystemInitializer::initializeComponents()
{
    taskENTER_CRITICAL();

    initBspLayer();
    initDriverLayer();

    taskEXIT_CRITICAL();
}

bool SystemInitializer::createSystemInitTask()
{
    return createTask(initSystem, "SystemInit", &systemInitTaskHandle);
}

/**
 * @brief 创建任务
 * @param taskFunction 任务函数
 * @param taskName 任务名称
 * @param taskHandle 任务句柄
 * @return 是否创建成功
 */
bool SystemInitializer::createTask(TaskFunction_t taskFunction, const char *taskName,
                                   TaskHandle_t *taskHandle)
{
    BaseType_t result =
        xTaskCreate(taskFunction, taskName, configMINIMAL_STACK_SIZE, nullptr, 3, taskHandle);

    if (result != pdPASS) {
        handleTaskCreationError(taskName);
        return false;
    }

    return true;
}

/**
 * @brief 处理任务创建失败
 * @param taskName 任务名称
 */
void SystemInitializer::handleTaskCreationError(const char *taskName)
{
    // 处理任务创建失败
    // 可以添加日志记录或其他错误处理逻辑
}

/**
 * @brief 初始化BSP层
 */
void SystemInitializer::initBspLayer()
{
    /* LED配置 */
    HAL::initLed();
    /* 初始化UART */
    HAL::initUart();
    /* 初始化PWM */
    HAL::initPwm();
    /* 初始化CAN */
    HAL::initCan();
    /* 初始化EIRQ */
    HAL::initEirq();
    /* 初始化GPIO */
    HAL::initGpio();
}

/**
 * @brief 初始化驱动层
 */
void SystemInitializer::initDriverLayer()
{
    /* 初始化Uart驱动 */
    auto uartDriver = DRV::UARTDriver::getInstance();
    uartDriver->initUARTHandlers();
    /* 初始化CAN驱动 */
    auto canDriver = DRV::CANDriver::getInstance();
    canDriver->initCANHandlers();
    /* 初始化电机 */
    auto motorController = DRV::MotorController::getInstance();
    motorController->initExitGate();
}
