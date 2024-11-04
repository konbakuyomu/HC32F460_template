/**
 * @file bsp_pwm.cpp
 * @brief 电机驱动PWM初始化
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-10-11
 */

#include "variable.h"

namespace BSP_PWM {

    std::unique_ptr<bsp_pwm> bsp_pwm::m_pwm;

    /**
     * @brief 添加PWM配置
     * @param name 名称
     * @param config 配置
     */
    void bsp_pwm::add_pwm(const std::string &name, const PWMConfig &config)
    {
        m_pwm_map[name] = config;
    }

    /**
     * @brief 初始化PWM
     */
    void bsp_pwm::init()
    {
        m_pwm = std::make_unique<bsp_pwm>();

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

        m_pwm->add_pwm("PWM1", config1);

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

        m_pwm->add_pwm("PWM2", config2);

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

        m_pwm->add_pwm("PWM3", config3);

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

        m_pwm->add_pwm("PWM4", config4);

        // TimerA初始化结构体
        stc_tmra_init_t stcTmraInit;
        // PWM初始化结构体
        stc_tmra_pwm_init_t stcPwmInit;

        for (const auto &pwm : m_pwm->m_pwm_map)
        {
            // 使能Timer时钟
            FCG_Fcg2PeriphClockCmd(pwm.second.tmra_clk, ENABLE);

            // 计算周期值
            uint32_t PeriodValue = HCLK1_FREQ / pwm.second.frequency;

            // 为stcTmraInit设置默认的初始化值并进行配置
            (void)TMRA_StructInit(&stcTmraInit);
            stcTmraInit.sw_count.u8CountMode = TMRA_MD_SAWTOOTH; // 设置为锯齿波模式
            stcTmraInit.sw_count.u8CountDir = TMRA_DIR_UP;       // 设置为向上计数
            stcTmraInit.u32PeriodValue = PeriodValue;            // 设置周期
            (void)TMRA_Init(pwm.second.tmra_unit, &stcTmraInit);

            // PWM占空比配置
            (void)TMRA_PWM_StructInit(&stcPwmInit);
            stcPwmInit.u32CompareValue = pwm.second.duty_cycle * PeriodValue;
            GPIO_SetFunc(pwm.second.tmra_port, pwm.second.tmra_pin, pwm.second.tmra_func);
            (void)TMRA_PWM_Init(pwm.second.tmra_unit, pwm.second.tmra_channel, &stcPwmInit);

            // PWM方向引脚初始化
            stc_gpio_init_t Dir_GPIO_Init;
            (void)GPIO_StructInit(&Dir_GPIO_Init);
            Dir_GPIO_Init.u16PinState = (pwm.second.tmra_dir_direction == PWM_DIR_FORWARD) ? PIN_SET : PIN_RESET; // 设置为高电平
            Dir_GPIO_Init.u16PinDir = PIN_DIR_OUT;                                                                // 设置为输出
            (void)GPIO_Init(pwm.second.tmra_dir_port, pwm.second.tmra_dir_pin, &Dir_GPIO_Init);

            // 启动TimerA
            TMRA_Start(pwm.second.tmra_unit);
        }
    }

    /**
     * @brief 启动PWM
     * @param name 名称
     */
    void bsp_pwm::pwm_start(const std::string &name)
    {
        if (m_pwm->m_pwm_map.find(name) != m_pwm->m_pwm_map.end())
        {
            const auto &pwm = m_pwm->m_pwm_map[name];
            TMRA_PWM_OutputCmd(pwm.tmra_unit, pwm.tmra_channel, ENABLE);
        }
    }

    /**
     * @brief 停止PWM
     * @param name 名称
     */
    void bsp_pwm::pwm_stop(const std::string &name)
    {
        if (m_pwm->m_pwm_map.find(name) != m_pwm->m_pwm_map.end())
        {
            const auto &pwm = m_pwm->m_pwm_map[name];
            TMRA_PWM_OutputCmd(pwm.tmra_unit, pwm.tmra_channel, DISABLE);
        }
    }

    /**
     * @brief 设置PWM占空比
     * @param name 名称
     * @param duty_cycle 占空比
     */
    void bsp_pwm::pwm_set_duty_cycle(const std::string &name, float duty_cycle)
    {
        if (m_pwm->m_pwm_map.find(name) != m_pwm->m_pwm_map.end())
        {
            auto &pwm = m_pwm->m_pwm_map[name];
            pwm.duty_cycle = duty_cycle;
            TMRA_Update_PWM_DutyCycle(pwm.tmra_unit, pwm.frequency, pwm.tmra_channel, pwm.duty_cycle);
        }
    }

    /**
     * @brief 设置PWM频率
     * @param name 名称
     * @param frequency 频率
     */
    void bsp_pwm::pwm_set_frequency(const std::string &name, uint32_t frequency)
    {
        if (m_pwm->m_pwm_map.find(name) != m_pwm->m_pwm_map.end())
        {
            auto &pwm = m_pwm->m_pwm_map[name];
            pwm.frequency = frequency;
            // 更新PWM硬件设置
            TMRA_SetPeriodValue(pwm.tmra_unit, HCLK1_FREQ / pwm.frequency);
        }
    }

    /**
     * @brief 获取当前PWM方向
     * @param name 名称
     * @return 当前方向（PWM_DIR_FORWARD 或 PWM_DIR_REVERSE）
     */
    uint8_t bsp_pwm::pwm_get_direction(const std::string &name)
    {
        if (m_pwm->m_pwm_map.find(name) != m_pwm->m_pwm_map.end())
        {
            const auto &pwm = m_pwm->m_pwm_map[name];
            return pwm.tmra_dir_direction;
        }
        return PWM_DIR_FORWARD;
    }

    /**
     * @brief 设置PWM方向
     * @param name 名称
     * @param direction 方向
     */
    void bsp_pwm::pwm_set_direction(const std::string &name, uint8_t direction)
    {
        if (m_pwm->m_pwm_map.find(name) != m_pwm->m_pwm_map.end())
        {
            auto &pwm = m_pwm->m_pwm_map[name];
            pwm.tmra_dir_direction = direction;
            (direction == PWM_DIR_FORWARD) ? GPIO_SetPins(pwm.tmra_dir_port, pwm.tmra_dir_pin) : GPIO_ResetPins(pwm.tmra_dir_port, pwm.tmra_dir_pin);
        }
    }
} // namespace BSP_PWM
