#ifndef CAN_COMM_TASK_H_
#define CAN_COMM_TASK_H_

#include "hal_data.h"
#include "edge_ai_app.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 说明：CAN 通信任务状态，便于调试时观察数据帧和测试帧发送结果。 */
typedef struct st_can_comm_task_status
{
    uint8_t data_sequence;
    uint8_t test_sequence;
    fsp_err_t last_data_status;
    fsp_err_t last_test_status;
} can_comm_task_status_t;

/**
 * 名称：can_comm_task_init
 * 作用：初始化 CAN 板间通信任务和协议层状态。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示任务初始化完成。
 */
fsp_err_t can_comm_task_init(void);

/**
 * 名称：can_comm_task_send_data
 * 作用：根据边缘 AI 输出发送一帧数据帧，目前只携带是否适合晾衣服。
 * 接受值：p_ai_result 指向边缘 AI 输出。
 * 返回值：FSP_SUCCESS 表示发送成功；FSP_ERR_NOT_OPEN 表示底层 CANFD 尚未启用。
 */
fsp_err_t can_comm_task_send_data(edge_ai_app_result_t const * p_ai_result);

/**
 * 名称：can_comm_task_send_test
 * 作用：发送一帧测试帧，用于板间 CAN 链路验证。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示发送成功；FSP_ERR_NOT_OPEN 表示底层 CANFD 尚未启用。
 */
fsp_err_t can_comm_task_send_test(void);

/**
 * 名称：can_comm_task_get_status
 * 作用：获取 CAN 通信任务最近一次发送状态。
 * 接受值：无。
 * 返回值：状态结构体只读指针。
 */
can_comm_task_status_t const * can_comm_task_get_status(void);

#ifdef __cplusplus
}
#endif

#endif /* CAN_COMM_TASK_H_ */