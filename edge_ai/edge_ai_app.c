#include "edge_ai_app.h"
#include <string.h>

/* 说明：DHT11 暂未接通或读取失败时使用的默认环境温度，保证 AI 模型仍可部署运行。 */
#define EDGE_AI_DEFAULT_TEMPERATURE_C        (25.0f)

/* 说明：DHT11 暂未接通或读取失败时使用的默认环境湿度，保证 AI 模型仍可部署运行。 */
#define EDGE_AI_DEFAULT_HUMIDITY_PCT         (55.0f)

/* 说明：暂未接入风速模块时使用的默认风速，保证 drying_ai_project 模型输入完整。 */
#define EDGE_AI_DEFAULT_WIND_SPEED_MS        (1.5f)

/* 说明：光敏模块未触发或读取失败时使用的保守光照特征。 */
#define EDGE_AI_DEFAULT_SOLAR_RADIATION      (0.5f)

/* 说明：光敏 DO 低电平触发时映射到边缘 AI 的较高光照特征，后续 ADC 接入后替换为连续值。 */
#define EDGE_AI_LIGHT_TRIGGERED_RADIATION    (1.0f)

/* 说明：暂未接入雨雪检测时使用的默认降雨量。 */
#define EDGE_AI_DEFAULT_RAINFALL_MM          (0.0f)

/* 说明：暂未接入雨雪检测时使用的默认降雪量。 */
#define EDGE_AI_DEFAULT_SNOWFALL_CM          (0.0f)

/* 说明：暂未接入称重模块时使用的默认重量，避免触发过载保护。 */
#define EDGE_AI_DEFAULT_WEIGHT_KG            (0.0f)

/**
 * 名称：edge_ai_app_init
 * 作用：初始化边缘 AI 应用框架，当前模型为 drying_ai_project 导出的轻量级 C 决策树。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示框架初始化完成。
 */
fsp_err_t edge_ai_app_init(void)
{
    return FSP_SUCCESS;
}

/**
 * 名称：edge_ai_app_update
 * 作用：把当前传感器状态转换为 drying_ai_project 模型输入，并执行一次边缘 AI 推理。
 * 接受值：p_status 指向传感器轮询结果。
 * 返回值：edge_ai_app_result_t，包含输入快照、模型输出和有效标志。
 */
edge_ai_app_result_t edge_ai_app_update(sensor_task_status_t const * p_status)
{
    /* 说明：保存本次边缘 AI 推理的输入快照、输出结果和有效标志。 */
    edge_ai_app_result_t result;
    memset(&result, 0, sizeof(result));

    result.input.temperature_c = EDGE_AI_DEFAULT_TEMPERATURE_C;
    result.input.humidity_pct = EDGE_AI_DEFAULT_HUMIDITY_PCT;
    result.input.wind_speed_ms = EDGE_AI_DEFAULT_WIND_SPEED_MS;
    result.input.solar_radiation = EDGE_AI_DEFAULT_SOLAR_RADIATION;
    if ((NULL != p_status) &&
        (FSP_SUCCESS == p_status->light_sensor_status) &&
        p_status->light_sensor_data.threshold_reached)
    {
        result.input.solar_radiation = EDGE_AI_LIGHT_TRIGGERED_RADIATION;
    }
    result.input.rainfall_mm = EDGE_AI_DEFAULT_RAINFALL_MM;
    result.input.snowfall_cm = EDGE_AI_DEFAULT_SNOWFALL_CM;
    result.input.weight_kg = EDGE_AI_DEFAULT_WEIGHT_KG;

    if ((NULL != p_status) && (FSP_SUCCESS == p_status->dht11_status))
    {
        result.input_valid = true;
        result.input.humidity_pct = (float) p_status->dht11_data.humidity_integer +
                                    ((float) p_status->dht11_data.humidity_decimal / 10.0f);
        result.input.temperature_c = (float) p_status->dht11_data.temperature_integer +
                                     ((float) p_status->dht11_data.temperature_decimal / 10.0f);
    }
    else
    {
        result.input_valid = true;
    }

    result.output = drying_ai_update(result.input);
    return result;
}