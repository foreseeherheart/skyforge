#include "drying_tree_model.h"

/**
 * 名称：predict_drying_state
 * 作用：执行由 drying_ai_project 导出的轻量级决策树模型，判断当前环境是否适合晾晒。
 * 接受值：temperature_c 摄氏温度；humidity_pct 相对湿度百分比；wind_speed_ms 风速；solar_radiation 光照/太阳辐射；rainfall_mm 降雨量；snowfall_cm 降雪量。
 * 返回值：DRYING_UNSUITABLE、DRYING_MODERATE 或 DRYING_SUITABLE。
 */
DryingState predict_drying_state(
    float temperature_c,
    float humidity_pct,
    float wind_speed_ms,
    float solar_radiation,
    float rainfall_mm,
    float snowfall_cm
)
{
    if (humidity_pct <= 84.500000f)
    {
        if (solar_radiation <= 0.295000f)
        {
            if (snowfall_cm <= 0.050000f)
            {
                if (rainfall_mm <= 0.050000f)
                {
                    return DRYING_MODERATE;
                }
                else
                {
                    if (rainfall_mm <= 0.350000f)
                    {
                        return DRYING_UNSUITABLE;
                    }
                    else
                    {
                        return DRYING_UNSUITABLE;
                    }
                }
            }
            else
            {
                return DRYING_UNSUITABLE;
            }
        }
        else
        {
            if (temperature_c <= 9.950000f)
            {
                if (snowfall_cm <= 0.100000f)
                {
                    if (temperature_c <= -8.450000f)
                    {
                        return DRYING_MODERATE;
                    }
                    else
                    {
                        return DRYING_MODERATE;
                    }
                }
                else
                {
                    if (wind_speed_ms <= 1.400000f)
                    {
                        return DRYING_UNSUITABLE;
                    }
                    else
                    {
                        return DRYING_UNSUITABLE;
                    }
                }
            }
            else
            {
                if (humidity_pct <= 69.500000f)
                {
                    if (temperature_c <= 35.049999f)
                    {
                        return DRYING_SUITABLE;
                    }
                    else
                    {
                        return DRYING_MODERATE;
                    }
                }
                else
                {
                    if (solar_radiation <= 0.890000f)
                    {
                        return DRYING_MODERATE;
                    }
                    else
                    {
                        return DRYING_MODERATE;
                    }
                }
            }
        }
    }
    else
    {
        return DRYING_UNSUITABLE;
    }
}

/**
 * 名称：drying_state_to_string
 * 作用：将晾晒状态枚举转换为调试用字符串。
 * 接受值：state 为晾晒状态枚举。
 * 返回值：状态字符串常量指针。
 */
const char* drying_state_to_string(DryingState state)
{
    switch (state)
    {
        case DRYING_UNSUITABLE:
            return "Unsuitable";

        case DRYING_MODERATE:
            return "Moderate";

        case DRYING_SUITABLE:
            return "Suitable";

        default:
            return "Unknown";
    }
}