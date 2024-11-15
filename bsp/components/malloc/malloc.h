/**
 * @file malloc.h
 * @brief 内存管理头文件
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-09-13
 *
 * @details 包含内存管理相关的声明和定义
 */

#ifndef __MALLOC_H
#define __MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "semphr.h"
#include <stdint.h>

/* 宏定义 ------------------------------------------------------------------*/

/* 内存管理参数定义 */
#define MEM1_BLOCK_SIZE       32         /**< 内存块大小为32字节 */
#define MEM1_MAX_SIZE         50 * 1024  /**< 最大管理内存 100K，留出一些空间给堆栈，这里只用了50k */
#define MEM1_ALLOC_TABLE_SIZE MEM1_MAX_SIZE / MEM1_BLOCK_SIZE /**< 内存表大小 */

/* 定义内存管理表类型 */
#define MT_TYPE uint16_t

/* 结构体 ------------------------------------------------------------------*/

/* 内存管理控制器 */
struct _m_mallco_dev {
    void (*init)(void);       /**< 初始化 */
    uint8_t (*perused)(void); /**< 内存使用率 */
    uint8_t *membase;         /**< 内存池 */
    MT_TYPE *memmap;          /**< 内存管理状态表 */
    uint8_t memrdy;           /**< 内存管理是否就绪 */
};

/* 全局变量 ------------------------------------------------------------------*/

extern struct _m_mallco_dev mallco_dev; /**< 在mallco.c里面定义 */
extern SemaphoreHandle_t xMallocMutex;  /**< 互斥信号量，用于保护mymalloc和myfree的互斥访问 */

/* 函数声明 ------------------------------------------------------------------*/

void my_mem_init(void);
uint8_t my_mem_perused(void);
void my_mem_set(void *s, uint8_t c, uint32_t count);
void my_mem_copy(void *des, void *src, uint32_t n);
void myfree(void *ptr);
void *mymalloc(uint32_t size);
void *myrealloc(void *ptr, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
