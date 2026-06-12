#include "can_comm_task.h"
#include "can_comm.h"
#include <string.h>

static can_comm_task_status_t g_can_comm_task_status;

/**
 * 名称：can_comm_task_init
 * 作用：初始化 CAN 板间通信任务和协议层状态。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示任务初始化完成。
 */
fsp_err_t can_comm_task_init(void)
{
    memset(&g_can_comm_task_status, 0, sizeof(g_can_comm_task_status));
    g_can_comm_task_status.last_data_status = FSP_ERR_NOT_OPEN;
    g_can_comm_task_status.last_test_status = FSP_ERR_NOT_OPEN;
    return can_comm_init();
}

/**
 * 名称：can_comm_task_send_data
 * 作用：根据边缘 AI 输出发送一帧数据帧，目前只携带是否适合晾衣服。
 * 接受值：p_ai_result 指向边缘 AI 输出。
 * 返回值：FSP_SUCCESS 表示发送成功；FSP_ERR_NOT_OPEN 表示底层 CANFD 尚未启用。
 */
fsp_err_t can_comm_task_send_data(edge_ai_app_result_t const * p_ai_result)
{
    if (NULL == p_ai_result)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    g_can_comm_task_status.last_data_status = can_comm_send_ai_data(&p_ai_result->output,
                                                                    g_can_comm_task_status.data_sequence);
    g_can_comm_task_status.data_sequence++;
    return g_can_comm_task_status.last_data_status;
}

/**
 * 名称：can_comm_task_send_test
 * 作用：发送一帧测试帧，用于板间 CAN 链路验证。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示发送成功；FSP_ERR_NOT_OPEN 表示底层 CANFD 尚未启用。
 */
fsp_err_t can_comm_task_send_test(void)
{
    g_can_comm_task_status.last_test_status = can_comm_send_test(g_can_comm_task_status.test_sequence);
    g_can_comm_task_status.test_sequence++;
    return g_can_comm_task_status.last_test_status;
}

/**
 * 名称：can_comm_task_get_status
 * 作用：获取 CAN 通信任务最近一次发送状态。
 * 接受值：无。
 * 返回值：状态结构体只读指针。
 */
can_comm_task_status_t const * can_comm_task_get_status(void)
{
    return &g_can_comm_task_status;
}