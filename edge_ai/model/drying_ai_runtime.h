#ifndef DRYING_AI_RUNTIME_H
#define DRYING_AI_RUNTIME_H

#include "drying_tree_model.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 说明：AI 模型输出后对应的执行建议，用于后续控制电机、报警或保持当前状态。 */
typedef enum
{
    DRYING_ACTION_NONE = 0,
    DRYING_ACTION_LOWER = 1,
    DRYING_ACTION_RAISE = 2,
    DRYING_ACTION_STOP = 3,
    DRYING_ACTION_ALARM = 4
} DryingAction;

/* 说明：边缘 AI 推理输入数据，当前阶段部分字段使用默认值，后续接入光照、称重、限位等模块后再替换为真实采样。 */
typedef struct
{
    float temperature_c;
    float humidity_pct;
    float wind_speed_ms;
    float solar_radiation;
    float rainfall_mm;
    float snowfall_cm;

    float weight_kg;
    int upper_limit_triggered;
    int lower_limit_triggered;
    int obstacle_detected;
} DryingSensorData;

/* 说明：边缘 AI 推理输出结果，包含模型状态、动作建议以及便于调试观察的字符串。 */
typedef struct
{
    DryingState state;
    DryingAction action;
    const char* state_text;
    const char* action_text;
} DryingAIResult;

/**
 * 名称：drying_ai_update
 * 作用：综合安全条件和决策树模型，输出晾晒状态与动作建议。
 * 接受值：data 为传感器采样和安全状态输入。
 * 返回值：DryingAIResult，包含状态、动作和调试字符串。
 */
DryingAIResult drying_ai_update(DryingSensorData data);

/**
 * 名称：drying_action_to_string
 * 作用：将动作建议枚举转换为调试用字符串。
 * 接受值：action 为动作建议枚举。
 * 返回值：动作字符串常量指针。
 */
const char* drying_action_to_string(DryingAction action);

#ifdef __cplusplus
}
#endif

#endif /* DRYING_AI_RUNTIME_H */