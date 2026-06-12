#include "sensor_task.h"
#include <string.h>

/**
 * 名称：sensor_task_init
 * 作用：初始化当前阶段已接入的湿度模块、语音模块和光敏电阻模块连接。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示初始化成功，其它值表示某个模块初始化失败。
 */
fsp_err_t sensor_task_init(void)
{
    fsp_err_t err = dht11_init();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = us516p6_init();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return light_sensor_init();
}

/**
 * 名称：sensor_task_poll
 * 作用：轮询外接模块状态，当前读取 DHT11、光敏 DO 并检查语音模块串口空闲电平。
 * 接受值：p_status 指向状态输出结构体。
 * 返回值：FSP_SUCCESS 表示轮询完成；FSP_ERR_INVALID_ARGUMENT 表示参数无效。
 */
fsp_err_t sensor_task_poll(sensor_task_status_t * p_status)
{
    if (NULL == p_status)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    memset(p_status, 0, sizeof(*p_status));
    p_status->dht11_status = dht11_read(&p_status->dht11_data);
    p_status->light_sensor_status = light_sensor_read(&p_status->light_sensor_data);

    fsp_err_t err = us516p6_get_link_state(&p_status->voice_link_state);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return (FSP_SUCCESS == p_status->light_sensor_status) ? FSP_SUCCESS : p_status->light_sensor_status;
}
