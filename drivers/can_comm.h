/**
 * 模块名称：SkyForge CAN/CANFD 板间通信协议
 * 引脚对应：RA6M5 CANFD TX/RX 尚未在 RASC 中启用，需外接 CAN 收发器后再选择实际引脚。
 * 说明：CAN 控制器引脚不能直接两板互连，必须经 CAN 收发器转换为 CANH/CANL 总线。
 */
#ifndef CAN_COMM_H_
#define CAN_COMM_H_

#include "hal_data.h"
#include "drying_ai_runtime.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 说明：SkyForge CAN 协议版本号，接收端可据此兼容后续字段扩展。 */
#define CAN_COMM_PROTOCOL_VERSION       (1U)

/* 说明：CAN 标准帧 ID，测试帧用于链路验证。 */
#define CAN_COMM_TEST_FRAME_ID          (0x320U)

/* 说明：CAN 标准帧 ID，数据帧用于发送 AI 晾晒状态。 */
#define CAN_COMM_DATA_FRAME_ID          (0x321U)

/* 说明：当前协议使用 CANFD 16 字节数据区，后续增加温湿度、光照、动作建议等字段时不用重改帧结构。 */
#define CAN_COMM_PAYLOAD_BYTES          (16U)

/* 说明：有效数据区长度不包含最后 2 字节 CRC16。 */
#define CAN_COMM_PAYLOAD_WITHOUT_CRC    (CAN_COMM_PAYLOAD_BYTES - 2U)

/* 说明：帧头第 1 字节，ASCII 'S'。 */
#define CAN_COMM_HEADER_0               (0x53U)

/* 说明：帧头第 2 字节，ASCII 'F'，代表 SkyForge。 */
#define CAN_COMM_HEADER_1               (0x46U)

/* 说明：数据帧类型，负载中携带 AI 是否适合晾衣服以及预留字段。 */
#define CAN_COMM_FRAME_TYPE_DATA        (0x01U)

/* 说明：测试帧类型，负载中携带计数器和固定花码，用于检查板间通信。 */
#define CAN_COMM_FRAME_TYPE_TEST        (0x7EU)

/* 说明：AI 状态数据帧的标志位，bit0 为 1 表示当前 AI 判断适合晾衣服。 */
#define CAN_COMM_DATA_FLAG_SUITABLE     (0x01U)

/* 说明：SkyForge CAN 协议封装后的帧，底层可映射为 CANFD 发送结构。 */
typedef struct st_can_comm_frame
{
    uint32_t id;
    uint8_t dlc_bytes;
    uint8_t data[CAN_COMM_PAYLOAD_BYTES];
} can_comm_frame_t;

/* 说明：CAN 通信驱动状态，用于调试窗口观察协议层是否构帧、底层是否已启用。 */
typedef struct st_can_comm_status
{
    uint32_t tx_attempt_count;
    uint32_t tx_ok_count;
    fsp_err_t last_error;
    can_comm_frame_t last_frame;
} can_comm_status_t;

/**
 * 名称：can_comm_init
 * 作用：初始化 CAN 通信协议层并清空调试状态。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示协议层初始化完成。
 */
fsp_err_t can_comm_init(void);

/**
 * 名称：can_comm_send_ai_data
 * 作用：封装并发送 AI 晾晒状态数据帧。
 * 接受值：ai_result 为边缘 AI 输出；sequence 为发送序号。
 * 返回值：FSP_SUCCESS 表示发送接口调用成功；FSP_ERR_NOT_OPEN 表示底层 CANFD 尚未接入。
 */
fsp_err_t can_comm_send_ai_data(DryingAIResult const * ai_result, uint8_t sequence);

/**
 * 名称：can_comm_send_test
 * 作用：封装并发送测试帧，包含帧头、计数器、固定花码和 CRC16。
 * 接受值：sequence 为发送序号。
 * 返回值：FSP_SUCCESS 表示发送接口调用成功；FSP_ERR_NOT_OPEN 表示底层 CANFD 尚未接入。
 */
fsp_err_t can_comm_send_test(uint8_t sequence);

/**
 * 名称：can_comm_crc16_ccitt
 * 作用：计算 SkyForge CAN 负载 CRC16-CCITT，用于测试帧和数据帧校验。
 * 接受值：p_data 为待校验数据；length 为数据长度。
 * 返回值：CRC16 校验值，初值 0xFFFF，多项式 0x1021。
 */
uint16_t can_comm_crc16_ccitt(uint8_t const * p_data, uint8_t length);

/**
 * 名称：can_comm_get_status
 * 作用：获取 CAN 通信协议层最近一次发送状态。
 * 接受值：无。
 * 返回值：状态结构体只读指针。
 */
can_comm_status_t const * can_comm_get_status(void);

#ifdef __cplusplus
}
#endif

#endif /* CAN_COMM_H_ */