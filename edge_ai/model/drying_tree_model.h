#ifndef DRYING_TREE_MODEL_H
#define DRYING_TREE_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

/* 说明：晾晒环境状态枚举，数值保持与导出的决策树模型类别一致。 */
typedef enum
{
    DRYING_UNSUITABLE = 0,
    DRYING_MODERATE = 1,
    DRYING_SUITABLE = 2
} DryingState;

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
);

/**
 * 名称：drying_state_to_string
 * 作用：将晾晒状态枚举转换为调试用字符串。
 * 接受值：state 为晾晒状态枚举。
 * 返回值：状态字符串常量指针。
 */
const char* drying_state_to_string(DryingState state);

#ifdef __cplusplus
}
#endif

#endif /* DRYING_TREE_MODEL_H */