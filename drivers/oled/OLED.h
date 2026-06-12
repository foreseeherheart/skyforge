/*
 * 模块名称：0.96 寸 I2C OLED 显示屏，SSD1306 指令兼容
 * 使用接口：软件模拟 I2C + 电源
 * 引脚对应：OLED VCC -> RA6M5 开发板 3V3
 * 引脚对应：OLED GND -> RA6M5 开发板 GND
 * 引脚对应：OLED SCL -> RA6M5 P008 -> BSP_IO_PORT_00_PIN_08
 * 引脚对应：OLED SDA -> RA6M5 P009 -> BSP_IO_PORT_00_PIN_09
 * 备注：当前使用 GPIO 开漏输出模拟 I2C，未占用 FSP IIC 外设。
 */
#ifndef OLED_H_
#define OLED_H_

#include "hal_data.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 名称：OLED_Init
 * 作用：初始化 OLED 控制器并清屏。
 * 接受值：无。
 * 返回值：无。
 */
void OLED_Init(void);

/**
 * 名称：OLED_SetContrast
 * 作用：设置 OLED 显示对比度，用于启动时的一次性渐显渐隐效果。
 * 接受值：contrast 为 0-255 的对比度值。
 * 返回值：无。
 */
void OLED_SetContrast(uint8_t contrast);

/**
 * 名称：OLED_Clear
 * 作用：清空 OLED 全屏显示内容。
 * 接受值：无。
 * 返回值：无。
 */
void OLED_Clear(void);

void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char const * String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

#ifdef __cplusplus
}
#endif

#endif /* OLED_H_ */