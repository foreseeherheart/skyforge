#include "application_entry.h"
#include "soc_board.h"
#include "sensor_task.h"
#include "edge_ai_app.h"
#include "display_task.h"
#include "can_comm_task.h"

/* 说明：主循环轮询外设、AI 推理和 OLED 刷新的周期，当前阶段不追求高频采样，先保证接线和流程稳定。 */
#define APPLICATION_SENSOR_POLL_INTERVAL_SECONDS    (2U)

/* 说明：CAN 测试帧发送间隔，单位为主循环次数；当前 5 次约等于 10 秒。 */
#define APPLICATION_CAN_TEST_INTERVAL_TICKS         (5U)

/* 说明：记录最近一次 FSP 调用返回值，方便调试时在 Watch 窗口中观察初始化或轮询错误。 */
static volatile fsp_err_t g_application_last_error = FSP_SUCCESS;

/* 说明：保存最近一次传感器轮询结果，方便后续任务或调试窗口读取温湿度和语音模块连接状态。 */
static sensor_task_status_t g_sensor_status;

/* 说明：保存最近一次边缘 AI 模型输入、输出和有效标志，便于调试部署后的模型行为。 */
static edge_ai_app_result_t g_edge_ai_result;

/* 说明：主循环计数器，用于控制 CAN 测试帧发送节奏。 */
static uint32_t g_application_loop_count = 0U;

/**
 * 名称：application_entry
 * 作用：应用层入口函数，由 FSP 的 hal_entry 调用，负责初始化板级资源、传感器任务、边缘 AI 应用和 OLED 显示任务。
 * 接受值：无。
 * 返回值：无。
 */
void application_entry(void)
{
    g_application_last_error = soc_board_init();
    if (FSP_SUCCESS != g_application_last_error)
    {
        while (1)
        {
            ;
        }
    }

    g_application_last_error = sensor_task_init();
    if (FSP_SUCCESS != g_application_last_error)
    {
        while (1)
        {
            ;
        }
    }

    g_application_last_error = edge_ai_app_init();
    if (FSP_SUCCESS != g_application_last_error)
    {
        while (1)
        {
            ;
        }
    }

    g_application_last_error = display_task_init();
    if (FSP_SUCCESS != g_application_last_error)
    {
        while (1)
        {
            ;
        }
    }

    g_application_last_error = can_comm_task_init();
    if (FSP_SUCCESS != g_application_last_error)
    {
        while (1)
        {
            ;
        }
    }

    while (1)
    {
        g_application_last_error = sensor_task_poll(&g_sensor_status);
        g_edge_ai_result = edge_ai_app_update(&g_sensor_status);
        display_task_show_status(&g_sensor_status, &g_edge_ai_result);

        (void) can_comm_task_send_data(&g_edge_ai_result);
        if (0U == (g_application_loop_count % APPLICATION_CAN_TEST_INTERVAL_TICKS))
        {
            (void) can_comm_task_send_test();
        }
        g_application_loop_count++;

        R_BSP_SoftwareDelay(APPLICATION_SENSOR_POLL_INTERVAL_SECONDS, BSP_DELAY_UNITS_SECONDS);
    }
}