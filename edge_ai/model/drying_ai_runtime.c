#include "drying_ai_runtime.h"

/* 说明：晾晒机构允许的最大重量，超过该值时优先报警，不再执行普通模型动作。 */
#define DRYING_AI_MAX_WEIGHT_KG    (10.0f)

/**
 * 名称：drying_action_to_string
 * 作用：将动作建议枚举转换为调试用字符串。
 * 接受值：action 为动作建议枚举。
 * 返回值：动作字符串常量指针。
 */
const char* drying_action_to_string(DryingAction action)
{
    switch (action)
    {
        case DRYING_ACTION_NONE:
            return "None";

        case DRYING_ACTION_LOWER:
            return "Lower";

        case DRYING_ACTION_RAISE:
            return "Raise";

        case DRYING_ACTION_STOP:
            return "Stop";

        case DRYING_ACTION_ALARM:
            return "Alarm";

        default:
            return "Unknown";
    }
}

/**
 * 名称：drying_ai_update
 * 作用：综合安全条件和决策树模型，输出晾晒状态与动作建议。
 * 接受值：data 为传感器采样和安全状态输入。
 * 返回值：DryingAIResult，包含状态、动作和调试字符串。
 */
DryingAIResult drying_ai_update(DryingSensorData data)
{
    /* 说明：保存本次 AI 推理输出结果，函数末尾统一返回。 */
    DryingAIResult result;

    /* 安全保护优先级最高：过载、遇阻、限位先处理。 */
    if (data.weight_kg >= DRYING_AI_MAX_WEIGHT_KG)
    {
        result.state = DRYING_UNSUITABLE;
        result.action = DRYING_ACTION_ALARM;
        result.state_text = drying_state_to_string(result.state);
        result.action_text = drying_action_to_string(result.action);
        return result;
    }

    if (data.obstacle_detected)
    {
        result.state = DRYING_UNSUITABLE;
        result.action = DRYING_ACTION_STOP;
        result.state_text = drying_state_to_string(result.state);
        result.action_text = drying_action_to_string(result.action);
        return result;
    }

    result.state = predict_drying_state(
        data.temperature_c,
        data.humidity_pct,
        data.wind_speed_ms,
        data.solar_radiation,
        data.rainfall_mm,
        data.snowfall_cm
    );

    if (result.state == DRYING_SUITABLE)
    {
        result.action = data.lower_limit_triggered ? DRYING_ACTION_STOP : DRYING_ACTION_LOWER;
    }
    else if (result.state == DRYING_UNSUITABLE)
    {
        result.action = data.upper_limit_triggered ? DRYING_ACTION_STOP : DRYING_ACTION_RAISE;
    }
    else
    {
        result.action = DRYING_ACTION_NONE;
    }

    result.state_text = drying_state_to_string(result.state);
    result.action_text = drying_action_to_string(result.action);

    return result;
}