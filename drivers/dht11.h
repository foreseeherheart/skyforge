/*
 * 模块名称：DHT11 温湿度模块
 * 使用接口：单总线 DATA + 电源
 * 引脚对应：DHT11 VCC  -> RA6M5 开发板 3V3
 * 引脚对应：DHT11 GND  -> RA6M5 开发板 GND
 * 引脚对应：DHT11 DATA -> J23 排针 P313 -> BSP_IO_PORT_03_PIN_13
 * 备注：DATA 建议上拉到 3V3，若模块板已带上拉电阻可直接连接。
 */
#ifndef DHT11_H_
#define DHT11_H_

#include "hal_data.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_dht11_data
{
    uint8_t humidity_integer;
    uint8_t humidity_decimal;
    uint8_t temperature_integer;
    uint8_t temperature_decimal;
} dht11_data_t;

/**
 * 名称：dht11_init
 * 作用：初始化 DHT11 单总线引脚为空闲上拉输入状态。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示初始化成功，其它值表示引脚配置失败。
 */
fsp_err_t dht11_init(void);

/**
 * 名称：dht11_read
 * 作用：读取一次 DHT11 温湿度数据，并完成校验和检查。
 * 接受值：p_data 指向用于接收温湿度结果的结构体。
 * 返回值：FSP_SUCCESS 表示读取成功；FSP_ERR_INVALID_ARGUMENT 表示参数无效；FSP_ERR_TIMEOUT 表示总线超时；FSP_ERR_INVALID_DATA 表示校验失败。
 */
fsp_err_t dht11_read(dht11_data_t * p_data);

#ifdef __cplusplus
}
#endif

#endif /* DHT11_H_ */
