/**
 * @file  malloc.c
 * @brief 内存管理实现
 * @details 包含内存分配、释放和管理的具体实现
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "globalConfig.h"

SemaphoreHandle_t xMallocMutex = NULL;

/* 内存池(32字节对齐) */
static __attribute__((aligned(32))) uint8_t mem1base[MEM1_MAX_SIZE];

/* 内存管理表 */
static MT_TYPE mem1mapbase[MEM1_ALLOC_TABLE_SIZE];

/* 内存管理参数 */
const uint32_t memtblsize = MEM1_ALLOC_TABLE_SIZE; /**< 内存表大小 */
const uint32_t memblksize = MEM1_BLOCK_SIZE;       /**< 内存分块大小 */
const uint32_t memsize = MEM1_MAX_SIZE;            /**< 内存总大小 */

/* 内存管理控制器 */
struct _m_mallco_dev mallco_dev = {
    my_mem_init,    /**< 内存初始化 */
    my_mem_perused, /**< 内存使用率 */
    mem1base,       /**< 内存池 */
    mem1mapbase,    /**< 内存管理状态表 */
    0,              /**< 内存管理未就绪 */
};

/**
 * @brief 内存拷贝函数
 * @param des 目的地址
 * @param src 源地址
 * @param n 需要复制的内存长度(字节为单位)
 */
void my_mem_copy(void *des, void *src, uint32_t n)
{
    uint8_t *xdes = des;
    uint8_t *xsrc = src;
    while (n--)
        *xdes++ = *xsrc++;
}

/**
 * @brief 内存设置函数
 * @param s 内存首地址
 * @param c 要设置的值
 * @param count 需要设置的内存大小(字节为单位)
 */
void my_mem_set(void *s, uint8_t c, uint32_t count)
{
    uint8_t *xs = s;
    while (count--)
        *xs++ = c;
}

/**
 * @brief 内存管理初始化函数
 */
void my_mem_init(void)
{
    if (xMallocMutex == NULL) {
        xMallocMutex = xSemaphoreCreateMutex();
    }

    my_mem_set(mallco_dev.memmap, 0, memtblsize * 2); /**< 内存状态表数据清零 */
    mallco_dev.membase = (uint8_t *)SRAM_BASE;        /**< 设置内存池起始地址 */
    mallco_dev.memrdy = 1;                            /**< 内存管理初始化OK */
}

/**
 * @brief 获得内存使用率
 * @return 内存使用率(0~100)
 */
uint8_t my_mem_perused(void)
{
    uint32_t used = 0;
    uint32_t i;
    for (i = 0; i < memtblsize; i++) {
        if (mallco_dev.memmap[i])
            used++;
    }
    return (used * 100) / (memtblsize);
}

/**
 * @brief 内存分配函数(内部调用)
 * @param size 要分配的内存大小(字节)
 * @return 分配到的内存偏移地址,分配失败返回0xFFFFFFFF
 */
static uint32_t my_mem_malloc(uint32_t size)
{
    signed long offset = 0;
    uint32_t nmemb;     /**< 需要的内存块数 */
    uint32_t cmemb = 0; /**< 连续空内存块数 */
    uint32_t i;
    if (!mallco_dev.memrdy) {
        mallco_dev.init(); /**< 未初始化,先执行初始化 */
    }
    if (size == 0) /**< 不需要分配 */
        return 0xFFFFFFFF;
    nmemb = size / memblksize; /**< 获取需要分配的连续内存块数 */
    if (size % memblksize)     /**< 如果size不是memblksize的整数倍，则需要分配的内存块数加1 */
        nmemb++;
    for (offset = memtblsize - 1; offset >= 0; offset--) /**< 从内存池的末尾开始查找 */
    {
        if (!mallco_dev.memmap[offset])
            cmemb++; /**< 连续空内存块数增加 */
        else
            cmemb = 0;      /**< 连续内存块清零 */
        if (cmemb == nmemb) /**< 找到了连续nmemb个空内存块 */
        {
            for (i = 0; i < nmemb; i++) /**< 标注内存块非空 */
            {
                mallco_dev.memmap[offset + i] = nmemb;
            }
            return (offset * memblksize); /**< 返回偏移地址 */
        }
    }
    return 0xFFFFFFFF; /**< 未找到符合分配条件的内存块 */
}

/**
 * @brief 内存释放函数(内部调用)
 * @param offset 内存偏移地址
 * @return 0:成功 1:未初始化 2:偏移超出范围
 */
static uint8_t my_mem_free(uint32_t offset)
{
    int i;
    if (!mallco_dev.memrdy) {
        mallco_dev.init();
        return 1; /**< 未初始化 */
    }
    if (offset < memsize) /**< 偏移在内存池内 */
    {
        int index = offset / memblksize;      /**< 偏移所在内存块号码 */
        int nmemb = mallco_dev.memmap[index]; /**< 内存块数量 */
        for (i = 0; i < nmemb; i++)           /**< 内存块清零 */
        {
            mallco_dev.memmap[index + i] = 0;
        }
        return 0;
    } else
        return 2; /**< 偏移超区了 */
}

/**
 * @brief 内存释放(外部调用)
 * @param ptr 内存首地址
 * @note 此函数通过检查系统状态来决定是否使用互斥锁，以避免潜在的死锁问题
 */
void myfree(void *ptr)
{
    if (ptr == NULL)
        return;

    /**< 检查调度器是否已启动或当前是否在中断中 */
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED || xPortIsInsideInterrupt()) {
        uint32_t offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase;
        my_mem_free(offset);
    } else {
        /**< 系统正常运行时，使用互斥锁保护内存释放 */
        if (xSemaphoreTake(xMallocMutex, portMAX_DELAY) == pdTRUE) {
            uint32_t offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase;
            my_mem_free(offset);
            xSemaphoreGive(xMallocMutex);
        }
    }
}

/**
 * @brief 内存分配(外部调用)
 * @param size 要分配的内存大小(字节)
 * @return 分配到的内存首地址
 * @note 此函数通过检查系统状态来决定是否使用互斥锁，以避免潜在的死锁问题
 */
void *mymalloc(uint32_t size)
{
    void *ptr = NULL;

    if (xMallocMutex == NULL) {
        /**< 互斥锁未初始化，调用初始化函数 */
        my_mem_init();
    }

    /**< 检查调度器是否已启动或当前是否在中断中 */
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED || xPortIsInsideInterrupt()) {
        uint32_t offset = my_mem_malloc(size);
        if (offset != 0xFFFFFFFF) {
            ptr = (void *)((uint32_t)mallco_dev.membase + offset);
        }
    } else {
        /**< 系统正常运行时，使用互斥锁保护内存分配 */
        if (xSemaphoreTake(xMallocMutex, portMAX_DELAY) == pdTRUE) {
            uint32_t offset = my_mem_malloc(size);
            if (offset != 0xFFFFFFFF) {
                ptr = (void *)((uint32_t)mallco_dev.membase + offset);
            }
            xSemaphoreGive(xMallocMutex);
        }
    }
    return ptr;
}

/**
 * @brief 重新分配内存(外部调用)
 * @param ptr 旧内存首地址
 * @param size 要分配的内存大小(字节)
 * @return 新分配到的内存首地址
 * @note 此函数通过检查系统状态来决定是否使用互斥锁，以避免潜在的死锁问题
 */
void *myrealloc(void *ptr, uint32_t size)
{
    void *new_ptr = NULL;

    if (xMallocMutex == NULL) {
        /**< 互斥锁未初始化，调用初始化函数 */
        my_mem_init();
    }

    if (xSemaphoreTake(xMallocMutex, portMAX_DELAY) == pdTRUE) {
        uint32_t offset = my_mem_malloc(size);
        if (offset != 0xFFFFFFFF) {
            new_ptr = (void *)((uint32_t)mallco_dev.membase + offset);
            my_mem_copy(new_ptr, ptr, size);
            uint32_t old_offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase;
            my_mem_free(old_offset);
        }
        xSemaphoreGive(xMallocMutex);
    }
    return new_ptr;
}
