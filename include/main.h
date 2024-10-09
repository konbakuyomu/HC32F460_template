/**
 * @file main.h
 * @brief 主程序头文件
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-09-13
 *
 * @details 包含主函数和系统初始化相关的声明和定义
 * 
 */
#ifndef __MAIN_H__
#define __MAIN_H__

/* 如果使用 C++ 编译器构建，则定义的 C 绑定 */
#ifdef __cplusplus
extern "C"
{
#endif

#include "hc32_ll.h"

/**
 * @brief 创建应用程序任务
 * 
 * 此函数用于创建应用程序中的各种任务。
 */
void AppTaskCreate(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
