/**
 * @file BspPwm.h
 * @brief PWM底层驱动
 * @author konbakuyomu
 * @version 1.0
 * @date 2024-10-11
 */

#pragma once
#ifdef __cplusplus

#include "hal.hpp"
#include "hc32_ll.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace BspPwm
{

/**
 * @brief PWM类
 * @note 该类用于控制PWM的硬件参数
 */
class BspPwm
{
  private:
    /**
     * @brief PWM配置结构体
     * @note 该结构体用于配置PWM的硬件参数
     */
    struct PWMConfig {
        CM_TMRA_TypeDef *tmraUnit; // TMRA定时器单元指针
        uint32_t tmraClk;          // TMRA定时器时钟源
        uint8_t tmraChannel;       // TMRA定时器通道
        uint8_t tmraPort;          // TMRA定时器端口
        uint16_t tmraPin;          // TMRA定时器引脚
        uint8_t tmraFunc;          // TMRA定时器功能选择
        bool hasDir;            // 是否具有方向控制
        uint8_t tmraDirPort;      // TMRA方向控制端口
        uint16_t tmraDirPin;      // TMRA方向控制引脚
        uint8_t tmraDirDirection; // TMRA方向控制引脚方向
        int frequency;              // PWM频率（Hz）
        float dutyCycle;           // PWM占空比（0.0 - 1.0）
    };

    std::map<std::string, PWMConfig> mPwmMap; // 存储PWM配置的容器
    static std::unique_ptr<BspPwm> mPwm;

  public:
    BspPwm() = default;
    ~BspPwm() = default;

  public:
    void addPwm(const std::string &name, const PWMConfig &config);

  public:
    static void init();
    static void startPwm(const std::string &name);
    static void stopPwm(const std::string &name);
    static void setPwmDutyCycle(const std::string &name, float dutyCycle);
    static void setPwmFrequency(const std::string &name, uint32_t frequency);
    static uint8_t getPwmDirection(const std::string &name);
    static void setPwmDirection(const std::string &name, uint8_t direction);
};

} // namespace BspPwm
#endif
