/**
 * @file bsp_uart4.c
 * @brief 串口4底层驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-11-13
 */

#include "globalConfig.h"

static stc_dma_llp_descriptor_t stcLlpDesc; // DMA链表指针描述符（LLP功能）

/**
 * @brief 串口4接收超时中断回调函数
 */
static void USART4_RxTimeout_IrqCallback(void)
{
    uint16_t u16DataToSend =
        HOST_BUFFER_LENGTH - (uint16_t)DMA_GetTransCount(USART4_RX_DMA_UNIT, USART4_RX_DMA_CH);

    if (u16DataToSend <= HOST_BUFFER_LENGTH) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        BaseType_t xResult;

        // 注意：确保 u16DataToSend 的值不会超过任务通知的最大值
        xResult = xTaskNotifyFromISR(uartRxTaskHandle, u16DataToSend, eSetValueWithOverwrite,
                                     &xHigherPriorityTaskWoken);
        if (xResult != pdFAIL) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        AOS_SW_Trigger(); // 触发 DMA 重配置
        TMR0_Stop(TMR0_UNIT_USART4, TMR0_CH_USART4);
        USART_ClearStatus(USART4_UNIT, USART_FLAG_RX_TIMEOUT);
    }
}

/**
 * @brief 串口4接收DMA传输完成中断回调函数
 */
static void USART4_RX_DMA_TC_IrqCallback(void)
{
    // 使用任务通知并发送数据
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult;

    // 注意：确保 u16DataToSend 的值不会超过任务通知的最大值
    xResult = xTaskNotifyFromISR(uartRxTaskHandle, HOST_BUFFER_LENGTH, eSetValueWithOverwrite,
                                 &xHigherPriorityTaskWoken);
    if (xResult != pdFAIL) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    USART_FuncCmd(USART4_UNIT, USART_RX_TIMEOUT, DISABLE);                   // 关闭接收超时功能
    DMA_ClearTransCompleteStatus(USART4_RX_DMA_UNIT, USART4_RX_DMA_TC_FLAG); // 清除传输完成标志位
}

/**
 * @brief 串口4接收错误中断回调函数
 */
static void USART4_RxError_IrqCallback(void)
{
    // 首先从USART中读取数据
    (void)USART_ReadData(USART4_UNIT);
    // 然后清除USART的错误状态
    USART_ClearStatus(USART4_UNIT,
                      (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}

/**
 * @brief 串口4发送DMA传输完成中断回调函数
 */
static void USART4_TX_DMA_TC_IrqCallback(void)
{
    USART_FuncCmd(USART4_UNIT, USART_INT_TX_CPLT, ENABLE);
    DMA_ClearTransCompleteStatus(USART4_TX_DMA_UNIT, USART4_TX_DMA_TC_FLAG);
}

/**
 * @brief 串口4发送完成中断回调函数
 */
static void USART4_TxComplete_IrqCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* 关闭串口发送功能和发送完成中断 */
    USART_FuncCmd(USART4_UNIT, (USART_TX | USART_INT_TX_CPLT),
                  DISABLE);                                // 关闭串口发送功能和发送完成中断
    USART_ClearStatus(USART4_UNIT, USART_FLAG_TX_CPLT);    // 清除发送完成标志位
    USART_ClearStatus(USART4_UNIT, USART_FLAG_RX_TIMEOUT); // 清除接收超时标志位
    TMR0_Stop(TMR0_UNIT_USART4, TMR0_CH_USART4);           // 停止定时器
    USART_FuncCmd(USART4_UNIT, USART_RX_TIMEOUT, ENABLE);  // 使能接收超时功能
    USART4_485_RX_ENABLE();                                // 使能485接收

    /* 通知发送任务发送完成 */
    vTaskNotifyGiveFromISR(uartTxTaskHandle, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/**
 * @brief 配置TMR0定时器
 * @param u16TimeoutBits 超时时间
 */
static void USART4_TMR0_Config(uint16_t u16TimeoutBits)
{
    uint16_t u16Div;             // 时钟分频系数
    uint16_t u16Delay;           // 延时
    uint16_t u16CompareValue;    // 比较值
    stc_tmr0_init_t stcTmr0Init; // 定时器初始化结构体

    TMR0_USART4_FCG_ENABLE();

    /* Initialize TMR0 base function. */
    stcTmr0Init.u32ClockSrc = TMR0_CLK_SRC_XTAL32; // 使用外部32K晶振
    stcTmr0Init.u32ClockDiv = TMR0_CLK_DIV8;       // 时钟分频系数为8
    stcTmr0Init.u32Func = TMR0_FUNC_CMP;           // 定时器工作在比较模式
    if (TMR0_CLK_DIV1 == stcTmr0Init.u32ClockDiv) {
        u16Delay = 7U;
    } else if (TMR0_CLK_DIV2 == stcTmr0Init.u32ClockDiv) {
        u16Delay = 5U;
    } else if ((TMR0_CLK_DIV4 == stcTmr0Init.u32ClockDiv) ||
               (TMR0_CLK_DIV8 == stcTmr0Init.u32ClockDiv) ||
               (TMR0_CLK_DIV16 == stcTmr0Init.u32ClockDiv)) {
        u16Delay = 3U;
    } else {
        u16Delay = 2U;
    }

    u16Div = (uint16_t)1U << (stcTmr0Init.u32ClockDiv >> TMR0_BCONR_CKDIVA_POS);
    u16CompareValue = ((u16TimeoutBits + u16Div - 1U) / u16Div) - u16Delay;
    stcTmr0Init.u16CompareValue = u16CompareValue;
    (void)TMR0_Init(TMR0_UNIT_USART4, TMR0_CH_USART4, &stcTmr0Init);

    /* 在这里连接的就是USART的TIMEOUT，控制寄存器 1（USART_CR1）的UARTTIMEOUT功能使能位
     * 使能之后就可以自动触发TMR0开始计时了 */
    TMR0_HWStartCondCmd(TMR0_UNIT_USART4, TMR0_CH_USART4, ENABLE);
    TMR0_HWClearCondCmd(TMR0_UNIT_USART4, TMR0_CH_USART4, ENABLE);
}

/**
 * @brief 配置DMA
 * @return int32_t 返回值
 */
static int32_t USART4_DMA_Config(void)
{
    int32_t i32Ret;
    stc_dma_init_t stcDmaInit;
    stc_dma_llp_init_t stcDmaLlpInit;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* DMA&AOS FCG enable */
    USART4_RX_DMA_FCG_ENABLE();
    USART4_TX_DMA_FCG_ENABLE();
    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_AOS, ENABLE);

    /* USART_RX_DMA */
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = ARRAY_SZ(hostReceiveBuffer);
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stcDmaInit.u32DestAddr = (uint32_t)hostReceiveBuffer;
    stcDmaInit.u32SrcAddr = (uint32_t)(&USART4_UNIT->RDR);
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC;
    i32Ret = DMA_Init(USART4_RX_DMA_UNIT, USART4_RX_DMA_CH, &stcDmaInit);
    if (LL_OK == i32Ret) {
        //--------------------------------------------------------------------
        (void)DMA_LlpStructInit(&stcDmaLlpInit);
        stcDmaLlpInit.u32State = DMA_LLP_ENABLE;
        stcDmaLlpInit.u32Mode = DMA_LLP_WAIT;
        stcDmaLlpInit.u32Addr = (uint32_t)&stcLlpDesc;
        (void)DMA_LlpInit(USART4_RX_DMA_UNIT, USART4_RX_DMA_CH, &stcDmaLlpInit);
        //--------------------------------------------------------------------

        stcLlpDesc.SARx =
            stcDmaInit.u32SrcAddr; // LLP源地址。在LLP模式下，DMA从此地址开始读取数据。
        stcLlpDesc.DARx = stcDmaInit.u32DestAddr; // LLP目标地址。在LLP模式下，DMA将数据写入此地址。
        stcLlpDesc.DTCTLx = (stcDmaInit.u32TransCount << DMA_DTCTL_CNT_POS) |
                            (stcDmaInit.u32BlockSize << DMA_DTCTL_BLKSIZE_POS);
        stcLlpDesc.LLPx = (uint32_t)&stcLlpDesc; // LLP下一个链表指针
        stcLlpDesc.CHCTLx = stcDmaInit.u32SrcAddrInc | stcDmaInit.u32DestAddrInc |
                            stcDmaInit.u32DataWidth | stcDmaInit.u32IntEn | stcDmaLlpInit.u32State |
                            stcDmaLlpInit.u32Mode; // LLP通道控制。此字段定义了DMA通道的控制信息。

        DMA_ReconfigLlpCmd(USART4_RX_DMA_UNIT, USART4_RX_DMA_CH, ENABLE);
        DMA_ReconfigCmd(USART4_RX_DMA_UNIT, ENABLE);
        AOS_SetTriggerEventSrc(USART4_RX_DMA_RECONF_TRIG_SEL, USART4_RX_DMA_RECONF_TRIG_EVT_SRC);

        stcIrqSignConfig.enIntSrc = USART4_RX_DMA_TC_INT_SRC;
        stcIrqSignConfig.enIRQn = USART4_RX_DMA_TC_IRQn;
        stcIrqSignConfig.pfnCallback = &USART4_RX_DMA_TC_IrqCallback;
        installInterruptHandler(&stcIrqSignConfig, DDL_IRQ_PRIO_DEFAULT);

        AOS_SetTriggerEventSrc(USART4_RX_DMA_TRIG_SEL, USART4_RX_DMA_TRIG_EVT_SRC);
        DMA_Cmd(USART4_RX_DMA_UNIT, ENABLE);
        DMA_TransCompleteIntCmd(USART4_RX_DMA_UNIT, USART4_RX_DMA_TC_INT, ENABLE);
        (void)DMA_ChCmd(USART4_RX_DMA_UNIT, USART4_RX_DMA_CH, ENABLE);
    }

    /* USART_TX_DMA */
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stcDmaInit.u32DestAddr = (uint32_t)(&USART4_UNIT->TDR);
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_INC;
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_FIX;
    i32Ret = DMA_Init(USART4_TX_DMA_UNIT, USART4_TX_DMA_CH, &stcDmaInit);
    if (LL_OK == i32Ret) {
        stcIrqSignConfig.enIntSrc = USART4_TX_DMA_TC_INT_SRC;
        stcIrqSignConfig.enIRQn = USART4_TX_DMA_TC_IRQn;
        stcIrqSignConfig.pfnCallback = &USART4_TX_DMA_TC_IrqCallback;
        (void)INTC_IrqSignIn(&stcIrqSignConfig);
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
        NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

        AOS_SetTriggerEventSrc(USART4_TX_DMA_TRIG_SEL, USART4_TX_DMA_TRIG_EVT_SRC);

        DMA_Cmd(USART4_TX_DMA_UNIT, ENABLE);
        DMA_TransCompleteIntCmd(USART4_TX_DMA_UNIT, USART4_TX_DMA_TC_INT, ENABLE);
    }

    return i32Ret;
}

/**
 * @brief 配置GPIO
 */
static void USART4_GPIO_Config(void)
{
    stc_gpio_init_t stcGpioInit;

    /* 配置 TX/RX 引脚 */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PullUp = PIN_PU_ON; // 开启内部上拉
    GPIO_Init(USART4_RX_PORT, USART4_RX_PIN, &stcGpioInit);
    GPIO_Init(USART4_TX_PORT, USART4_TX_PIN, &stcGpioInit);
    /* 复用为串口发送和接收 */
    GPIO_SetFunc(USART4_RX_PORT, USART4_RX_PIN, USART4_RX_GPIO_FUNC);
    GPIO_SetFunc(USART4_TX_PORT, USART4_TX_PIN, USART4_TX_GPIO_FUNC);

    /* 配置 485 控制引脚 */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;  // 设置为输出
    stcGpioInit.u16PinDrv = PIN_HIGH_DRV; // 高驱动能力
    GPIO_Init(USART4_485_EN_PORT, USART4_485_EN_PIN, &stcGpioInit);
}

static void USART4_IRQ_Config(void)
{
    stc_irq_signin_config_t stcIrqSigninConfig;

    /* 注册 USART 发送完成中断处理函数 */
    stcIrqSigninConfig.enIRQn = USART4_TX_CPLT_IRQn;
    stcIrqSigninConfig.enIntSrc = USART4_TX_CPLT_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART4_TxComplete_IrqCallback;
    installInterruptHandler(&stcIrqSigninConfig, DDL_IRQ_PRIO_DEFAULT);

    /* 注册 USART 接收错误中断处理函数 */
    // USART接收错误的中断。当USART模块在接收数据时遇到错误（比如奇偶校验错误、帧错误、噪声错误或者溢出）时，这个中断会被触发
    stcIrqSigninConfig.enIRQn = USART4_RX_ERR_IRQn;
    stcIrqSigninConfig.enIntSrc = USART4_RX_ERR_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART4_RxError_IrqCallback;
    installInterruptHandler(&stcIrqSigninConfig, DDL_IRQ_PRIO_DEFAULT);

    /* 注册 USART 接收超时中断处理函数 */
    // 这是USART接收超时的中断。当USART接收到一个字节的数据后，如果在超时时间内没有接收到下一个字节的数据，这个中断会被触发
    stcIrqSigninConfig.enIRQn = USART4_RX_TIMEOUT_IRQn;
    stcIrqSigninConfig.enIntSrc = USART4_RX_TIMEOUT_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART4_RxTimeout_IrqCallback;
    installInterruptHandler(&stcIrqSigninConfig, DDL_IRQ_PRIO_DEFAULT);
}

/**
 * @brief 初始化串口4
 */
void USART4_Init(void)
{
    stc_usart_uart_init_t stcUartInit;

    USART4_DMA_Config();
    USART4_TMR0_Config(2000u);
    USART4_GPIO_Config();
    USART4_FCG_ENABLE();

    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockDiv = USART_CLK_DIV64;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_ENABLE;
    stcUartInit.u32Baudrate = USART4_BAUDRATE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
    USART_UART_Init(USART4_UNIT, &stcUartInit, NULL);

    USART4_IRQ_Config();
    USART_FuncCmd(USART4_UNIT, (USART_RX | USART_INT_RX | USART_RX_TIMEOUT | USART_INT_RX_TIMEOUT),
                  ENABLE);
    USART4_485_RX_ENABLE();
}

void USART4_SendData(void *data, size_t length)
{
    uint8_t *TxBuf = (uint8_t *)data;

    DMA_SetSrcAddr(USART4_TX_DMA_UNIT, USART4_TX_DMA_CH, (uint32_t)TxBuf); // 设置源地址
    DMA_SetTransCount(USART4_TX_DMA_UNIT, USART4_TX_DMA_CH, length);       // 设置传输数据长度
    (void)DMA_ChCmd(USART4_TX_DMA_UNIT, USART4_TX_DMA_CH, ENABLE);         // 使能通道
    USART_FuncCmd(USART4_UNIT, USART_TX, ENABLE);
    USART4_485_TX_ENABLE();
}
