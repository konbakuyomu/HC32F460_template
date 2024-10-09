/**
 * @file taskManage.h
 * @brief 任务管理头文件
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-09-13
 *
 * @details 包含任务创建和管理相关的类声明和定义
 * 
 */

#pragma once
#ifndef __TASK_H
#define __TASK_H

#include <memory>
#include <array>
#include <string>
#include <unordered_map>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"

namespace TaskManager
{
    class TaskCreator
    {
    public:
        /**
         * @brief 创建TaskCreator实例
         * @return TaskCreator* 返回TaskCreator实例的指针
         */
        static TaskCreator *create();

        /**
         * @brief 销毁TaskCreator实例
         */
        void destroy();

        /**
         * @brief 创建所有任务
         */
        void createTasks();

    private:
        TaskCreator() = default;
        ~TaskCreator() = default;

        TaskCreator(const TaskCreator &)            = delete;
        TaskCreator &operator=(const TaskCreator &) = delete;

        /**
         * @brief 创建工程中所需要的所有任务
         */
        void createTask();

        /**
         * @brief 创建工程中所需要的所有队列
         */
        void createQueues();

        /**
         * @brief 创建工程中所需要的所有事件组
         */
        void createEventGroups();

        /**
         * @brief 创建工程中所需要的所有定时器
         */
        void createTimers();

        /**
         * @brief LED任务的静态回调函数
         * @param pvParameters 任务参数
         */
        static void LEDTask(void *pvParameters);

        /**
         * @brief 测试任务的静态回调函数
         * @param pvParameters 任务参数
         */
        static void TestTask(void *pvParameters);
    };

}

#endif

