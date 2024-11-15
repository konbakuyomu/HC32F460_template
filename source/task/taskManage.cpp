/**
 * @file  taskManage.cpp
 * @brief 任务管理实现
 * @details 包含任务创建和管理的具体实现
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "globalConfig.h"

/**
 * @brief 创建TaskCreator实例
 * @return std::unique_ptr<TaskCreator> 返回TaskCreator实例的unique_ptr
 */
std::unique_ptr<TaskCreator> TaskCreator::create() { return std::make_unique<TaskCreator>(); }

/**
 * @brief 创建应用程序任务
 * @details 此函数用于创建应用程序中的各种任务。
 */
void TaskCreator::AppTaskCreate(void)
{
    /* 注入HAL层 */
    std::unique_ptr<HAL> hal = std::make_unique<DRV_HAL>();
    HAL::inject(hal.get());

    /* 创建任务 */
    // 使用 auto 而不是 auto*，std::unique_ptr 已经是一个指针类型的包装器，不需要额外的指针。
    auto taskCreator = TaskCreator::create();
    taskCreator->createTasks();

    /* 销毁FreeRTOS任务 */
    vTaskDelete(NULL);
}

/**
 * @brief 创建所有任务
 */
void TaskCreator::createTasks()
{
    taskENTER_CRITICAL();

    HAL_init();
    DRV_init();
    createQueues();
    createEventGroups();
    createTimers();
    createTask();

    taskEXIT_CRITICAL();
}

/**
 * @brief 初始化HAL类对象
 */
void TaskCreator::HAL_init()
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
}

/**
 * @brief 初始化驱动层
 */
void TaskCreator::DRV_init()
{
    /* 初始化电机驱动 */
    DRV::MotorController::init();
}

/**
 * @brief 创建工程中所需要的所有任务
 */
void TaskCreator::createTask()
{
    BaseType_t xReturn =
        xTaskCreate(LEDTask, "LEDTask", configMINIMAL_STACK_SIZE, nullptr, 3, &ledTaskHandle);
    if (xReturn != pdPASS) {
        // 处理错误
    }
    xReturn =
        xTaskCreate(UartTask, "UartTask", configMINIMAL_STACK_SIZE, nullptr, 3, &uartTaskHandle);
    if (xReturn != pdPASS) {
        // 处理错误
    }
    xReturn = xTaskCreate(MotorTask, "MotorTask", configMINIMAL_STACK_SIZE, nullptr, 3,
                          &motorTaskHandle);
    if (xReturn != pdPASS) {
        // 处理错误
    }
    xReturn = xTaskCreate(CANRXTask, "CANRXTask", configMINIMAL_STACK_SIZE, nullptr, 3,
                          &canRxTaskHandle);
    if (xReturn != pdPASS) {
        // 处理错误
    }
    xReturn = xTaskCreate(CANTXTask, "CANTXTask", configMINIMAL_STACK_SIZE, nullptr, 3,
                          &canTxTaskHandle);
    if (xReturn != pdPASS) {
        // 处理错误
    }
}

/**
 * @brief 创建工程中所需要的所有队列
 */
void TaskCreator::createQueues()
{
    // 实现创建队列的逻辑
    canTxQueueHandle = xQueueCreate(10, sizeof(stc_can_tx_frame_t));
}

/**
 * @brief 创建工程中所需要的所有事件组
 */
void TaskCreator::createEventGroups()
{
    // 实现创建事件组的逻辑
    motorControlEventGroupHandle = xEventGroupCreate();
}

/**
 * @brief 创建工程中所需要的所有定时器
 */
void TaskCreator::createTimers()
{
    // 实现创建定时器的逻辑
    MotorTimeoutTimer = xTimerCreate("MotorTimeoutTimer", pdMS_TO_TICKS(50000), pdFALSE,
                                     DRV::MotorController::getInstance(),
                                     DRV::MotorController::motorTimeoutCallback);
}

/**
 * @brief LED任务的静态回调函数
 * @param pvParameters 任务参数
 */
void TaskCreator::LEDTask(void *pvParameters)
{
    // 实现LED任务的逻辑
    for (;;) {
        HAL::toggleLed("Measure");
        DDL_Printf("Hello World\n");
        HAL::delayMillis(1000);
    }
}

/**
 * @brief 电机任务的静态回调函数
 * @param pvParameters 任务参数
 */
void TaskCreator::MotorTask(void *pvParameters)
{
    // 使用底层类型来定义等待的标志位
    const EventBits_t xBitsToWaitFor =
        EVENT_DOOR_ENTRY_MOTOR | EVENT_DOOR_EXIT_MOTOR | EVENT_HEIGHT_SENSOR | EVENT_OCCUPANCY_SENSOR;
    EventBits_t xEventGroupBits;

    auto motorController = DRV::MotorController::getInstance();
    motorController->startOpen();

    for (;;) {
        xEventGroupBits = xEventGroupWaitBits(motorControlEventGroupHandle, xBitsToWaitFor,
                                              pdTRUE,  // 收到事件后清除标志位
                                              pdFALSE, // 任一事件触发即可
                                              portMAX_DELAY);

        motorController->updateStatus(xEventGroupBits);
    }
}

/**
 * @brief Uart任务的静态回调函数
 * @param pvParameters 任务参数
 */
void TaskCreator::UartTask(void *pvParameters)
{
    uint32_t u32NotificationValue;

    // 实现Uart任务的逻辑
    for (;;) {
        if (xTaskNotifyWait(0, ULONG_MAX, &u32NotificationValue, portMAX_DELAY) == pdPASS) {
            uint16_t u16ReceivedLength = (uint16_t)u32NotificationValue;
            std::copy_n(hostReceiveBuffer, u16ReceivedLength,
                        hostTransmitBuffer);                      // 复制接收到的数据到发送缓冲区
            std::fill_n(hostReceiveBuffer, u16ReceivedLength, 0); // 使用std::fill_n清空接收缓冲区
            HAL::sendUart((void *)hostTransmitBuffer, u16ReceivedLength);
        }
    }
}

/**
 * @brief CAN发送任务的静态回调函数
 * @param pvParameters 任务参数
 */
void TaskCreator::CANTXTask(void *pvParameters)
{
    // 实现CAN发送任务的逻辑
    stc_can_tx_frame_t tx_frame;
    bool firstRun = true;

    for (;;) {
        while (xQueueReceive(canTxQueueHandle, &tx_frame, portMAX_DELAY) == pdTRUE) {
            // 第一次不执行等待CAN总线，因为还没有发送过
            if (!firstRun)
                // 等待CAN总线发送完成
                ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            else
                // 设置为false，下次发送时等待CAN总线
                firstRun = false;

            CAN_IntCmd(CM_CAN, CAN_INT_PTB_TX, ENABLE); // 使能发送中断
            (void)CAN_FillTxFrame(CM_CAN, CAN_TX_BUF_PTB,
                                  &tx_frame);    // 将发送帧的数据填充到发送缓冲区
            CAN_StartTx(CM_CAN, CAN_TX_REQ_PTB); // 启动发送
        }
    }
}

/**
 * @brief CAN接收任务的静态回调函数
 * @param pvParameters 任务参数
 */
void TaskCreator::CANRXTask(void *pvParameters)
{
    // 实现CAN接收任务的逻辑
    uint32_t u32NotificationValue;

    for (;;) {
        // 等待CAN总线接收完成
        u32NotificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // 如果中断在这个处理期间再次发生，ulNotificationValue将会大于1，可以来处理所有积累的消息
        while (u32NotificationValue > 0) {
            // CAN_ProcessReceivedData();
            u32NotificationValue--;
        }
    }
}
