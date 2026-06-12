/**
 * 模块名称：光敏电阻传感器模块
 * 引脚对应：光敏模块 VCC -> RA6M5 开发板 3V3
 * 引脚对应：光敏模块 GND -> RA6M5 开发板 GND
 * 引脚对应：光敏模块 DO  -> J23 排针 P304 -> BSP_IO_PORT_03_PIN_04
 * 说明：当前阶段只接数字输出 DO；AO 模拟量暂不接入，后续需要连续光照值时再启用 ADC。
 */
#ifndef LIGHT_SENSOR_H_
#define LIGHT_SENSOR_H_

#include "hal_data.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 说明：光敏模块 DO 数字输出状态，低电平按 51 例程视为触发，高电平视为未触发。 */
typedef enum e_light_sensor_state
{
    LIGHT_SENSOR_STATE_UNKNOWN = 0,
    LIGHT_SENSOR_STATE_TRIGGERED_LOW,
    LIGHT_SENSOR_STATE_RELEASED_HIGH,
} light_sensor_state_t;

/* 说明：光敏模块一次读取结果，保留原始电平，便于现场调节电位器时观察。 */
typedef struct st_light_sensor_data
{
    light_sensor_state_t state;
    bsp_io_level_t raw_level;
    bool threshold_reached;
} light_sensor_data_t;

/**
 * 名称：light_sensor_init
 * 作用：初始化光敏电阻传感器数字输出读取逻辑。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示初始化完成。
 */
fsp_err_t light_sensor_init(void);

/**
 * 名称：light_sensor_read
 * 作用：读取光敏电阻传感器 DO 引脚电平，并转换为触发状态。
 * 接受值：p_data 指向光敏传感器状态输出结构体。
 * 返回值：FSP_SUCCESS 表示读取完成；FSP_ERR_INVALID_ARGUMENT 表示参数无效；其它值表示 GPIO 读取失败。
 */
fsp_err_t light_sensor_read(light_sensor_data_t * p_data);

#ifdef __cplusplus
}
#endif

#endif /* LIGHT_SENSOR_H_ */