#ifndef SOC_BOARD_H_
#define SOC_BOARD_H_

#include "hal_data.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 说明：DHT11 DATA 连接到 J23 排针 P313，对应 RA6M5 P313。 */
#define SOC_BOARD_DHT11_DATA_PIN      BSP_IO_PORT_03_PIN_13

/* 说明：RA6M5 发送到 US516P6 RXD 的 UART 线，连接 J23 排针 P805，对应 RA6M5 P805，可复用为 SCI5 TXD5。 */
#define SOC_BOARD_US516P6_RA_TX_PIN   BSP_IO_PORT_08_PIN_05

/* 说明：RA6M5 接收 US516P6 TXD 的 UART 线，连接 J23 排针 P513，对应 RA6M5 P513，可复用为 SCI5 RXD5。 */
#define SOC_BOARD_US516P6_RA_RX_PIN   BSP_IO_PORT_05_PIN_13

/* 说明：OLED 软件 I2C 时钟线，沿用队友工程引脚，对应 RA6M5 P008。 */
#define SOC_BOARD_OLED_SCL_PIN        BSP_IO_PORT_00_PIN_08

/* 说明：OLED 软件 I2C 数据线，沿用队友工程引脚，对应 RA6M5 P009。 */
#define SOC_BOARD_OLED_SDA_PIN        BSP_IO_PORT_00_PIN_09

/* 说明：光敏电阻传感器数字输出 DO，连接 J23 排针 P304，对应 RA6M5 P304。 */
#define SOC_BOARD_LIGHT_SENSOR_DO_PIN BSP_IO_PORT_03_PIN_04

/* 说明：DHT11 空闲状态为输入上拉，读取时驱动层会临时切换为输出低电平。 */
#define SOC_BOARD_DHT11_DATA_CFG      (IOPORT_CFG_PORT_DIRECTION_INPUT | IOPORT_CFG_PULLUP_ENABLE)

/* 说明：UART 空闲电平为高，当前阶段未启用 SCI 外设前先保持 TX 为普通 GPIO 高电平。 */
#define SOC_BOARD_US516P6_TX_CFG      (IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_HIGH)

/* 说明：语音模块 TX 接入 RA6M5 RX，配置输入上拉用于检查串口空闲状态。 */
#define SOC_BOARD_US516P6_RX_CFG      (IOPORT_CFG_PORT_DIRECTION_INPUT | IOPORT_CFG_PULLUP_ENABLE)

/* 说明：OLED I2C 线配置为 NMOS 开漏输出并默认释放为高电平，适合 I2C 总线。 */
#define SOC_BOARD_OLED_I2C_CFG        (IOPORT_CFG_NMOS_ENABLE | IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_HIGH)

/* 说明：光敏 DO 为数字比较器输出，启用上拉可兼容开漏输出模块，模块请使用 3V3 供电。 */
#define SOC_BOARD_LIGHT_SENSOR_DO_CFG (IOPORT_CFG_PORT_DIRECTION_INPUT | IOPORT_CFG_PULLUP_ENABLE)

/**
 * 名称：soc_board_init
 * 作用：统一配置当前阶段外接模块所需的 RA6M5 GPIO 引脚。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示配置完成，其它值表示底层 IOPORT 配置失败。
 */
fsp_err_t soc_board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* SOC_BOARD_H_ */