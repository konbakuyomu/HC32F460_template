#include "variable.h"

namespace BSP_USART {
    std::vector<std::unique_ptr<USART>> USART::m_instances;

    /**
     * @brief 添加串口实例
     * @param config 串口配置
     */
    void USART::addUSART(const Config &config)
    {
        if (config.unit == CM_USART4)
        {
            USART::registerInstance(std::make_unique<USART4>(config));
            m_instances.at(USART4_CONFIG).get()->Init();
        }
    }

    /**
     * @brief 获取串口实例
     * @param index 实例索引
     * @return 串口实例指针
     */
    USART *USART::getUSART(size_t index)
    {
        if (index < m_instances.size())
        {
            try
            {
                return m_instances.at(index).get();
            } catch (const std::out_of_range &e)
            {
                return nullptr;
            }
        }
        return nullptr;
    }

    /**
     * @brief 注册串口实例
     * @param instance 串口实例
     */
    void USART::registerInstance(std::unique_ptr<USART> instance)
    {
        m_instances.push_back(std::move(instance));
    }

    /**
     * @brief 配置串口DMA
     */
    void USART::ConfigureDMA()
    {
        /* 初始化DMA */
        stc_dma_init_t stcDmaInit;
        stc_irq_signin_config_t stcIrqSignConfig;

        /* DMA&AOS FCG 使能 */
        TX_DMA_FCG_ENABLE();
        RX_DMA_FCG_ENABLE();
        FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_AOS, ENABLE);

        /* DMA TX 初始化 */
        (void)DMA_StructInit(&stcDmaInit);
        stcDmaInit.u32IntEn = DMA_INT_ENABLE;
        stcDmaInit.u32BlockSize = 1UL;
        stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
        stcDmaInit.u32DestAddr = (uint32_t)(&m_config.unit->TDR);
        stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_INC;
        stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_FIX;
        DMA_Init(m_config.tx_dma_unit, m_config.tx_dma_ch, &stcDmaInit);

        stcIrqSignConfig.enIntSrc = m_config.tx_dma_tc_int_src;
        stcIrqSignConfig.enIRQn = m_config.tx_dma_tc_irqn;
        if (m_config.unit == CM_USART4)
            stcIrqSignConfig.pfnCallback = []() {
                static_cast<USART *>(m_instances.at(USART4_CONFIG).get())->TX_DMA_TC_IrqCallback();
            };
        else
            stcIrqSignConfig.pfnCallback = []() {
                static_cast<USART *>(m_instances.at(USART4_CONFIG).get())->TX_DMA_TC_IrqCallback();
            };
        INTC_IrqInstalHandler(&stcIrqSignConfig, DDL_IRQ_PRIO_DEFAULT);
        AOS_SetTriggerEventSrc(m_config.tx_dma_trig_sel, m_config.tx_dma_trig_evt_src);
        DMA_Cmd(m_config.tx_dma_unit, ENABLE);
        DMA_TransCompleteIntCmd(m_config.tx_dma_unit, m_config.tx_dma_tc_int, ENABLE);

        /* DMA RX 初始化 */
        (void)DMA_StructInit(&stcDmaInit);
        stcDmaInit.u32IntEn = DMA_INT_ENABLE;
        stcDmaInit.u32BlockSize = 1UL;
        stcDmaInit.u32TransCount = m_config.rx_buffer_size;
        stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
        stcDmaInit.u32DestAddr = (uint32_t)m_config.rx_buffer;
        stcDmaInit.u32SrcAddr = (uint32_t)(&m_config.unit->RDR);
        stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;
        stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC;
        DMA_Init(m_config.rx_dma_unit, m_config.rx_dma_ch, &stcDmaInit);

        stcIrqSignConfig.enIntSrc = m_config.rx_dma_tc_int_src;
        stcIrqSignConfig.enIRQn = m_config.rx_dma_tc_irqn;
        if (m_config.unit == CM_USART4)
            stcIrqSignConfig.pfnCallback = []() {
                static_cast<USART *>(m_instances.at(USART4_CONFIG).get())->RX_DMA_TC_IrqCallback();
            };
        else
            stcIrqSignConfig.pfnCallback = []() {
                static_cast<USART *>(m_instances.at(USART4_CONFIG).get())->RX_DMA_TC_IrqCallback();
            };
        INTC_IrqInstalHandler(&stcIrqSignConfig, DDL_IRQ_PRIO_DEFAULT);
        AOS_SetTriggerEventSrc(m_config.rx_dma_trig_sel, m_config.rx_dma_trig_evt_src);
        DMA_Cmd(m_config.rx_dma_unit, ENABLE);
        DMA_TransCompleteIntCmd(m_config.rx_dma_unit, m_config.rx_dma_tc_int, ENABLE);
        (void)DMA_ChCmd(m_config.rx_dma_unit, m_config.rx_dma_ch, ENABLE);
    }

    /**
     * @brief 配置串口超时接收定时器
     * @param timeoutBits 超时接收定时器位数
     */
    void USART::ConfigureTimer(uint16_t timeoutBits)
    {
        uint16_t u16Div;             // 时钟分频系数
        uint16_t u16Delay;           // 延时
        uint16_t u16CompareValue;    // 比较值
        stc_tmr0_init_t stcTmr0Init; // 定时器初始化结构体

        TMR0_FCG_ENABLE();
        CLK_LrcCmd(ENABLE);

        /* Initialize TMR0 base function. */
        stcTmr0Init.u32ClockSrc = TMR0_CLK_SRC_XTAL32; // 时钟源为内部低速时钟
        stcTmr0Init.u32ClockDiv = TMR0_CLK_DIV8;       // 时钟分频系数为8
        stcTmr0Init.u32Func = TMR0_FUNC_CMP;           // 定时器工作在比较模式
        if (TMR0_CLK_DIV1 == stcTmr0Init.u32ClockDiv)
        {
            u16Delay = 7U;
        } else if (TMR0_CLK_DIV2 == stcTmr0Init.u32ClockDiv)
        {
            u16Delay = 5U;
        } else if ((TMR0_CLK_DIV4 == stcTmr0Init.u32ClockDiv) ||
                   (TMR0_CLK_DIV8 == stcTmr0Init.u32ClockDiv) ||
                   (TMR0_CLK_DIV16 == stcTmr0Init.u32ClockDiv))
        {
            u16Delay = 3U;
        } else
        {
            u16Delay = 2U;
        }

        u16Div = (uint16_t)1U << (stcTmr0Init.u32ClockDiv >> TMR0_BCONR_CKDIVA_POS);
        u16CompareValue = ((timeoutBits + u16Div - 1U) / u16Div) - u16Delay;
        stcTmr0Init.u16CompareValue = u16CompareValue;
        (void)TMR0_Init(m_config.TMR0_UNIT, m_config.TMR0_CH, &stcTmr0Init);

        /** 在这里连接的就是USART的TIMEOUT，控制寄存器
         * （USART_CR1）的UARTTIMEOUT功能使能位
         * 使能之后就可以自动触发TMR0开始计时了 */
        TMR0_HWStartCondCmd(m_config.TMR0_UNIT, m_config.TMR0_CH, ENABLE);
        TMR0_HWClearCondCmd(m_config.TMR0_UNIT, m_config.TMR0_CH, ENABLE);
    }

    /**
     * @brief 配置串口GPIO
     */
    void USART::ConfigureGPIO()
    {
        stc_gpio_init_t stcGpioInit;

        (void)GPIO_StructInit(&stcGpioInit);
        stcGpioInit.u16PullUp = PIN_PU_ON;
        GPIO_Init(m_config.rx_port, m_config.rx_pin, &stcGpioInit);
        GPIO_Init(m_config.tx_port, m_config.tx_pin, &stcGpioInit);
        /* 复用为串口发送和接收 */
        GPIO_SetFunc(m_config.rx_port, m_config.rx_pin, m_config.rx_func);
        GPIO_SetFunc(m_config.tx_port, m_config.tx_pin, m_config.tx_func);
    }

    /**
     * @brief 配置串口
     */
    void USART::ConfigureUSART()
    {
        stc_usart_uart_init_t stcUartInit;

        FCG_ENABLE();

        (void)USART_UART_StructInit(&stcUartInit);
        stcUartInit.u32ClockDiv = USART_CLK_DIV64;
        stcUartInit.u32CKOutput = USART_CK_OUTPUT_ENABLE;
        stcUartInit.u32Baudrate = m_config.baud_rate;
        stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
        USART_UART_Init(m_config.unit, &stcUartInit, NULL);
    }

    /**
     * @brief 配置串口中断
     */
    void USART::ConfigureInterrupts()
    {
        stc_irq_signin_config_t stcIrqSigninConfig;

        /* 注册 USART 发送完成中断处理函数 */
        stcIrqSigninConfig.enIRQn = m_config.tx_cplt_irqn;
        stcIrqSigninConfig.enIntSrc = m_config.tx_cplt_int_src;
        if (m_config.unit == CM_USART4)
            stcIrqSigninConfig.pfnCallback = []() {
                static_cast<USART *>(m_instances.at(USART4_CONFIG).get())->TxComplete_IrqCallback();
            };
        else
            stcIrqSigninConfig.pfnCallback = []() {
                static_cast<USART *>(m_instances.at(USART4_CONFIG).get())->TxComplete_IrqCallback();
            };
        INTC_IrqInstalHandler(&stcIrqSigninConfig, DDL_IRQ_PRIO_DEFAULT);

        /** 注册 USART 接收错误中断处理函数
         *  USART接收错误的中断。当USART模块在接收数据时遇到错误（比如奇偶校验错误、帧错误、噪声错误或者溢出）时，这个中断会被触发
         */
        stcIrqSigninConfig.enIRQn = m_config.rx_err_irqn;
        stcIrqSigninConfig.enIntSrc = m_config.rx_err_int_src;
        if (m_config.unit == CM_USART4)
            stcIrqSigninConfig.pfnCallback = []() {
                static_cast<USART *>(m_instances.at(USART4_CONFIG).get())->RxError_IrqCallback();
            };
        else
            stcIrqSigninConfig.pfnCallback = []() {
                static_cast<USART *>(m_instances.at(USART4_CONFIG).get())->RxError_IrqCallback();
            };
        INTC_IrqInstalHandler(&stcIrqSigninConfig, DDL_IRQ_PRIO_DEFAULT);

        /* 注册 USART 接收超时中断处理函数 */
        // 这是USART接收超时的中断。当USART接收到一个字节的数据后，如果在超时时间内没有接收到下一个字节的数据，这个中断会被触发
        stcIrqSigninConfig.enIRQn = m_config.rx_timeout_irqn;
        stcIrqSigninConfig.enIntSrc = m_config.rx_timeout_int_src;
        if (m_config.unit == CM_USART4)
            stcIrqSigninConfig.pfnCallback = []() {
                static_cast<USART *>(m_instances.at(USART4_CONFIG).get())->RxTimeout_IrqCallback();
            };
        else
            stcIrqSigninConfig.pfnCallback = []() {
                static_cast<USART *>(m_instances.at(USART4_CONFIG).get())->RxTimeout_IrqCallback();
            };
        INTC_IrqInstalHandler(&stcIrqSigninConfig, DDL_IRQ_PRIO_DEFAULT);
    }

    /**
     * @brief 使能串口
     */
    void USART::EnableUSART()
    {
        /* 使能USART的接收功能和接收中断 */
        /* 这里的使能 USART_INT_RX | USART_RX_TIMEOUT 就是 TMR0的内部硬件触发事件和USART串口连接的桥梁*/
        /* 本质上这里是使能了控制寄存器 1（USART_CR1）的 b0和b1两个位（UARTTIMEOUT功能使能位） */
        USART_FuncCmd(m_config.unit, (USART_RX | USART_INT_RX | USART_RX_TIMEOUT | USART_INT_RX_TIMEOUT), ENABLE);
    }

    /**
     * @brief 发送数据
     * @param data 数据指针
     * @param length 数据长度
     */
    void USART::SendData(void *data, size_t length)
    {
        uint8_t *pData = static_cast<uint8_t *>(data);
        DMA_SetTransCount(m_config.tx_dma_unit, m_config.tx_dma_ch, length);       // 设置传输数据长度
        DMA_SetSrcAddr(m_config.tx_dma_unit, m_config.tx_dma_ch, (uint32_t)pData); // 设置源地址
        (void)DMA_ChCmd(m_config.tx_dma_unit, m_config.tx_dma_ch, ENABLE);         // 使能通道
        USART_FuncCmd(m_config.unit, USART_TX, ENABLE);                            // 使能发送
    }

    /**
     * @brief 重新配置串口DMA
     */
    void USART::ReconfigureUartDma()
    {
        stc_dma_init_t stcDmaInit;

        (void)DMA_StructInit(&stcDmaInit);
        stcDmaInit.u32IntEn = DMA_INT_ENABLE;
        stcDmaInit.u32BlockSize = 1UL;
        stcDmaInit.u32TransCount = m_config.rx_buffer_size;
        stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
        stcDmaInit.u32DestAddr = (uint32_t)m_config.rx_buffer;
        stcDmaInit.u32SrcAddr = (uint32_t)(&m_config.unit->RDR);
        stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;
        stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC;
        DMA_Init(m_config.rx_dma_unit, m_config.rx_dma_ch, &stcDmaInit);
    }

    /**
     * @brief 使能TMR0时钟
     */
    void USART4::TMR0_FCG_ENABLE() { FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMR0_2, ENABLE); }

    /**
     * @brief 使能TX DMA时钟
     */
    void USART4::TX_DMA_FCG_ENABLE() { FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA2, ENABLE); }

    /**
     * @brief 使能RX DMA时钟
     */
    void USART4::RX_DMA_FCG_ENABLE() { FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA1, ENABLE); }

    /**
     * @brief 使能FCG时钟
     */
    void USART4::FCG_ENABLE() { FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART4, ENABLE); }

    /**
     * @brief 发送完成中断回调函数
     */
    void USART4::TxComplete_IrqCallback()
    {
        USART_FuncCmd(m_config.unit, (USART_TX | USART_INT_TX_CPLT), DISABLE);          // 关闭串口发送功能和发送完成中断
        USART_ClearStatus(m_config.unit, USART_FLAG_TX_CPLT);                           // 清除发送完成标志位
        USART_ClearStatus(m_config.unit, USART_FLAG_RX_TIMEOUT);                        // 清除接收超时标志位
        TMR0_Stop(m_config.TMR0_UNIT, m_config.TMR0_CH);                                // 停止定时器
        USART_FuncCmd(m_config.unit, USART_RX_TIMEOUT, ENABLE);                         // 使能接收超时中断
        std::fill(m_config.rx_buffer, m_config.rx_buffer + m_config.rx_buffer_size, 0); // 清空接收缓冲区
    }

    /**
     * @brief 接收错误中断回调函数
     */
    void USART4::RxError_IrqCallback()
    {
        // 首先从USART中读取数据
        (void)USART_ReadData(m_config.unit);
        // 然后清除USART的错误状态
        USART_ClearStatus(m_config.unit, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
    }

    /**
     * @brief 接收超时中断回调函数
     */
    void USART4::RxTimeout_IrqCallback()
    {
        uint16_t u16DataToSend = ARRAY_LENGTH - (uint16_t)DMA_GetTransCount(m_config.rx_dma_unit, m_config.rx_dma_ch);

        if (u16DataToSend <= ARRAY_LENGTH)
        {
            // 使用任务通知并发送数据
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            // 注意：确保 u16DataToSend 的值不会超过任务通知的最大值
            xTaskNotifyFromISR(UartTask_Handle, u16DataToSend, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

            ReconfigureUartDma();

            TMR0_Stop(m_config.TMR0_UNIT, m_config.TMR0_CH);         // 停止定时器
            USART_ClearStatus(m_config.unit, USART_FLAG_RX_TIMEOUT); // 清除接收超时标志位
        }
    }

    /**
     * @brief 接收DMA传输完成中断回调函数
     */
    void USART4::RX_DMA_TC_IrqCallback()
    {
        BSP_LED_On(LED_GREEN);
        // 使用任务通知并发送数据
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // 注意：确保 u16DataToSend 的值不会超过任务通知的最大值
        xTaskNotifyFromISR(UartTask_Handle, ARRAY_LENGTH, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        USART_FuncCmd(m_config.unit, USART_RX_TIMEOUT, DISABLE);                     // 关闭接收超时功能
        DMA_ClearTransCompleteStatus(m_config.rx_dma_unit, m_config.rx_dma_tc_flag); // 清除传输完成标志位
    }

    /**
     * @brief 发送DMA传输完成中断回调函数
     */
    void USART4::TX_DMA_TC_IrqCallback()
    {
        USART_FuncCmd(m_config.unit, USART_INT_TX_CPLT, ENABLE); // 使能发送完成中断
        DMA_ClearTransCompleteStatus(m_config.tx_dma_unit, m_config.tx_dma_tc_flag);
    }

    /**
     * @brief 初始化USART4
     */
    void USART4::USART4_Init()
    {
        Config USART_CONFIG = {
            .TMR0_UNIT = CM_TMR0_2,
            .TMR0_CH = TMR0_CH_B,
            .USART_TIMEOUT_BITS = 2000U,
            .tx_dma_unit = CM_DMA2,
            .tx_dma_ch = DMA_CH0,
            .tx_dma_trig_sel = AOS_DMA2_0,
            .tx_dma_trig_evt_src = EVT_SRC_USART4_TI,
            .tx_dma_tc_int = DMA_INT_TC_CH0,
            .tx_dma_tc_flag = DMA_FLAG_TC_CH0,
            .tx_dma_tc_irqn = INT001_IRQn,
            .tx_dma_tc_int_src = INT_SRC_DMA2_TC0,
            .rx_dma_unit = CM_DMA1,
            .rx_dma_ch = DMA_CH0,
            .rx_dma_trig_sel = AOS_DMA1_0,
            .rx_dma_trig_evt_src = EVT_SRC_USART4_RI,
            .rx_dma_tc_int = DMA_INT_TC_CH0,
            .rx_dma_tc_flag = DMA_FLAG_TC_CH0,
            .rx_dma_tc_irqn = INT000_IRQn,
            .rx_dma_tc_int_src = INT_SRC_DMA1_TC0,
            .rx_dma_reconf_trig_sel = AOS_DMA_RC,
            .rx_dma_reconf_trig_evt_src = EVT_SRC_AOS_STRG,
            .tx_port = GPIO_PORT_E,
            .tx_pin = GPIO_PIN_03,
            .tx_func = GPIO_FUNC_36,
            .rx_port = GPIO_PORT_E,
            .rx_pin = GPIO_PIN_02,
            .rx_func = GPIO_FUNC_37,
            .baud_rate = 115200U,
            .unit = CM_USART4,
            .tx_buffer = Host_TxBuf,
            .tx_buffer_size = ARRAY_LENGTH,
            .rx_buffer = Host_RxBuf,
            .rx_buffer_size = ARRAY_LENGTH,
            .tx_cplt_irqn = INT002_IRQn,
            .tx_cplt_int_src = INT_SRC_USART4_TCI,
            .rx_err_irqn = INT003_IRQn,
            .rx_err_int_src = INT_SRC_USART4_EI,
            .rx_timeout_irqn = INT004_IRQn,
            .rx_timeout_int_src = INT_SRC_USART4_RTO,
        };

        USART::addUSART(USART_CONFIG);
    }

    /**
     * @brief 发送数据
     * @param data 数据指针
     * @param length 数据长度
     */
    void USART4::USART4_SendData(void *data, size_t length)
    {
        /* 获取USART4实例的索引 */
        if (USART::m_instances.at(USART4_CONFIG).get()->m_config.unit == CM_USART4)
            USART::m_instances.at(USART4_CONFIG).get()->SendData(data, length);
    }
} // namespace BSP_USART
