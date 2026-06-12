#ifndef EDGE_AI_APP_H_
#define EDGE_AI_APP_H_

#include "hal_data.h"
#include "sensor_task.h"
#include "drying_ai_runtime.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 说明：应用层保存的 AI 推理结果，包含输入快照、输出结果以及本次推理是否基于有效传感器数据。 */
typedef struct st_edge_ai_app_result
{
    bool input_valid;
    DryingSensorData input;
    DryingAIResult output;
} edge_ai_app_result_t;

/**
 * 名称：edge_ai_app_init
 * 作用：初始化边缘 AI 应用框架，当前模型为 drying_ai_project 导出的轻量级 C 决策树。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示框架初始化完成。
 */
fsp_err_t edge_ai_app_init(void);

/**
 * 名称：edge_ai_app_update
 * 作用：把当前传感器状态转换为 drying_ai_project 模型输入，并执行一次边缘 AI 推理。
 * 接受值：p_status 指向传感器轮询结果。
 * 返回值：edge_ai_app_result_t，包含输入快照、模型输出和有效标志。
 */
edge_ai_app_result_t edge_ai_app_update(sensor_task_status_t const * p_status);

#ifdef __cplusplus
}
#endif

#endif /* EDGE_AI_APP_H_ */