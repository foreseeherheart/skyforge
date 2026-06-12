/**
 * 模块名称：光敏电阻传感器模块
 * 引脚对应：光敏模块 VCC -> RA6M5 开发板 3V3
 * 引脚对应：光敏模块 GND -> RA6M5 开发板 GND
 * 引脚对应：光敏模块 DO  -> J23 排针 P304 -> BSP_IO_PORT_03_PIN_04
 * 说明：当前阶段只接数字输出 DO；AO 模拟量暂不接入，后续需要连续光照值时再启用 ADC。
 */
#include "light_sensor.h"
#include "soc_board.h"
#include <string.h>

/**
 * 名称：light_sensor_init
 * 作用：初始化光敏电阻传感器数字输出读取逻辑。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示初始化完成。
 */
fsp_err_t light_sensor_init(void)
{
    return FSP_SUCCESS;
}

/**
 * 名称：light_sensor_read
 * 作用：读取光敏电阻传感器 DO 引脚电平，并转换为触发状态。
 * 接受值：p_data 指向光敏传感器状态输出结构体。
 * 返回值：FSP_SUCCESS 表示读取完成；FSP_ERR_INVALID_ARGUMENT 表示参数无效；其它值表示 GPIO 读取失败。
 */
fsp_err_t light_sensor_read(light_sensor_data_t * p_data)
{
    if (NULL == p_data)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    memset(p_data, 0, sizeof(*p_data));

    fsp_err_t err = g_ioport.p_api->pinRead(g_ioport.p_ctrl,
                                            SOC_BOARD_LIGHT_SENSOR_DO_PIN,
                                            &p_data->raw_level);
    if (FSP_SUCCESS != err)
    {
        p_data->state = LIGHT_SENSOR_STATE_UNKNOWN;
        return err;
    }

    if (BSP_IO_LEVEL_LOW == p_data->raw_level)
    {
        p_data->state = LIGHT_SENSOR_STATE_TRIGGERED_LOW;
        p_data->threshold_reached = true;
    }
    else
    {
        p_data->state = LIGHT_SENSOR_STATE_RELEASED_HIGH;
        p_data->threshold_reached = false;
    }

    return FSP_SUCCESS;
}