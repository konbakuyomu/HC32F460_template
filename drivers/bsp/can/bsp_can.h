/**
 * @file drv_can.h
 * @brief CAN驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-10-24
 */

#pragma once

#ifdef __cplusplus
    #include "drv_hal.h"
    #include "hc32_ll.h"
    #include <algorithm>
    #include <functional>
    #include <iostream>
    #include <memory>
    #include <type_traits>
    #include <vector>

/* 类定义 ------------------------------------------------------------------*/
namespace BSP_CAN {

    enum class CANBaudRate {
        BAUD_250K,
        BAUD_500K,
        BAUD_1M
    };

    struct Config {
        /* CAN 基础配置 */
        CM_CAN_TypeDef *CAN_UNIT;  // CAN单元
        uint32_t CAN_FCG_CLK;      // CAN时钟
        uint8_t CAN_TX_PORT;       // CAN发送端口
        uint8_t CAN_RX_PORT;       // CAN接收端口
        uint8_t CAN_PHY_STBY_PORT; // CAN物理层使能端口
        uint16_t CAN_TX_PIN;       // CAN发送引脚
        uint16_t CAN_RX_PIN;       // CAN接收引脚
        uint16_t CAN_PHY_STBY_PIN; // CAN物理层使能引脚
        uint32_t CAN_TX_PIN_FUNC;  // CAN发送引脚功能
        uint32_t CAN_RX_PIN_FUNC;  // CAN接收引脚功能
        uint8_t CAN_TX_DLC;        // CAN发送数据长度
        uint32_t CAN_RX_FRAME_NUM; // CAN接收帧数
        CANBaudRate baudRate;      // CAN波特率

        /* CAN 中断配置 */
        en_int_src_t CAN_INT_SRC; // CAN中断源
        IRQn_Type CAN_INT_IRQn;   // CAN中断IRQn
        uint32_t CAN_INT_SEL;     // CAN中断选择

        /* CAN 过滤器配置 */
        uint16_t CAN_FILTER_SEL;      // CAN过滤器选择
        uint8_t CAN_FILTER_NUM;       // CAN过滤器数量
        uint32_t CAN_FILTER1_ID;      // 过滤器1 ID
        uint32_t CAN_FILTER1_ID_MASK; // 过滤器1 ID 掩码
        uint8_t CAN_FILTER1_ID_TYPE;  // 过滤器1 ID 类型
        uint32_t CAN_FILTER2_ID;      // 过滤器2 ID
        uint32_t CAN_FILTER2_ID_MASK; // 过滤器2 ID 掩码
        uint8_t CAN_FILTER2_ID_TYPE;  // 过滤器2 ID 类型
        uint32_t CAN_FILTER3_ID;      // 过滤器3 ID
        uint32_t CAN_FILTER3_ID_MASK; // 过滤器3 ID 掩码
        uint8_t CAN_FILTER3_ID_TYPE;  // 过滤器3 ID 类型
        uint32_t CAN_FILTER4_ID;      // 过滤器4 ID
        uint32_t CAN_FILTER4_ID_MASK; // 过滤器4 ID 掩码
        uint8_t CAN_FILTER4_ID_TYPE;  // 过滤器4 ID 类型
        uint32_t CAN_FILTER5_ID;      // 过滤器5 ID
        uint32_t CAN_FILTER5_ID_MASK; // 过滤器5 ID 掩码
        uint8_t CAN_FILTER5_ID_TYPE;  // 过滤器5 ID 类型
        uint32_t CAN_FILTER6_ID;      // 过滤器6 ID
        uint32_t CAN_FILTER6_ID_MASK; // 过滤器6 ID 掩码
        uint8_t CAN_FILTER6_ID_TYPE;  // 过滤器6 ID 类型
        uint32_t CAN_FILTER7_ID;      // 过滤器7 ID
        uint32_t CAN_FILTER7_ID_MASK; // 过滤器7 ID 掩码
        uint8_t CAN_FILTER7_ID_TYPE;  // 过滤器7 ID 类型
        uint32_t CAN_FILTER8_ID;      // 过滤器8 ID
        uint32_t CAN_FILTER8_ID_MASK; // 过滤器8 ID 掩码
        uint8_t CAN_FILTER8_ID_TYPE;  // 过滤器8 ID 类型
    };

    class CAN
    {
      public:
        Config m_config;
        static std::unique_ptr<CAN> m_instance;
        static std::vector<stc_can_filter_config_t> astcFilter;

      public:
        CAN(Config config) : m_config(config)
        {
            CanPinConfig();
            CanInitConfig();
            CanIrqConfig();
            CanPhyEnable();
        }

        /**
         * @brief 默认构造函数
         */
        CAN(void) = default;

        static void _bsp_can_init();
        static void SendData(void *value);

      public:
        void CanPinConfig();
        void CanInitConfig();
        void CanIrqConfig();
        void CanPhyEnable();
        void initFilters();
        void ProcessReceivedData();

      private:
        static constexpr int MAX_FILTERS = 8;
        static stc_can_rx_frame_t *m_astRxFrame;

      private:
        void CAN_IrqCallback();
        int CAN_InitReceiveBuffer();
        void ProcessCANFrame(stc_can_rx_frame_t Rx_Data);
    };
} // namespace BSP_CAN
#endif
