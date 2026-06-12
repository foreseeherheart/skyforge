#include "soc_board.h"

/**
 * 名称：soc_board_init
 * 作用：统一配置当前阶段外接模块所需的 RA6M5 GPIO 引脚。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示配置完成，其它值表示底层 IOPORT 配置失败。
 */
fsp_err_t soc_board_init(void)
{
    fsp_err_t err = g_ioport.p_api->pinCfg(g_ioport.p_ctrl,
                                           SOC_BOARD_DHT11_DATA_PIN,
                                           SOC_BOARD_DHT11_DATA_CFG);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = g_ioport.p_api->pinCfg(g_ioport.p_ctrl,
                                 SOC_BOARD_US516P6_RA_TX_PIN,
                                 SOC_BOARD_US516P6_TX_CFG);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = g_ioport.p_api->pinCfg(g_ioport.p_ctrl,
                                 SOC_BOARD_US516P6_RA_RX_PIN,
                                 SOC_BOARD_US516P6_RX_CFG);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = g_ioport.p_api->pinCfg(g_ioport.p_ctrl,
                                 SOC_BOARD_OLED_SCL_PIN,
                                 SOC_BOARD_OLED_I2C_CFG);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = g_ioport.p_api->pinCfg(g_ioport.p_ctrl,
                                 SOC_BOARD_OLED_SDA_PIN,
                                 SOC_BOARD_OLED_I2C_CFG);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return g_ioport.p_api->pinCfg(g_ioport.p_ctrl,
                                  SOC_BOARD_LIGHT_SENSOR_DO_PIN,
                                  SOC_BOARD_LIGHT_SENSOR_DO_CFG);
}