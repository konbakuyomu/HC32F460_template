/**
 * @file memory_management.cpp
 * @brief 内存管理实现
 * @details 包含内存分配、释放和管理的具体实现
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "FreeRTOS.h"
#include "malloc.h"
#include "task.h"
#include <cstddef>
#include <new>

/**
 * @brief 重载new操作符，用于分配内存
 * @param size 需要分配的内存大小
 * @return 分配的内存指针
 */
void *operator new(size_t size)
{
    return mymalloc(size);
}

/**
 * @brief 重载new[]操作符，用于分配数组内存
 * @param size 需要分配的内存大小
 * @return 分配的内存指针
 */
void *operator new[](size_t size)
{
    return mymalloc(size);
}

/**
 * @brief 重载delete操作符，用于释放内存
 * @param ptr 需要释放的内存指针
 */
void operator delete(void *ptr) noexcept
{
    myfree(ptr);
}

/**
 * @brief 重载带size参数的delete操作符
 * @param ptr 需要释放的内存指针
 * @param size 内存大小（未使用）
 */
void operator delete(void *ptr, std::size_t size) noexcept
{
    (void)size; // 防止未使用参数警告
    myfree(ptr);
}

/**
 * @brief 重载delete[]操作符，用于释放数组内存
 * @param ptr 需要释放的内存指针
 */
void operator delete[](void *ptr) noexcept
{
    myfree(ptr);
}

/**
 * @brief 重载带size参数的delete[]操作符
 * @param ptr 需要释放的内存指针
 * @param size 内存大小（未使用）
 */
void operator delete[](void *ptr, std::size_t size) noexcept
{
    (void)size; // 防止未使用参数警告
    myfree(ptr);
}
