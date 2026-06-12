/**
 * 模块名称：SkyForge CAN/CANFD 板间通信协议
 * 引脚对应：RA6M5 CANFD TX/RX 尚未在 RASC 中启用，需外接 CAN 收发器后再选择实际引脚。
 * 说明：CAN 控制器引脚不能直接两板互连，必须经 CAN 收发器转换为 CANH/CANL 总线。
 */
#include "can_comm.h"
#include <string.h>

/* 说明：CANFD 硬件尚未通过 RASC 生成实例时保持为 0；生成 g_canfd 后在本文件底部替换发送实现。 */
#define CAN_COMM_FSP_TRANSPORT_ENABLED      (0U)

/* 说明：数据帧中当前保留的应用数据长度，后续增加字段时从保留区继续分配。 */
#define CAN_COMM_DATA_FIELD_LENGTH          (4U)

/* 说明：测试帧中当前保留的测试数据长度。 */
#define CAN_COMM_TEST_FIELD_LENGTH          (8U)

static can_comm_status_t g_can_comm_status;

static void can_comm_fill_common_header(can_comm_frame_t * p_frame, uint32_t id, uint8_t frame_type, uint8_t sequence);
static void can_comm_append_crc(can_comm_frame_t * p_frame);
static fsp_err_t can_comm_send_frame(can_comm_frame_t const * p_frame);
static fsp_err_t can_comm_transport_send(can_comm_frame_t const * p_frame);

/**
 * 名称：can_comm_init
 * 作用：初始化 CAN 通信协议层并清空调试状态。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示协议层初始化完成。
 */
fsp_err_t can_comm_init(void)
{
    memset(&g_can_comm_status, 0, sizeof(g_can_comm_status));
    g_can_comm_status.last_error = FSP_ERR_NOT_OPEN;
    return FSP_SUCCESS;
}

/**
 * 名称：can_comm_send_ai_data
 * 作用：封装并发送 AI 晾晒状态数据帧。
 * 接受值：ai_result 为边缘 AI 输出；sequence 为发送序号。
 * 返回值：FSP_SUCCESS 表示发送接口调用成功；FSP_ERR_NOT_OPEN 表示底层 CANFD 尚未接入。
 */
fsp_err_t can_comm_send_ai_data(DryingAIResult const * ai_result, uint8_t sequence)
{
    if (NULL == ai_result)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    can_comm_frame_t frame;
    can_comm_fill_common_header(&frame, CAN_COMM_DATA_FRAME_ID, CAN_COMM_FRAME_TYPE_DATA, sequence);

    frame.data[5] = CAN_COMM_DATA_FIELD_LENGTH;
    frame.data[6] = (uint8_t) ai_result->state;
    frame.data[7] = (DRYING_SUITABLE == ai_result->state) ? CAN_COMM_DATA_FLAG_SUITABLE : 0U;
    frame.data[8] = (uint8_t) ai_result->action;
    frame.data[9] = 0U;
    frame.data[10] = 0U;
    frame.data[11] = 0U;
    frame.data[12] = 0U;
    frame.data[13] = 0U;

    can_comm_append_crc(&frame);
    return can_comm_send_frame(&frame);
}

/**
 * 名称：can_comm_send_test
 * 作用：封装并发送测试帧，包含帧头、计数器、固定花码和 CRC16。
 * 接受值：sequence 为发送序号。
 * 返回值：FSP_SUCCESS 表示发送接口调用成功；FSP_ERR_NOT_OPEN 表示底层 CANFD 尚未接入。
 */
fsp_err_t can_comm_send_test(uint8_t sequence)
{
    can_comm_frame_t frame;
    can_comm_fill_common_header(&frame, CAN_COMM_TEST_FRAME_ID, CAN_COMM_FRAME_TYPE_TEST, sequence);

    frame.data[5] = CAN_COMM_TEST_FIELD_LENGTH;
    frame.data[6] = 0xA5U;
    frame.data[7] = 0x5AU;
    frame.data[8] = sequence;
    frame.data[9] = (uint8_t) ~sequence;
    frame.data[10] = 0x12U;
    frame.data[11] = 0x34U;
    frame.data[12] = 0x56U;
    frame.data[13] = 0x78U;

    can_comm_append_crc(&frame);
    return can_comm_send_frame(&frame);
}

/**
 * 名称：can_comm_crc16_ccitt
 * 作用：计算 SkyForge CAN 负载 CRC16-CCITT，用于测试帧和数据帧校验。
 * 接受值：p_data 为待校验数据；length 为数据长度。
 * 返回值：CRC16 校验值，初值 0xFFFF，多项式 0x1021。
 */
uint16_t can_comm_crc16_ccitt(uint8_t const * p_data, uint8_t length)
{
    uint16_t crc = 0xFFFFU;

    if (NULL == p_data)
    {
        return crc;
    }

    for (uint8_t i = 0U; i < length; i++)
    {
        crc ^= (uint16_t) ((uint16_t) p_data[i] << 8U);
        for (uint8_t bit = 0U; bit < 8U; bit++)
        {
            if (0U != (crc & 0x8000U))
            {
                crc = (uint16_t) (((uint32_t) crc << 1U) ^ 0x1021U);
            }
            else
            {
                crc = (uint16_t) ((uint32_t) crc << 1U);
            }
        }
    }

    return crc;
}

/**
 * 名称：can_comm_get_status
 * 作用：获取 CAN 通信协议层最近一次发送状态。
 * 接受值：无。
 * 返回值：状态结构体只读指针。
 */
can_comm_status_t const * can_comm_get_status(void)
{
    return &g_can_comm_status;
}

/**
 * 名称：can_comm_fill_common_header
 * 作用：填充 SkyForge CAN 帧公共头部和基础字段。
 * 接受值：p_frame 为待填充帧；id 为 CAN 帧 ID；frame_type 为协议帧类型；sequence 为发送序号。
 * 返回值：无。
 */
static void can_comm_fill_common_header(can_comm_frame_t * p_frame, uint32_t id, uint8_t frame_type, uint8_t sequence)
{
    memset(p_frame, 0, sizeof(*p_frame));
    p_frame->id = id;
    p_frame->dlc_bytes = CAN_COMM_PAYLOAD_BYTES;
    p_frame->data[0] = CAN_COMM_HEADER_0;
    p_frame->data[1] = CAN_COMM_HEADER_1;
    p_frame->data[2] = CAN_COMM_PROTOCOL_VERSION;
    p_frame->data[3] = frame_type;
    p_frame->data[4] = sequence;
}

/**
 * 名称：can_comm_append_crc
 * 作用：计算并写入帧末尾 CRC16，低字节在前，高字节在后。
 * 接受值：p_frame 为待补 CRC 的协议帧。
 * 返回值：无。
 */
static void can_comm_append_crc(can_comm_frame_t * p_frame)
{
    uint16_t crc = can_comm_crc16_ccitt(p_frame->data, CAN_COMM_PAYLOAD_WITHOUT_CRC);
    p_frame->data[CAN_COMM_PAYLOAD_WITHOUT_CRC] = (uint8_t) (crc & 0xFFU);
    p_frame->data[CAN_COMM_PAYLOAD_WITHOUT_CRC + 1U] = (uint8_t) ((crc >> 8U) & 0xFFU);
}

/**
 * 名称：can_comm_send_frame
 * 作用：更新发送调试状态并调用底层 CAN/CANFD 发送出口。
 * 接受值：p_frame 为待发送协议帧。
 * 返回值：底层发送结果。
 */
static fsp_err_t can_comm_send_frame(can_comm_frame_t const * p_frame)
{
    g_can_comm_status.tx_attempt_count++;
    g_can_comm_status.last_frame = *p_frame;

    fsp_err_t err = can_comm_transport_send(p_frame);
    g_can_comm_status.last_error = err;
    if (FSP_SUCCESS == err)
    {
        g_can_comm_status.tx_ok_count++;
    }

    return err;
}

/**
 * 名称：can_comm_transport_send
 * 作用：CAN/CANFD 底层发送出口；当前工程尚未生成 FSP CANFD 实例，先返回未打开。
 * 接受值：p_frame 为待发送协议帧。
 * 返回值：FSP_ERR_NOT_OPEN 表示 RASC CANFD 尚未启用；接入 FSP 后返回实际 Write 结果。
 */
static fsp_err_t can_comm_transport_send(can_comm_frame_t const * p_frame)
{
    FSP_PARAMETER_NOT_USED(p_frame);

#if CAN_COMM_FSP_TRANSPORT_ENABLED
    /* 后续在 RASC 生成 CANFD 实例后，将 p_frame 映射为 can_frame_t 并调用 g_canfd.p_api->write。 */
    return FSP_SUCCESS;
#else
    return FSP_ERR_NOT_OPEN;
#endif
}