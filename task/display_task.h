#ifndef DISPLAY_TASK_H_
#define DISPLAY_TASK_H_

#include "hal_data.h"
#include "sensor_task.h"
#include "edge_ai_app.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 名称：display_task_init
 * 作用：初始化 OLED 显示任务，第一行居中显示 SkyForge 并完成一次渐显渐隐。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示初始化完成。
 */
fsp_err_t display_task_init(void);

/**
 * 名称：display_task_show_status
 * 作用：将温湿度、光敏电阻、语音模块连接状态和边缘 AI 输出刷新到 OLED。
 * 接受值：p_sensor_status 为传感器状态；p_ai_result 为边缘 AI 结果。
 * 返回值：无。
 */
void display_task_show_status(sensor_task_status_t const * p_sensor_status,
                              edge_ai_app_result_t const * p_ai_result);

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_TASK_H_ */