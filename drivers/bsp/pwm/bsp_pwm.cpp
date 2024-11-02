/**
 * @file bsp_pwm.cpp
 * @brief 电机驱动PWM初始化
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-10-11
 */

#include "variable.h"

namespace BSP_PWM {

    std::vector<std::unique_ptr<bsp_pwm>> bsp_pwm::m_pwms;

    /* PWM 类 ------------------------------------------------------------------*/
    /**
     * @brief 创建PWM实例
     * @param config PWM配置
     */
    bsp_pwm::bsp_pwm(const PWMConfig &config)
        : m_config(config), m_is_running(false)
    {
        stc_tmra_init_t stcTmraInit;
        stc_tmra_pwm_init_t stcPwmInit;

        // 使能Timer时钟
        FCG_Fcg2PeriphClockCmd(m_config.tmra_clk, ENABLE);

        // 计算周期值
        uint32_t PeriodValue = HCLK1_FREQ / m_config.frequency;

        // 为stcTmraInit设置默认的初始化值并进行配置
        (void)TMRA_StructInit(&stcTmraInit);
        stcTmraInit.sw_count.u8CountMode = TMRA_MD_SAWTOOTH; // 设置为锯齿波模式
        stcTmraInit.sw_count.u8CountDir = TMRA_DIR_UP;       // 设置为向上计数
        stcTmraInit.u32PeriodValue = PeriodValue;            // 设置周期
        (void)TMRA_Init(m_config.tmra_unit, &stcTmraInit);

        // PWM占空比配置
        (void)TMRA_PWM_StructInit(&stcPwmInit);
        stcPwmInit.u32CompareValue = m_config.duty_cycle * PeriodValue;
        GPIO_SetFunc(m_config.tmra_port, m_config.tmra_pin, m_config.tmra_func);
        (void)TMRA_PWM_Init(m_config.tmra_unit, m_config.tmra_channel, &stcPwmInit);

        // PWM方向引脚初始化
        stc_gpio_init_t Dir_GPIO_Init;
        (void)GPIO_StructInit(&Dir_GPIO_Init);
        Dir_GPIO_Init.u16PinState = (m_config.tmra_dir_direction == PWM_DIR_FORWARD) ? PIN_SET : PIN_RESET; // 设置为高电平
        Dir_GPIO_Init.u16PinDir = PIN_DIR_OUT;                                                              // 设置为输出
        (void)GPIO_Init(m_config.tmra_dir_port, m_config.tmra_dir_pin, &Dir_GPIO_Init);

        // 启动TimerA
        TMRA_Start(m_config.tmra_unit);
    }

    /**
     * @brief 初始化PWM
     */
    void bsp_pwm::_bsp_pwm_init()
    {
        // 初始化PWM1
        bsp_pwm::PWMConfig config1 = {
            .tmra_unit = CM_TMRA_1,
            .tmra_clk = FCG2_PERIPH_TMRA_1,
            .tmra_channel = TMRA_CH1,
            .tmra_port = GPIO_PORT_E,
            .tmra_pin = GPIO_PIN_09,
            .tmra_func = GPIO_FUNC_4,
            .tmra_dir_port = GPIO_PORT_E,
            .tmra_dir_pin = GPIO_PIN_10,
            .tmra_dir_direction = PWM_DIR_FORWARD,
            .frequency = 18000U,
            .duty_cycle = 0.5f};

        bsp_pwm::_bsp_add_pwm(config1);

        // 初始化PWM2
        bsp_pwm::PWMConfig config2 = {
            .tmra_unit = CM_TMRA_3,
            .tmra_clk = FCG2_PERIPH_TMRA_3,
            .tmra_channel = TMRA_CH1,
            .tmra_port = GPIO_PORT_A,
            .tmra_pin = GPIO_PIN_06,
            .tmra_func = GPIO_FUNC_5,
            .tmra_dir_port = GPIO_PORT_A,
            .tmra_dir_pin = GPIO_PIN_07,
            .tmra_dir_direction = PWM_DIR_FORWARD,
            .frequency = 18000U,
            .duty_cycle = 0.5f};

        bsp_pwm::_bsp_add_pwm(config2);

        // 初始化PWM3
        bsp_pwm::PWMConfig config3 = {
            .tmra_unit = CM_TMRA_2,
            .tmra_clk = FCG2_PERIPH_TMRA_2,
            .tmra_channel = TMRA_CH1,
            .tmra_port = GPIO_PORT_A,
            .tmra_pin = GPIO_PIN_05,
            .tmra_func = GPIO_FUNC_4,
            .tmra_dir_port = GPIO_PORT_A,
            .tmra_dir_pin = GPIO_PIN_04,
            .tmra_dir_direction = PWM_DIR_FORWARD,
            .frequency = 18000U,
            .duty_cycle = 0.5f};

        bsp_pwm::_bsp_add_pwm(config3);

        // 初始化PWM4
        bsp_pwm::PWMConfig config4 = {
            .tmra_unit = CM_TMRA_5,
            .tmra_clk = FCG2_PERIPH_TMRA_5,
            .tmra_channel = TMRA_CH1,
            .tmra_port = GPIO_PORT_A,
            .tmra_pin = GPIO_PIN_02,
            .tmra_func = GPIO_FUNC_5,
            .tmra_dir_port = GPIO_PORT_A,
            .tmra_dir_pin = GPIO_PIN_01,
            .tmra_dir_direction = PWM_DIR_FORWARD,
            .frequency = 18000U,
            .duty_cycle = 0.5f};

        bsp_pwm::_bsp_add_pwm(config4);
    }

    /**
     * @brief 销毁PWM实例
     */
    bsp_pwm::~bsp_pwm()
    {
        if (m_is_running)
        {
            _bsp_pwm_stop();
        }
        // 清理PWM硬件
    }

    /**
     * @brief 启动PWM
     */
    void bsp_pwm::_bsp_pwm_start()
    {
        if (!m_is_running)
        {
            // 启动PWM输出
            m_is_running = true;
            TMRA_PWM_OutputCmd(m_config.tmra_unit, m_config.tmra_channel, ENABLE);
        }
    }

    /**
     * @brief 停止PWM
     */
    void bsp_pwm::_bsp_pwm_stop()
    {
        if (m_is_running)
        {
            // 停止PWM输出
            m_is_running = false;
            TMRA_PWM_OutputCmd(m_config.tmra_unit, m_config.tmra_channel, DISABLE);
        }
    }

    /**
     * @brief 设置PWM占空比
     * @param duty_cycle 占空比
     */
    void bsp_pwm::_bsp_pwm_set_duty_cycle(float duty_cycle)
    {
        m_config.duty_cycle = duty_cycle;
        // 更新PWM硬件设置
        TMRA_Update_PWM_DutyCycle(m_config.tmra_unit, m_config.frequency, m_config.tmra_channel, m_config.duty_cycle);
    }

    /**
     * @brief 设置PWM频率
     * @param frequency 频率
     */
    void bsp_pwm::_bsp_pwm_set_frequency(uint32_t frequency)
    {
        m_config.frequency = frequency;
        // 更新PWM硬件设置
        TMRA_SetPeriodValue(m_config.tmra_unit, HCLK1_FREQ / m_config.frequency);
    }

    /**
     * @brief 获取当前PWM方向
     * @return 当前方向（PWM_DIR_FORWARD 或 PWM_DIR_REVERSE）
     */
    uint8_t bsp_pwm::_bsp_pwm_get_direction()
    {
        return m_config.tmra_dir_direction;
    }

    /**
     * @brief 设置PWM方向
     * @param direction 方向
     */
    void bsp_pwm::_bsp_pwm_set_direction(uint8_t direction)
    {
        m_config.tmra_dir_direction = direction;
        (direction == PWM_DIR_FORWARD) ? GPIO_SetPins(m_config.tmra_dir_port, m_config.tmra_dir_pin) : GPIO_ResetPins(m_config.tmra_dir_port, m_config.tmra_dir_pin);
    }

    /**
     * @brief 添加新的PWM对象
     * @param config PWM配置
     */
    void bsp_pwm::_bsp_add_pwm(const PWMConfig &config)
    {
        m_pwms.push_back(std::make_unique<bsp_pwm>(config));
    }

    /**
     * @brief 获取指定索引的PWM对象
     * @param index PWM索引
     * @return PWM对象指针，如果索引无效则返回nullptr
     */
    bsp_pwm *bsp_pwm::_bsp_get_pwm(size_t index)
    {
        if (index < m_pwms.size())
        {
            try
            {
                return m_pwms.at(index).get();
            } catch (const std::out_of_range &e)
            {
                // 处理异常，例如记录日志
                return nullptr;
            }
        }
        return nullptr;
    }
} // namespace BSP_PWM
