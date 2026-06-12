/*
 * 模块名称：DHT11 温湿度模块
 * 使用接口：单总线 DATA + 电源
 * 引脚对应：DHT11 VCC  -> RA6M5 开发板 3V3
 * 引脚对应：DHT11 GND  -> RA6M5 开发板 GND
 * 引脚对应：DHT11 DATA -> J23 排针 P313 -> BSP_IO_PORT_03_PIN_13
 * 备注：DATA 建议上拉到 3V3，若模块板已带上拉电阻可直接连接。
 */
#include "dht11.h"
#include "soc_board.h"
#include <string.h>

/* 说明：等待 DHT11 响应电平变化的超时时间，单位微秒。 */
#define DHT11_RESPONSE_TIMEOUT_US    (100U)

/* 说明：等待 DHT11 单个数据位电平变化的超时时间，单位微秒。 */
#define DHT11_BIT_TIMEOUT_US         (120U)

/* 说明：主机拉低总线启动 DHT11 采样的保持时间，单位毫秒。 */
#define DHT11_START_LOW_MS           (20U)

/* 说明：主机释放总线后等待 DHT11 响应的准备时间，单位微秒。 */
#define DHT11_START_RELEASE_US       (30U)

/* 说明：DHT11 高电平持续时间大于该阈值时判定数据位为 1，单位微秒。 */
#define DHT11_BIT_ONE_THRESHOLD_US   (40U)

/* 说明：DHT11 一帧数据包含湿度、温度和校验和，共 5 字节。 */
#define DHT11_FRAME_BYTES            (5U)

/* 说明：DHT11 一帧数据总位数。 */
#define DHT11_FRAME_BITS             (40U)

static fsp_err_t dht11_drive_low(void);
static fsp_err_t dht11_release_bus(void);
static fsp_err_t dht11_wait_level(bsp_io_level_t level, uint32_t timeout_us, uint32_t * p_elapsed_us);

/**
 * 名称：dht11_init
 * 作用：初始化 DHT11 单总线引脚为空闲上拉输入状态。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示初始化成功，其它值表示引脚配置失败。
 */
fsp_err_t dht11_init(void)
{
    return dht11_release_bus();
}

/**
 * 名称：dht11_read
 * 作用：读取一次 DHT11 温湿度数据，并完成校验和检查。
 * 接受值：p_data 指向用于接收温湿度结果的结构体。
 * 返回值：FSP_SUCCESS 表示读取成功；FSP_ERR_INVALID_ARGUMENT 表示参数无效；FSP_ERR_TIMEOUT 表示总线超时；FSP_ERR_INVALID_DATA 表示校验失败。
 */
fsp_err_t dht11_read(dht11_data_t * p_data)
{
    if (NULL == p_data)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* 说明：保存 DHT11 原始 5 字节数据帧。 */
    uint8_t frame[DHT11_FRAME_BYTES] = {0};

    /* 说明：记录数据位高电平持续时间，用于判断当前位为 0 还是 1。 */
    uint32_t high_time_us = 0U;

    fsp_err_t err = dht11_drive_low();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    R_BSP_SoftwareDelay(DHT11_START_LOW_MS, BSP_DELAY_UNITS_MILLISECONDS);

    err = dht11_release_bus();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    R_BSP_SoftwareDelay(DHT11_START_RELEASE_US, BSP_DELAY_UNITS_MICROSECONDS);

    err = dht11_wait_level(BSP_IO_LEVEL_LOW, DHT11_RESPONSE_TIMEOUT_US, NULL);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = dht11_wait_level(BSP_IO_LEVEL_HIGH, DHT11_RESPONSE_TIMEOUT_US, NULL);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = dht11_wait_level(BSP_IO_LEVEL_LOW, DHT11_RESPONSE_TIMEOUT_US, NULL);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    for (uint32_t bit_index = 0U; bit_index < DHT11_FRAME_BITS; bit_index++)
    {
        err = dht11_wait_level(BSP_IO_LEVEL_HIGH, DHT11_BIT_TIMEOUT_US, NULL);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        err = dht11_wait_level(BSP_IO_LEVEL_LOW, DHT11_BIT_TIMEOUT_US, &high_time_us);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        frame[bit_index / 8U] <<= 1U;
        if (high_time_us > DHT11_BIT_ONE_THRESHOLD_US)
        {
            frame[bit_index / 8U] |= 1U;
        }
    }

    /* 说明：DHT11 校验和等于前 4 个数据字节的低 8 位累加和。 */
    uint8_t checksum = (uint8_t) (frame[0] + frame[1] + frame[2] + frame[3]);
    if (checksum != frame[4])
    {
        memset(p_data, 0, sizeof(*p_data));
        return FSP_ERR_INVALID_DATA;
    }

    p_data->humidity_integer = frame[0];
    p_data->humidity_decimal = frame[1];
    p_data->temperature_integer = frame[2];
    p_data->temperature_decimal = frame[3];

    return FSP_SUCCESS;
}

/**
 * 名称：dht11_drive_low
 * 作用：将 DHT11 单总线配置为输出低电平，用于发送起始信号。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示配置成功，其它值表示 IOPORT 操作失败。
 */
static fsp_err_t dht11_drive_low(void)
{
    fsp_err_t err = g_ioport.p_api->pinCfg(g_ioport.p_ctrl,
                                           SOC_BOARD_DHT11_DATA_PIN,
                                           IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_LOW);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return g_ioport.p_api->pinWrite(g_ioport.p_ctrl, SOC_BOARD_DHT11_DATA_PIN, BSP_IO_LEVEL_LOW);
}

/**
 * 名称：dht11_release_bus
 * 作用：释放 DHT11 单总线并恢复为输入上拉状态。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示配置成功，其它值表示 IOPORT 操作失败。
 */
static fsp_err_t dht11_release_bus(void)
{
    return g_ioport.p_api->pinCfg(g_ioport.p_ctrl,
                                  SOC_BOARD_DHT11_DATA_PIN,
                                  SOC_BOARD_DHT11_DATA_CFG);
}

/**
 * 名称：dht11_wait_level
 * 作用：等待 DHT11 单总线变为指定电平，并可返回等待耗时。
 * 接受值：level 为目标电平；timeout_us 为超时时间；p_elapsed_us 可选输出等待时间。
 * 返回值：FSP_SUCCESS 表示等到目标电平；FSP_ERR_TIMEOUT 表示超时；其它值表示 IOPORT 读取失败。
 */
static fsp_err_t dht11_wait_level(bsp_io_level_t level, uint32_t timeout_us, uint32_t * p_elapsed_us)
{
    /* 说明：保存当前读取到的 DHT11 DATA 电平。 */
    bsp_io_level_t current_level = BSP_IO_LEVEL_LOW;

    /* 说明：累计等待时间，单位微秒。 */
    uint32_t elapsed_us = 0U;

    while (elapsed_us < timeout_us)
    {
        fsp_err_t err = g_ioport.p_api->pinRead(g_ioport.p_ctrl, SOC_BOARD_DHT11_DATA_PIN, &current_level);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        if (current_level == level)
        {
            if (NULL != p_elapsed_us)
            {
                *p_elapsed_us = elapsed_us;
            }

            return FSP_SUCCESS;
        }

        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MICROSECONDS);
        elapsed_us++;
    }

    return FSP_ERR_TIMEOUT;
}