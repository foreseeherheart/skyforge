/*
 * 模块名称：US516P6 蜂鸟 M 语音模块
 * 使用接口：UART + 电源
 * 引脚对应：US516P6 VCC     -> RA6M5 开发板 5V
 * 引脚对应：US516P6 GND     -> RA6M5 开发板 GND
 * 引脚对应：US516P6 PB6/RXD -> J23 排针 P805 / SCI5 TXD5 -> BSP_IO_PORT_08_PIN_05
 * 引脚对应：US516P6 PB7/TXD -> J23 排针 P513 / SCI5 RXD5 -> BSP_IO_PORT_05_PIN_13
 * 备注：当前阶段先用 GPIO 检查串口空闲电平；后续在 RASC 中启用 SCI5 后再切换为真实 UART 收发。
 */
#ifndef US516P6_H_
#define US516P6_H_

#include "hal_data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum e_us516p6_link_state
{
    US516P6_LINK_UNKNOWN = 0,
    US516P6_LINK_IDLE_HIGH,
    US516P6_LINK_BUSY_LOW
} us516p6_link_state_t;

/**
 * 名称：us516p6_init
 * 作用：初始化 US516P6 语音模块 UART 连接所需的 RA6M5 引脚状态。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示引脚配置完成，其它值表示底层 IOPORT 配置失败。
 */
fsp_err_t us516p6_init(void);

/**
 * 名称：us516p6_get_link_state
 * 作用：读取语音模块 TX 线在 RA6M5 RX 引脚上的当前电平，用于初步判断串口连接是否处于空闲高电平。
 * 接受值：p_state 指向连接状态输出变量。
 * 返回值：FSP_SUCCESS 表示读取成功；FSP_ERR_INVALID_ARGUMENT 表示参数无效；其它值表示底层 IOPORT 读取失败。
 */
fsp_err_t us516p6_get_link_state(us516p6_link_state_t * p_state);

#ifdef __cplusplus
}
#endif

#endif /* US516P6_H_ */
