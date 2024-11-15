/**
 * @file  printf_usart.c
 * @brief 打印串口实现
 * @details 包含打印串口的具体实现
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "globalConfig.h"

/**
 * @brief GPIO配置函数
 *
 * 将引脚复用为RX和TX，并配置控制脚
 */
static void GPIO_SetFunction(void)
{
    /* Configure USART RX/TX pin. */
    GPIO_SetFunc(USART_RX_PORT, USART_RX_PIN, USART_RX_GPIO_FUNC);
    GPIO_SetFunc(USART_TX_PORT, USART_TX_PIN, USART_TX_GPIO_FUNC);
}

/**
 * @brief USART配置函数
 *
 * @param vpDevice USART设备指针
 * @param u32Param USART参数（通常是波特率）
 * @return int32_t 配置结果，LL_OK表示成功
 */
int32_t usartConfig(void *vpDevice, uint32_t u32Param)
{
    stc_usart_uart_init_t stcUartInit;
    CM_USART_TypeDef *USARTx = (CM_USART_TypeDef *)vpDevice;

    /* 将USART2引脚复用为RX和TX,以及配置控制脚 */
    GPIO_SetFunction();
    /* 使能串口时钟 */
    USART_FCG_ENABLE();

    /* 初始化 UART. */
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockDiv = USART_CLK_DIV64;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_ENABLE;
    stcUartInit.u32Baudrate = u32Param;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
    if (LL_OK != USART_UART_Init(USARTx, &stcUartInit, NULL)) {
        for (;;) {
        }
    }

    USART_FuncCmd(USARTx, (USART_RX | USART_TX), ENABLE);

    return LL_OK;
}
