/**
 * @file bsp_can.c
 * @brief CAN底层驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-10-25
 */

#include "globalConfig.h"

/**
 * @brief CAN错误类型字符串
 */
static const char *m_s8ErrorTypeStr[] = {
    "NO error.",  "Bit Error.", "Form Error.",  "Stuff Error.",
    "ACK Error.", "CRC Error.", "Other Error.", "Error type is NOT defined.",
};

/**
 * @brief CAN引脚复用
 * @param 无
 * @return 无
 */
static void CanPinConfig(void)
{
    GPIO_SetFunc(CAN_TX_PORT, CAN_TX_PIN, CAN_TX_PIN_FUNC);
    GPIO_SetFunc(CAN_RX_PORT, CAN_RX_PIN, CAN_RX_PIN_FUNC);
}

/**
 * @brief CAN初始化配置
 * @param 无
 * @return 无
 */
static void CanInitConfig(void)
{
    stc_can_init_t stcCanInit;
    /**
     *CAN_FILTERx_ID：过滤器x的ID值，x的取值范围是1~28。
     *CAN_FILTERx_ID_MASK：过滤器x的ID掩码，用于指定哪些位需要匹配，哪些位不需要匹配。
     *CAN_FILTERx_ID_TYPE：过滤器x的ID类型，指定是标准帧还是扩展帧。
     **/
    stc_can_filter_config_t astcFilter = {CAN_ID_QUERY_REALTIME_DATA, CAN_MASK_QUERY_REALTIME_DATA,
                                          CAN_ID_TYPE_QUERY_REALTIME_DATA};

    /* CAN 波特率设定，这里时钟来源是外部晶振 8MHz ，最后波特率 = 8M/(SEG1+SEG2)*PRESC */
    /* PRESC:2  SEG1:6  SEG2:2 SJW:2  --->  500K */
    /* PRESC:2  SEG1:10  SEG2:4 SJW:3  --->  250K */
    /* PRESC:2  SEG1:3  SEG2:1 SJW:1  --->  1M */
    (void)CAN_StructInit(&stcCanInit);
    stcCanInit.stcBitCfg.u32Prescaler = 2U;
    stcCanInit.stcBitCfg.u32TimeSeg1 = 6U;
    stcCanInit.stcBitCfg.u32TimeSeg2 = 2U;
    stcCanInit.stcBitCfg.u32SJW = 2U;
    stcCanInit.pstcFilter = &astcFilter;         // 这里先自己配置任意数量的过滤器
    stcCanInit.u16FilterSelect = CAN_FILTER_SEL; // 然后在这里具体选择要哪些过滤器
    stcCanInit.u8WorkMode = CAN_WORK_MD_NORMAL;  // 正常工作模式

    /* 使能CAN时钟 */
    FCG_Fcg1PeriphClockCmd(CAN_PERIPH_CLK, ENABLE); // 初始化CAN时钟
    (void)CAN_Init(CAN_UNIT, &stcCanInit);
    /* 失能所有的中断，然后再使能选择的中断 */
    CAN_IntCmd(CAN_UNIT, CAN_INT_ALL, DISABLE);
    CAN_IntCmd(CAN_UNIT, CAN_INT_SEL, ENABLE);
}

/**
 * @brief CAN中断回调函数
 * @param 无
 * @return 无
 */
static void CAN_IrqCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    stc_can_error_info_t stcErr;

    /* 获取CAN错误信息 */
    (void)CAN_GetErrorInfo(CAN_UNIT, &stcErr);
    if (stcErr.u8ErrorType != 0) {
        DDL_Printf("---> CAN error type: %u, %s\r\n", stcErr.u8ErrorType,
                   m_s8ErrorTypeStr[stcErr.u8ErrorType]);
    }

    /* CAN接收缓冲区警告 */
    if (CAN_GetStatus(CAN_UNIT, CAN_FLAG_RX_BUF_WARN) == SET) {
        // 可以加入接收缓冲区警告处理
        CAN_ClearStatus(CAN_UNIT, CAN_FLAG_RX_BUF_WARN);
        CAN_IntCmd(CAN_UNIT, CAN_INT_RX, DISABLE);
    }

    /* CAN接收缓冲区满 */
    if (CAN_GetStatus(CAN_UNIT, CAN_FLAG_RX_BUF_FULL) == SET) {
        // 可以加入接收缓冲区满处理
        CAN_ClearStatus(CAN_UNIT, CAN_FLAG_RX_BUF_FULL);
        CAN_IntCmd(CAN_UNIT, CAN_INT_RX, DISABLE);
    }

    /* CAN接收溢出 */
    if (CAN_GetStatus(CAN_UNIT, CAN_FLAG_RX_OVERRUN) == SET) {
        // 可以加入接收溢出处理
        CAN_ClearStatus(CAN_UNIT, CAN_FLAG_RX_OVERRUN);
        CAN_IntCmd(CAN_UNIT, CAN_INT_RX, DISABLE);
    }

    /* CAN发送完成中断 */
    if (CAN_GetStatus(CAN_UNIT, CAN_FLAG_PTB_TX) == SET) {
        CAN_ClearStatus(CAN_UNIT, CAN_FLAG_PTB_TX);
        vTaskNotifyGiveFromISR(canTxTaskHandle, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken == pdTRUE) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        CAN_IntCmd(CAN_UNIT, CAN_INT_PTB_TX, DISABLE);
    }

    /* CAN接收中断 */
    if (CAN_GetStatus(CAN_UNIT, CAN_FLAG_RX) == SET) {
        CAN_ClearStatus(CAN_UNIT, CAN_FLAG_RX);
        vTaskNotifyGiveFromISR(canRxTaskHandle, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken == pdTRUE) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        CAN_IntCmd(CAN_UNIT, CAN_INT_RX, DISABLE);
    }

    /* 清除所有CAN错误标志 */
    CAN_ClearStatus(CAN_UNIT, CAN_FLAG_CLR_ALL);
}

/**
 * @brief CAN中断配置
 * @param 无
 * @return 无
 */
static void CanIrqConfig(void)
{
    stc_irq_signin_config_t stcIrq;

    stcIrq.enIntSrc = CAN_INT_SRC;
    stcIrq.enIRQn = CAN_INT_IRQn;
    stcIrq.pfnCallback = &CAN_IrqCallback;
    installInterruptHandler(&stcIrq, CAN_INT_PRIO);
}

/**
 * @brief CAN初始化
 * @param 无
 * @return 无
 */
void initBspCanImpl(void)
{
    CanPinConfig();
    CanInitConfig();
    CanIrqConfig();
}
