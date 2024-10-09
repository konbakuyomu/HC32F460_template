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

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"

/* 内存管理参数定义 */
#define MEM1_BLOCK_SIZE       32                              /**< 内存块大小为32字节 */
#define MEM1_MAX_SIZE         100 * 1024                      /**< 最大管理内存 100K，留出一些空间给堆栈 */
#define MEM1_ALLOC_TABLE_SIZE MEM1_MAX_SIZE / MEM1_BLOCK_SIZE /**< 内存表大小 */

/* 定义内存管理表类型 */
#define MT_TYPE uint16_t

/* 内存管理控制器 */
struct _m_mallco_dev {
    void (*init)(void);       /**< 初始化 */
    uint8_t (*perused)(void); /**< 内存使用率 */
    uint8_t *membase;         /**< 内存池 */
    MT_TYPE *memmap;          /**< 内存管理状态表 */
    uint8_t memrdy;           /**< 内存管理是否就绪 */
};

extern struct _m_mallco_dev mallco_dev; /**< 在mallco.c里面定义 */
extern SemaphoreHandle_t xMallocMutex;  /**< 互斥信号量，用于保护mymalloc和myfree的互斥访问 */

/**
 * @brief 内存管理初始化函数
 */
void my_mem_init(void);

/**
 * @brief 获得内存使用率
 * @return 内存使用率(0~100)
 */
uint8_t my_mem_perused(void);

/**
 * @brief 内存设置函数
 * @param s 内存首地址
 * @param c 要设置的值
 * @param count 需要设置的内存大小(字节为单位)
 */
void my_mem_set(void *s, uint8_t c, uint32_t count);

/**
 * @brief 内存拷贝函数
 * @param des 目的地址
 * @param src 源地址
 * @param n 需要复制的内存长度(字节为单位)
 */
void my_mem_copy(void *des, void *src, uint32_t n);

/**
 * @brief 内存释放(外部调用)
 * @param ptr 内存首地址
 * @note 此函数通过检查系统状态来决定是否使用互斥锁，以避免潜在的死锁问题
 */
void myfree(void *ptr);

/**
 * @brief 内存分配(外部调用)
 * @param size 要分配的内存大小(字节)
 * @return 分配到的内存首地址
 * @note 此函数通过检查系统状态来决定是否使用互斥锁，以避免潜在的死锁问题
 */
void *mymalloc(uint32_t size);

/**
 * @brief 重新分配内存(外部调用)
 * @param ptr 旧内存首地址
 * @param size 要分配的内存大小(字节)
 * @return 新分配到的内存首地址
 * @note 此函数通过检查系统状态来决定是否使用互斥锁，以避免潜在的死锁问题
 */
void *myrealloc(void *ptr, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
