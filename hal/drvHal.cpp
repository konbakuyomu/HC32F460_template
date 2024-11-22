/**
 * @file drv_hal.cpp
 * @brief 硬件驱动层实现
 * @version 1.0
 * @date 2024-10-28
 */

#include "globalConfig.h"

void DRV_HAL::delayMillisImpl(unsigned long _mill) { vTaskDelay(_mill / portTICK_PERIOD_MS); }

unsigned long DRV_HAL::getTickImpl() { return xTaskGetTickCount(); }

/**
 * @brief LED初始化
 */
void DRV_HAL::initLedImpl() { BSP_LED::LED::init(); }

/**
 * @brief LED打开
 * @param name LED名称
 */
void DRV_HAL::turnOnLedImpl(const std::string &name) { BSP_LED::LED::turnOn(name); }

/**
 * @brief LED关闭
 * @param name LED名称
 */
void DRV_HAL::turnOffLedImpl(const std::string &name) { BSP_LED::LED::turnOff(name); }

/**
 * @brief LED切换
 * @param name LED名称
 */
void DRV_HAL::toggleLedImpl(const std::string &name)
{
    BSP_LED::LED::toggle(name); // 切换LED状态
}

/**
 * @brief PWM初始化
 */
void DRV_HAL::initPwmImpl() { BspPwm::BspPwm::init(); }

/**
 * @brief PWM启动
 * @param name PWM名称
 */
void DRV_HAL::startPwmImpl(const std::string &name) { BspPwm::BspPwm::startPwm(name); }

/**
 * @brief PWM停止
 * @param name PWM名称
 */
void DRV_HAL::stopPwmImpl(const std::string &name) { BspPwm::BspPwm::stopPwm(name); }

/**
 * @brief PWM设置占空比
 * @param name PWM名称
 * @param dutyCycle 占空比
 */
void DRV_HAL::setPwmDutyCycleImpl(const std::string &name, float dutyCycle)
{
    BspPwm::BspPwm::setPwmDutyCycle(name, dutyCycle);
}

/**
 * @brief PWM设置频率
 * @param name PWM名称
 * @param frequency 频率
 */
void DRV_HAL::setPwmFrequencyImpl(const std::string &name, uint32_t frequency)
{
    BspPwm::BspPwm::setPwmFrequency(name, frequency);
}

/**
 * @brief PWM设置方向
 * @param name PWM名称
 * @param direction 方向
 */
void DRV_HAL::setPwmDirectionImpl(const std::string &name, uint8_t direction)
{
    BspPwm::BspPwm::setPwmDirection(name, direction);
}

/**
 * @brief PWM获取方向
 * @param name PWM名称
 * @return 方向
 */
uint8_t DRV_HAL::getPwmDirectionImpl(const std::string &name)
{
    return BspPwm::BspPwm::getPwmDirection(name);
}

/**
 * @brief CAN初始化
 */
void DRV_HAL::initCanImpl() { initBspCanImpl(); }

/**
 * @brief CAN发送
 * @param value 发送数据
 */
void DRV_HAL::sendCanImpl(void *value) { DRV::CANDriver::getInstance()->sendToQueue(value); }

/**
 * @brief UART初始化
 */
void DRV_HAL::initUartImpl()
{
    USART4_Init();
}

/**
 * @brief UART发送
 * @param data 发送数据
 * @param length 数据长度
 */
void DRV_HAL::sendUartImpl(void *data, size_t length)
{
    USART4_SendData(data, length);
}

/**
 * @brief EIRQ外部中断初始化
 */
void DRV_HAL::initEirqImpl() { initBspEirq(); }

/**
 * @brief GPIO初始化
 */
void DRV_HAL::initGpioImpl() { initBspGpio(); }
