/**
 * @file hal.cpp
 * @brief 硬件抽象层实现
 * @version 1.0
 * @date 2024-10-28
 */

#include "variable.h"

HAL *HAL::hal = nullptr;

bool HAL::inject(HAL *_hal)
{
    if (_hal == nullptr)
    {
        return false;
    }

    _hal->init();
    hal = _hal;
    return true;
}

HAL *HAL::get(void)
{
    return hal;
}

