/**
 * @file  GlobalConfig.c
 * @brief 全局变量定义
 * @details 包含FreeRTOS任务句柄等全局变量的定义
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "globalConfig.h"

/*---------------------- 串口 ----------------------*/
uint8_t hostReceiveBuffer[HOST_BUFFER_LENGTH];
uint8_t hostSendBuffer[HOST_BUFFER_LENGTH];

/**
 * @brief 安装中断处理函数
 * @param pstcConfig 中断配置结构体
 * @param u32Priority 中断优先级
 * @return void
 */
void installInterruptHandler(const stc_irq_signin_config_t *pstcConfig, uint32_t u32Priority)
{
    if (NULL != pstcConfig) {
        (void)INTC_IrqSignIn(pstcConfig);
        NVIC_ClearPendingIRQ(pstcConfig->enIRQn);
        NVIC_SetPriority(pstcConfig->enIRQn, u32Priority);
        NVIC_EnableIRQ(pstcConfig->enIRQn);
    }
}

/**
 * @brief 初始化动态缓冲区(串口)
 * @param buffer 指向 uint8_t 指针的指针，用于存储分配的内存地址
 * @param size 要分配的内存大小
 * @return 成功时返回分配的内存地址，失败时返回 NULL
 */
uint8_t *createDynamicBuffer(uint8_t **buffer, size_t size)
{
    if (buffer == NULL || size == 0) {
        return NULL;
    }

    *buffer = (uint8_t *)mymalloc(size);

    if (*buffer != NULL) {
        memset(*buffer, 0, size);
        return *buffer;
    } else {
        // 内存分配失败，可以在这里添加错误处理代码
        // 例如：打印错误信息或触发系统重置
        return NULL;
    }
}