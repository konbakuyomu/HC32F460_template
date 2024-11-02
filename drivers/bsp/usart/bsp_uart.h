/**
 * @file drv_uart.h
 * @brief 串口驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-10-11
 */

#pragma once

#ifdef __cplusplus

    #include "hc32_ll.h"
    #include "malloc.h"
    #include <algorithm>
    #include <array>
    #include <functional>
    #include <iostream>
    #include <memory>
    #include <stdlib.h>
    #include <type_traits>
    #include <vector>

/* 类定义 ------------------------------------------------------------------*/
namespace BSP_USART {
    /**
     * @brief 串口配置结构体
     * @note 该结构体用于配置串口的硬件参数
     */
    struct Config {
        /* TMR0 配置 */
        CM_TMR0_TypeDef *TMR0_UNIT;
        uint8_t TMR0_CH;
        uint32_t USART_TIMEOUT_BITS;

        /* 串口DMA配置 */
        CM_DMA_TypeDef *tx_dma_unit;
        uint8_t tx_dma_ch;
        uint32_t tx_dma_trig_sel;
        en_event_src_t tx_dma_trig_evt_src;
        uint32_t tx_dma_tc_int;
        uint32_t tx_dma_tc_flag;
        IRQn_Type tx_dma_tc_irqn;
        en_int_src_t tx_dma_tc_int_src;

        CM_DMA_TypeDef *rx_dma_unit;
        uint8_t rx_dma_ch;
        uint32_t rx_dma_trig_sel;
        en_event_src_t rx_dma_trig_evt_src;
        uint32_t rx_dma_tc_int;
        uint32_t rx_dma_tc_flag;
        IRQn_Type rx_dma_tc_irqn;
        en_int_src_t rx_dma_tc_int_src;
        uint32_t rx_dma_reconf_trig_sel;
        en_event_src_t rx_dma_reconf_trig_evt_src;

        /* 串口引脚配置 */
        uint16_t tx_port;
        uint16_t tx_pin;
        uint32_t tx_func;
        uint16_t rx_port;
        uint16_t rx_pin;
        uint32_t rx_func;

        /* 串口波特率 */
        uint32_t baud_rate;

        /* 串口 unit 设置 */
        CM_USART_TypeDef *unit;

        /* 串口缓冲区 */
        uint8_t *tx_buffer;
        size_t tx_buffer_size;
        uint8_t *rx_buffer;
        size_t rx_buffer_size;

        /* 串口中断配置 */
        IRQn_Type tx_cplt_irqn;
        en_int_src_t tx_cplt_int_src;
        IRQn_Type rx_full_irqn;
        en_int_src_t rx_full_int_src;
        IRQn_Type rx_err_irqn;
        en_int_src_t rx_err_int_src;
        IRQn_Type rx_timeout_irqn;
        en_int_src_t rx_timeout_int_src;
    };

    /**
     * @brief 串口类
     * @note 该类用于控制串口的硬件参数
     */
    class USART
    {
      public:
        USART(Config config) : m_config(config) {}
        virtual ~USART() {};
        static void registerInstance(std::unique_ptr<USART> instance);

      public:
        virtual void Init()
        {
            ConfigureDMA();
            ConfigureTimer(m_config.USART_TIMEOUT_BITS);
            ConfigureGPIO();
            ConfigureUSART();
            ConfigureInterrupts();
            EnableUSART();
        }
        void ConfigureGPIO();
        void ConfigureTimer(uint16_t timeoutBits);
        void ConfigureDMA();
        void ConfigureUSART();
        void ConfigureInterrupts();
        void EnableUSART();
        void ReconfigureUartDma();
        void SendData(void *data, size_t length);

      public:
        virtual void TMR0_FCG_ENABLE() = 0;
        virtual void TX_DMA_FCG_ENABLE() = 0;
        virtual void RX_DMA_FCG_ENABLE() = 0;
        virtual void FCG_ENABLE() = 0;
        virtual void TxComplete_IrqCallback() = 0;
        virtual void RxError_IrqCallback() = 0;
        virtual void RxTimeout_IrqCallback() = 0;
        virtual void RX_DMA_TC_IrqCallback() = 0;
        virtual void TX_DMA_TC_IrqCallback() = 0;

      public:
        static void addUSART(const Config &config);
        static USART *getUSART(size_t index);

      public:
        /**
         * @brief 串口配置
         */
        Config m_config;

        /**
         * @brief 串口实例列表
         */
        static std::vector<std::unique_ptr<USART>> m_instances;
    };

    class USART4 : public USART
    {
      public:
        USART4(Config config) : USART(config)
        {
            // reset() 方法用于重置 unique_ptr 所管理的对象
            // this 指向当前正在构造的 USART4 对象
            // 使 USART4_instance 获得了当前对象的所有权
            // USART4_instance.reset(this);
            // USART4_instance.release() 释放 USART4_instance 对当前对象的所有权，并返回该对象的原始指针
            // std::unique_ptr<USART>(...) 创建一个新的 unique_ptr<USART>，接管上一步释放的指针
            // registerInstance(std::unique_ptr<USART>(USART4_instance.release()));
        }

      public:
        void TMR0_FCG_ENABLE() override;
        void TX_DMA_FCG_ENABLE() override;
        void RX_DMA_FCG_ENABLE() override;
        void FCG_ENABLE() override;

        void TxComplete_IrqCallback() override;
        void RxError_IrqCallback() override;
        void RxTimeout_IrqCallback() override;
        void RX_DMA_TC_IrqCallback() override;
        void TX_DMA_TC_IrqCallback() override;

      public:
        static void USART4_Init();
        static void USART4_SendData(void *data, size_t length);
    };

} // namespace BSP_USART
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

    // C 兼容的函数声明
    void USART_SendData(void *data, size_t length);

#ifdef __cplusplus
}
#endif
