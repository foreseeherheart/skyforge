#ifndef SENSOR_TASK_H_
#define SENSOR_TASK_H_

#include "hal_data.h"
#include "dht11.h"
#include "us516p6.h"
#include "light_sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_sensor_task_status
{
    fsp_err_t dht11_status;
    dht11_data_t dht11_data;
    us516p6_link_state_t voice_link_state;
    fsp_err_t light_sensor_status;
    light_sensor_data_t light_sensor_data;
} sensor_task_status_t;

/**
 * 名称：sensor_task_init
 * 作用：初始化当前阶段已接入的湿度模块、语音模块和光敏电阻模块连接。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示初始化成功，其它值表示某个模块初始化失败。
 */
fsp_err_t sensor_task_init(void);

/**
 * 名称：sensor_task_poll
 * 作用：轮询外接模块状态，当前读取 DHT11、光敏 DO 并检查语音模块串口空闲电平。
 * 接受值：p_status 指向状态输出结构体。
 * 返回值：FSP_SUCCESS 表示轮询完成；FSP_ERR_INVALID_ARGUMENT 表示参数无效。
 */
fsp_err_t sensor_task_poll(sensor_task_status_t * p_status);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_TASK_H_ */
