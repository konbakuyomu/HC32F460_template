/**
 * @file drv_can.cpp
 * @brief CAN驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-10-25
 */

#include "variable.h"

namespace BSP_CAN {
    stc_can_rx_frame_t *CAN::m_astRxFrame = NULL;
    std::unique_ptr<CAN> CAN::m_instance;
    std::vector<stc_can_filter_config_t> CAN::astcFilter;

    /**
     * @brief 初始化CAN
     */
    void CAN::_bsp_can_init()
    {
        Config config = {
            .CAN_UNIT = CM_CAN,
            .CAN_FCG_CLK = FCG1_PERIPH_CAN,
            .CAN_TX_PORT = GPIO_PORT_B,
            .CAN_RX_PORT = GPIO_PORT_B,
            .CAN_PHY_STBY_PORT = GPIO_PORT_D,
            .CAN_TX_PIN = GPIO_PIN_07,
            .CAN_RX_PIN = GPIO_PIN_06,
            .CAN_PHY_STBY_PIN = GPIO_PIN_15,
            .CAN_TX_PIN_FUNC = GPIO_FUNC_50,
            .CAN_RX_PIN_FUNC = GPIO_FUNC_51,
            .CAN_TX_DLC = CAN_DLC8,
            .CAN_RX_FRAME_NUM = 50,
            .baudRate = CANBaudRate::BAUD_250K,
            .CAN_INT_SRC = INT_SRC_CAN_INT,
            .CAN_INT_IRQn = INT122_IRQn,
            .CAN_INT_SEL = CAN_INT_PTB_TX | CAN_INT_RX,
            .CAN_FILTER_SEL = CAN_FILTER1,
            .CAN_FILTER_NUM = 1,
            .CAN_FILTER1_ID = 0x0000DD00UL,
            .CAN_FILTER1_ID_MASK = 0x000000FFUL,
            .CAN_FILTER1_ID_TYPE = CAN_ID_STD_EXT};

        CAN::m_instance = std::make_unique<CAN>(config);
    }

    /**
     * @brief 配置CAN的引脚
     */
    void CAN::CanPinConfig()
    {
        GPIO_SetFunc(m_config.CAN_TX_PORT, m_config.CAN_TX_PIN, m_config.CAN_TX_PIN_FUNC);
        GPIO_SetFunc(m_config.CAN_RX_PORT, m_config.CAN_RX_PIN, m_config.CAN_RX_PIN_FUNC);
    }

    /**
     * @brief 初始化过滤器
     */
    void CAN::initFilters()
    {
        static const std::array<uint32_t, MAX_FILTERS> filterIds = {
            m_config.CAN_FILTER1_ID,
            m_config.CAN_FILTER2_ID,
            m_config.CAN_FILTER3_ID,
            m_config.CAN_FILTER4_ID,
            m_config.CAN_FILTER5_ID,
            m_config.CAN_FILTER6_ID,
            m_config.CAN_FILTER7_ID,
            m_config.CAN_FILTER8_ID};
        static const std::array<uint32_t, MAX_FILTERS> filterMasks = {
            m_config.CAN_FILTER1_ID_MASK,
            m_config.CAN_FILTER2_ID_MASK,
            m_config.CAN_FILTER3_ID_MASK,
            m_config.CAN_FILTER4_ID_MASK,
            m_config.CAN_FILTER5_ID_MASK,
            m_config.CAN_FILTER6_ID_MASK,
            m_config.CAN_FILTER7_ID_MASK,
            m_config.CAN_FILTER8_ID_MASK};
        static const std::array<uint32_t, MAX_FILTERS> filterTypes = {
            m_config.CAN_FILTER1_ID_TYPE,
            m_config.CAN_FILTER2_ID_TYPE,
            m_config.CAN_FILTER3_ID_TYPE,
            m_config.CAN_FILTER4_ID_TYPE,
            m_config.CAN_FILTER5_ID_TYPE,
            m_config.CAN_FILTER6_ID_TYPE,
            m_config.CAN_FILTER7_ID_TYPE,
            m_config.CAN_FILTER8_ID_TYPE};

        astcFilter.reserve(m_config.CAN_FILTER_NUM);
        for (int i = 0; i < m_config.CAN_FILTER_NUM && i < MAX_FILTERS; ++i)
        {
            astcFilter.push_back({filterIds[i], filterMasks[i], filterTypes[i]});
        }
    }

    /**
     * @brief 初始化CAN配置
     */
    void CAN::CanInitConfig()
    {
        stc_can_init_t stcCanInit;

        // 初始化过滤器
        initFilters();

        // CAN 波特率设定，这里时钟来源是外部晶振 8MHz ，最后波特率 = 8M/(SEG1+SEG2)*PRESC
        /* PRESC:2  SEG1:6  SEG2:2 SJW:2  --->  500K */
        /* PRESC:2  SEG1:12  SEG2:4 SJW:3  --->  250K */
        /* PRESC:2  SEG1:3  SEG2:1 SJW:1  --->  1M */
        // 默认波特率为 250K
        (void)CAN_StructInit(&stcCanInit);
        switch (m_config.baudRate)
        {
        case CANBaudRate::BAUD_250K:
            stcCanInit.stcBitCfg.u32Prescaler = 2U;
            stcCanInit.stcBitCfg.u32TimeSeg1 = 12U;
            stcCanInit.stcBitCfg.u32TimeSeg2 = 4U;
            stcCanInit.stcBitCfg.u32SJW = 3U;
            break;
        case CANBaudRate::BAUD_500K:
            stcCanInit.stcBitCfg.u32Prescaler = 2U;
            stcCanInit.stcBitCfg.u32TimeSeg1 = 6U;
            stcCanInit.stcBitCfg.u32TimeSeg2 = 2U;
            stcCanInit.stcBitCfg.u32SJW = 2U;
            break;
        case CANBaudRate::BAUD_1M:
            stcCanInit.stcBitCfg.u32Prescaler = 2U;
            stcCanInit.stcBitCfg.u32TimeSeg1 = 3U;
            stcCanInit.stcBitCfg.u32TimeSeg2 = 1U;
            stcCanInit.stcBitCfg.u32SJW = 1U;
            break;
        default:
            stcCanInit.stcBitCfg.u32Prescaler = 2U;
            stcCanInit.stcBitCfg.u32TimeSeg1 = 12U;
            stcCanInit.stcBitCfg.u32TimeSeg2 = 4U;
            stcCanInit.stcBitCfg.u32SJW = 3U;
            break;
        }

        stcCanInit.pstcFilter = astcFilter.data();            // 使用 vector 的 data() 方法获取指针
        stcCanInit.u16FilterSelect = m_config.CAN_FILTER_SEL; // 选择要使用的过滤器
        stcCanInit.u8WorkMode = CAN_WORK_MD_NORMAL;           // 正常工作模式

        // 初始化使能CAN时钟
        FCG_Fcg1PeriphClockCmd(m_config.CAN_FCG_CLK, ENABLE);
        // 初始化CAN
        (void)CAN_Init(m_config.CAN_UNIT, &stcCanInit);
        // 禁用所有的CAN中断
        CAN_IntCmd(m_config.CAN_UNIT, CAN_INT_ALL, DISABLE);
        // 启用所需要的CAN中断
        CAN_IntCmd(m_config.CAN_UNIT, m_config.CAN_INT_SEL, ENABLE);
    }

    /**
     * @brief 配置CAN中断
     */
    void CAN::CanIrqConfig()
    {
        stc_irq_signin_config_t stcIrq;

        // 配置中断
        stcIrq.enIntSrc = m_config.CAN_INT_SRC;                             // 设置中断源
        stcIrq.enIRQn = m_config.CAN_INT_IRQn;                              // 设置中断号
        stcIrq.pfnCallback = []() { m_instance.get()->CAN_IrqCallback(); }; // 设置回调函数
        INTC_IrqInstalHandler(&stcIrq, DDL_IRQ_PRIO_DEFAULT);               // 注册中断
    }

    /**
     * @brief 启用CAN物理层
     */
    void CAN::CanPhyEnable()
    {
        GPIO_ResetPins(m_config.CAN_PHY_STBY_PORT, m_config.CAN_PHY_STBY_PIN);
        GPIO_OutputCmd(m_config.CAN_PHY_STBY_PORT, m_config.CAN_PHY_STBY_PIN, ENABLE);
    }

    /**
     * @brief CAN中断回调函数
     */
    void CAN::CAN_IrqCallback()
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        if (CAN_GetStatus(m_config.CAN_UNIT, CAN_FLAG_PTB_TX) == SET)
        {
            CAN_ClearStatus(m_config.CAN_UNIT, CAN_FLAG_PTB_TX);
            vTaskNotifyGiveFromISR(CANTXTask_Handle, &xHigherPriorityTaskWoken);
            CAN_IntCmd(m_config.CAN_UNIT, CAN_INT_PTB_TX, DISABLE);
        }

        if (CAN_GetStatus(m_config.CAN_UNIT, CAN_FLAG_RX) == SET)
        {
            CAN_ClearStatus(m_config.CAN_UNIT, CAN_FLAG_RX);
            vTaskNotifyGiveFromISR(CANRXTask_Handle, &xHigherPriorityTaskWoken);
            CAN_IntCmd(m_config.CAN_UNIT, CAN_INT_RX, DISABLE);
        }

        CAN_ClearStatus(m_config.CAN_UNIT, CAN_FLAG_ALL);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    /**
     * @brief 初始化接收缓冲区
     * @return 0 成功，-1 失败
     */
    int CAN::CAN_InitReceiveBuffer()
    {
        if (m_astRxFrame == NULL)
        {
            m_astRxFrame = (stc_can_rx_frame_t *)mymalloc(sizeof(stc_can_rx_frame_t) * m_config.CAN_RX_FRAME_NUM);
            if (m_astRxFrame == NULL)
            {
                // 内存分配失败，处理错误
                return -1;
            }
        }
        return 0;
    }

    /**
     * @brief 处理接收到的数据
     */
    void CAN::ProcessReceivedData()
    {
        uint8_t u8RxFrameNum = 0U;
        int32_t i32Ret;

        // 确保缓冲区已初始化
        if (m_astRxFrame == NULL)
        {
            if (CAN_InitReceiveBuffer() != 0)
            {
                // 初始化失败，处理错误
                return;
            }
        }

        // 清空接收缓冲区
        std::fill(m_astRxFrame, m_astRxFrame + m_config.CAN_RX_FRAME_NUM, stc_can_rx_frame_t{});

        // 从接收缓冲区获取接收到的数据帧
        do
        {
            i32Ret = CAN_GetRxFrame(m_config.CAN_UNIT, &m_astRxFrame[u8RxFrameNum]);
            /* 如果输入的数据帧结构指针是NULL，函数将返回错误代码LL_ERR_INVD_PARAM */
            if (i32Ret == LL_OK)
            {
                u8RxFrameNum++;
                /* 如果接收缓冲区已满，将从第一个数据帧开始覆盖 */
                if (u8RxFrameNum >= m_config.CAN_RX_FRAME_NUM)
                {
                    u8RxFrameNum = 0;
                }
            }
        } while (i32Ret == LL_OK); // 如果接收缓冲区有数据，就一直获取

        /* 处理接收到的帧. */
        uint8_t i = 0UL;
        while (i < u8RxFrameNum)
        {
            // 收到8字节即为正常收发数据
            if (m_astRxFrame[i].DLC == m_config.CAN_TX_DLC)
            {
                ProcessCANFrame(m_astRxFrame[i]);
            }
            i++;
        }
        CAN_IntCmd(m_config.CAN_UNIT, CAN_INT_RX, ENABLE);
    }

    /**
     * @brief 处理接收到的数据帧
     * @param Rx_Data 接收到的数据帧
     */
    void CAN::ProcessCANFrame(stc_can_rx_frame_t Rx_Data)
    {
        BSP_LED_On(LED_RED);
    }

    /**
     * @brief 发送数据
     * @param value 发送的数据
     */
    void CAN::SendData(void *value)
    {
        stc_can_tx_frame_t stcTx;
        uint32_t u32ExtendedID = 0xDD01;

        for (uint8_t i = 0; i < CAN_DLC8; i++)
        {
            stcTx.au8Data[i] = *((uint8_t *)value);
        }

        stcTx.u32Ctrl = 0x0UL;                           //  初始化CAN发送帧的控制域
        stcTx.IDE = 1UL;                                 //  CAN发送帧的ID类型, 1: 扩展帧
        stcTx.u32ID = u32ExtendedID;                     //  CAN发送帧的ID
        stcTx.DLC = CAN_DLC8;                            //  CAN发送帧的数据长度
        xQueueSend(xQueue_CanTx, &stcTx, portMAX_DELAY); //  将发送帧加入到发送队列
    }
} // namespace BSP_CAN
