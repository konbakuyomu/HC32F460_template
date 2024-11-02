/**
 * @file drv_hal.cpp
 * @brief 硬件驱动层实现
 * @version 1.0
 * @date 2024-10-28
 */

#include "variable.h"

void DRV_HAL::init()
{
    // 初始化bsp类对象
    m_pwm = std::make_unique<BSP_PWM::bsp_pwm>();
}

void DRV_HAL::_system_init()
{
    /* LED灯初始化 */
    BSP_LED_Init();
    /* CPU使用率统计初始化 */
    CPU_TMR0_Config();
    /* 开启CPU使用率统计 */
    Start_CPU_Utilization();
    /* 串口printf初始化 -> PB9:USART4_RX  PE6:USART4_TX */
    LL_PrintfInit(USART_UNIT, USART_BAUDRATE, USART_Config);
}

void DRV_HAL::_global_key_value_init()
{
    /* 初始化键值对线程锁 */
    key_value_mutex_init();
    /* 初始化键值对 */
    Key_Value_Init();
    /* 发送PWM启动命令 */
    key_value_msg("PWM_start", NULL, PWM_CONFIG_1);
    key_value_msg("PWM_start", NULL, PWM_CONFIG_2);
    key_value_msg("PWM_start", NULL, PWM_CONFIG_3);
    key_value_msg("PWM_start", NULL, PWM_CONFIG_4);
}

void DRV_HAL::_pwm_init()
{
    BSP_PWM::bsp_pwm::_bsp_pwm_init();
}

void DRV_HAL::_pwm_start(size_t index)
{
    auto *pwm = m_pwm.get()->_bsp_get_pwm(index);
    pwm->_bsp_pwm_start();
}

void DRV_HAL::_pwm_stop(size_t index)
{
    auto *pwm = m_pwm.get()->_bsp_get_pwm(index);
    pwm->_bsp_pwm_stop();
}

void DRV_HAL::_pwm_set_duty_cycle(size_t index, float duty_cycle)
{
    auto *pwm = m_pwm.get()->_bsp_get_pwm(index);
    pwm->_bsp_pwm_set_duty_cycle(duty_cycle);
}

void DRV_HAL::_pwm_set_frequency(size_t index, uint32_t frequency)
{
    auto *pwm = m_pwm.get()->_bsp_get_pwm(index);
    pwm->_bsp_pwm_set_frequency(frequency);
}

void DRV_HAL::_pwm_set_direction(size_t index, uint8_t direction)
{
    auto *pwm = m_pwm.get()->_bsp_get_pwm(index);
    pwm->_bsp_pwm_set_direction(direction);
}

uint8_t DRV_HAL::_pwm_get_direction(size_t index)
{
    auto *pwm = m_pwm.get()->_bsp_get_pwm(index);
    return pwm->_bsp_pwm_get_direction();
}

void DRV_HAL::_can_init()
{
    BSP_CAN::CAN::_bsp_can_init();
}

void DRV_HAL::_can_send(void *value)
{
    BSP_CAN::CAN::SendData(value);
}

void DRV_HAL::_uart_init()
{
    BSP_USART::USART4::USART4_Init();
}

void DRV_HAL::_uart_send(void *data, size_t length)
{
    BSP_USART::USART4::USART4_SendData(data, length);
}
