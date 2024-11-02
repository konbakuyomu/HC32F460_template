/**
 * @file  taskManage.cpp
 * @brief 任务管理实现
 * @details 包含任务创建和管理的具体实现
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "variable.h"

namespace TaskManager {

    /**
     * @brief 创建TaskCreator实例
     * @return std::unique_ptr<TaskCreator> 返回TaskCreator实例的unique_ptr
     */
    std::unique_ptr<TaskCreator> TaskCreator::create()
    {
        return std::make_unique<TaskCreator>();
    }
    /**
     * @brief 创建所有任务
     */
    void TaskCreator::createTasks()
    {
        taskENTER_CRITICAL();

        HAL_init();
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
        /* 系统配置 */
        HAL::system_init();
        /* 初始化UART */
        HAL::uart_init();
        /* 初始化PWM */
        HAL::pwm_init();
        /* 初始化CAN */
        HAL::can_init();
        /* 键值对配置 */
        HAL::global_key_value_init();
    }

    /**
     * @brief 创建工程中所需要的所有任务
     */
    void TaskCreator::createTask()
    {
        BaseType_t xReturn = xTaskCreate(LEDTask, "LEDTask", configMINIMAL_STACK_SIZE, nullptr, 3, &LEDTask_Handle);
        if (xReturn != pdPASS)
        {
            // 处理错误
        }
        xReturn = xTaskCreate(UartTask, "UartTask", configMINIMAL_STACK_SIZE, nullptr, 3, &UartTask_Handle);
        if (xReturn != pdPASS)
        {
            // 处理错误
        }
        xReturn = xTaskCreate(CANRXTask, "CANRXTask", configMINIMAL_STACK_SIZE, nullptr, 3, &CANRXTask_Handle);
        if (xReturn != pdPASS)
        {
            // 处理错误
        }
        xReturn = xTaskCreate(CANTXTask, "CANTXTask", configMINIMAL_STACK_SIZE, nullptr, 3, &CANTXTask_Handle);
        if (xReturn != pdPASS)
        {
            // 处理错误
        }
    }

    /**
     * @brief 创建工程中所需要的所有队列
     */
    void TaskCreator::createQueues()
    {
        // 实现创建队列的逻辑
        xQueue_CanTx = xQueueCreate(10, sizeof(stc_can_tx_frame_t));
    }

    /**
     * @brief 创建工程中所需要的所有事件组
     */
    void TaskCreator::createEventGroups()
    {
        // 实现创建事件组的逻辑
    }

    /**
     * @brief 创建工程中所需要的所有定时器
     */
    void TaskCreator::createTimers()
    {
        // 实现创建定时器的逻辑
    }

    /**
     * @brief LED任务的静态回调函数
     * @param pvParameters 任务参数
     */
    void TaskCreator::LEDTask(void *pvParameters)
    {
        // 实现LED任务的逻辑
        for (;;)
        {
            BSP_LED_Toggle(LED_BLUE);
            DDL_Printf("Hello World\n");
            vTaskDelay(1000);
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
        for (;;)
        {
            if (xTaskNotifyWait(0, ULONG_MAX, &u32NotificationValue, portMAX_DELAY) == pdPASS)
            {
                uint16_t u16ReceivedLength = (uint16_t)u32NotificationValue;
                std::copy_n(Host_RxBuf, u16ReceivedLength, Host_TxBuf);
                key_value_msg("USART4_SendData", (void *)Host_TxBuf, u16ReceivedLength);
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

        for (;;)
        {
            while (xQueueReceive(xQueue_CanTx, &tx_frame, portMAX_DELAY) == pdTRUE) // 从CAN发送队列中接收数据
            {
                // 第一次不执行等待CAN总线，因为还没有发送过
                if (!firstRun)
                {
                    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // 等待CAN总线发送完成
                } else
                {
                    firstRun = false;
                }

                CAN_IntCmd(CM_CAN, CAN_INT_PTB_TX, ENABLE);               // 使能发送中断
                (void)CAN_FillTxFrame(CM_CAN, CAN_TX_BUF_PTB, &tx_frame); // 将发送帧的数据填充到发送缓冲区
                CAN_StartTx(CM_CAN, CAN_TX_REQ_PTB);                      // 启动发送
            }
        }
    }

    void TaskCreator::CANRXTask(void *pvParameters)
    {
        // 实现CAN接收任务的逻辑
        uint32_t u32NotificationValue;

        for (;;)
        {
            // 等待CAN总线接收完成
            u32NotificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

            // 如果中断在这个处理期间再次发生，ulNotificationValue将会大于1，可以来处理所有积累的消息
            while (u32NotificationValue > 0)
            {
                // CAN_ProcessReceivedData();
                BSP_CAN::CAN::m_instance.get()->ProcessReceivedData();
                u32NotificationValue--;
            }
        }
    }

    extern "C"
    {
        void AppTaskCreate(void)
        {
            /* 注入HAL层 */
            std::unique_ptr<HAL> hal = std::make_unique<DRV_HAL>();
            HAL::inject(hal.get());

            /* 创建任务 */
            // 使用 auto 而不是 auto*，std::unique_ptr 已经是一个指针类型的包装器，不需要额外的指针。
            auto taskCreator = TaskManager::TaskCreator::create();
            taskCreator->createTasks();

            /* 销毁FreeRTOS任务 */
            vTaskDelete(NULL);
        }
    }
} // namespace TaskManager
