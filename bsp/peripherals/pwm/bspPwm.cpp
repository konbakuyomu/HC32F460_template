/**
 * @file BspPwm.cpp
 * @brief PWM底层驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-10-11
 */

#include "globalConfig.h"

namespace BspPwm
{

std::unique_ptr<BspPwm> BspPwm::mPwm;

/**
 * @brief 添加PWM配置
 * @param name 名称
 * @param config 配置
 */
void BspPwm::addPwm(const std::string &name, const PWMConfig &config)
{
    mPwmMap[name] = config;
}

/**
 * @brief 初始化PWM
 */
void BspPwm::init()
{
    mPwm = std::make_unique<BspPwm>();

    // 初始化PWM1
    BspPwm::PWMConfig config1 = {.tmraUnit = CM_TMRA_1,
                                  .tmraClk = FCG2_PERIPH_TMRA_1,
                                  .tmraChannel = TMRA_CH7,
                                  .tmraPort = GPIO_PORT_B,
                                  .tmraPin = GPIO_PIN_15,
                                  .tmraFunc = GPIO_FUNC_4,
                                  .hasDir = false,
                                  .frequency = 28000U,
                                  .dutyCycle = 0.9f};

    mPwm->addPwm("PWM1", config1);

    // 初始化PWM2
    BspPwm::PWMConfig config2 = {.tmraUnit = CM_TMRA_1,
                                  .tmraClk = FCG2_PERIPH_TMRA_1,
                                  .tmraChannel = TMRA_CH6,
                                  .tmraPort = GPIO_PORT_B,
                                  .tmraPin = GPIO_PIN_14,
                                  .tmraFunc = GPIO_FUNC_4,
                                  .hasDir = false,
                                  .frequency = 28000U,
                                  .dutyCycle = 0.9f};

    mPwm->addPwm("PWM2", config2);

    // TimerA初始化结构体
    stc_tmra_init_t stcTmraInit;
    // PWM初始化结构体
    stc_tmra_pwm_init_t stcPwmInit;

    for (const auto &pwm : mPwm->mPwmMap) {
        // 使能Timer时钟
        FCG_Fcg2PeriphClockCmd(pwm.second.tmraClk, ENABLE);

        // 计算周期值
        uint32_t PeriodValue = SYSTEM_PCLK1_FREQUENCY_HZ  / pwm.second.frequency;

        // 为stcTmraInit设置默认的初始化值并进行配置
        (void)TMRA_StructInit(&stcTmraInit);
        stcTmraInit.sw_count.u8CountMode = TMRA_MD_SAWTOOTH; // 设置为锯齿波模式
        stcTmraInit.sw_count.u8CountDir = TMRA_DIR_UP;       // 设置为向上计数
        stcTmraInit.u32PeriodValue = PeriodValue;            // 设置周期
        (void)TMRA_Init(pwm.second.tmraUnit, &stcTmraInit);

        // PWM占空比配置
        (void)TMRA_PWM_StructInit(&stcPwmInit);
        stcPwmInit.u32CompareValue = pwm.second.dutyCycle * PeriodValue;
        stcPwmInit.u16CompareMatchPolarity = TMRA_PWM_LOW;
        stcPwmInit.u16PeriodMatchPolarity = TMRA_PWM_HIGH;
        GPIO_SetFunc(pwm.second.tmraPort, pwm.second.tmraPin, pwm.second.tmraFunc);
        (void)TMRA_PWM_Init(pwm.second.tmraUnit, pwm.second.tmraChannel, &stcPwmInit);

        if (pwm.second.hasDir) {
            // PWM方向引脚初始化
            stc_gpio_init_t Dir_GPIO_Init;
            (void)GPIO_StructInit(&Dir_GPIO_Init);
            Dir_GPIO_Init.u16PinState = (pwm.second.tmraDirDirection == MOTOR_DIRECTION_FORWARD)
                                            ? PIN_SET
                                            : PIN_RESET; // 设置为高电平
            Dir_GPIO_Init.u16PinDir = PIN_DIR_OUT;       // 设置为输出
            (void)GPIO_Init(pwm.second.tmraDirPort, pwm.second.tmraDirPin, &Dir_GPIO_Init);
        }

        // 启动TimerA
        TMRA_Start(pwm.second.tmraUnit);
    }
}

/**
 * @brief 启动PWM
 * @param name 名称
 */
void BspPwm::startPwm(const std::string &name)
{
    auto it = mPwm->mPwmMap.find(name);

    if (it != mPwm->mPwmMap.end()) {
        const auto &pwm = it->second;
        TMRA_PWM_OutputCmd(pwm.tmraUnit, pwm.tmraChannel, ENABLE);
    }
}

/**
 * @brief 停止PWM
 * @param name 名称
 */
void BspPwm::stopPwm(const std::string &name)
{
    auto it = mPwm->mPwmMap.find(name);

    if (it != mPwm->mPwmMap.end()) {
        const auto &pwm = it->second;
        TMRA_PWM_OutputCmd(pwm.tmraUnit, pwm.tmraChannel, DISABLE);
    }
}

/**
 * @brief 设置PWM占空比
 * @param name 名称
 * @param dutyCycle 占空比
 */
/**
 * @brief 设置PWM占空比
 * @param name 名称
 * @param dutyCycle 占空比
 */
void BspPwm::setPwmDutyCycle(const std::string &name, float dutyCycle)
{
    auto it = mPwm->mPwmMap.find(name);

    if (it != mPwm->mPwmMap.end()) {
        auto &pwm = it->second;
        pwm.dutyCycle = dutyCycle;
        TMRA_Update_PWM_DutyCycle(pwm.tmraUnit, pwm.frequency, pwm.tmraChannel, pwm.dutyCycle);
    }
}

/**
 * @brief 设置PWM频率
 * @param name 名称
 * @param frequency 频率
 */
void BspPwm::setPwmFrequency(const std::string &name, uint32_t frequency)
{
    auto it = mPwm->mPwmMap.find(name);

    if (it != mPwm->mPwmMap.end()) {
        auto &pwm = it->second;
        pwm.frequency = frequency;
        TMRA_SetPeriodValue(pwm.tmraUnit, SYSTEM_PCLK1_FREQUENCY_HZ  / pwm.frequency);
    }
}

/**
 * @brief 获取当前PWM方向
 * @param name 名称
 * @return 当前方向（PWM_DIR_FORWARD 或 PWM_DIR_REVERSE）
 */
uint8_t BspPwm::getPwmDirection(const std::string &name)
{
    auto it = mPwm->mPwmMap.find(name);

    if (it != mPwm->mPwmMap.end()) {
        const auto &pwm = it->second;
        return pwm.tmraDirDirection;
    }
    return MOTOR_DIRECTION_FORWARD;
}

/**
 * @brief 设置PWM方向
 * @param name 名称
 * @param direction 方向
 */
void BspPwm::setPwmDirection(const std::string &name, uint8_t direction)
{
    auto it = mPwm->mPwmMap.find(name);

    if (it != mPwm->mPwmMap.end()) {
        auto &pwm = it->second;
        pwm.tmraDirDirection = direction;
        (direction == MOTOR_DIRECTION_FORWARD) ? GPIO_SetPins(pwm.tmraDirPort, pwm.tmraDirPin)
                                             : GPIO_ResetPins(pwm.tmraDirPort, pwm.tmraDirPin);
    }
}
} // namespace BspPwm
