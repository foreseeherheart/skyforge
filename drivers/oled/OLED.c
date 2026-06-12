/*
 * 模块名称：0.96 寸 I2C OLED 显示屏，SSD1306 指令兼容
 * 使用接口：软件模拟 I2C + 电源
 * 引脚对应：OLED VCC -> RA6M5 开发板 3V3
 * 引脚对应：OLED GND -> RA6M5 开发板 GND
 * 引脚对应：OLED SCL -> RA6M5 P008 -> BSP_IO_PORT_00_PIN_08
 * 引脚对应：OLED SDA -> RA6M5 P009 -> BSP_IO_PORT_00_PIN_09
 * 备注：当前使用 GPIO 开漏输出模拟 I2C，未占用 FSP IIC 外设。
 */
#include "OLED.h"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif
#include "OLED_Font.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include "soc_board.h"

/* 说明：软件 I2C 每次拉高/拉低后的延时，单位微秒。 */
#define OLED_I2C_DELAY_US            (2U)

/* 说明：SSD1306 OLED 7 位地址 0x3C 左移 1 位后的写地址。 */
#define OLED_I2C_WRITE_ADDRESS       (0x78U)

/* 说明：OLED 写命令控制字。 */
#define OLED_CONTROL_BYTE_COMMAND    (0x00U)

/* 说明：OLED 写数据控制字。 */
#define OLED_CONTROL_BYTE_DATA       (0x40U)

/* 说明：OLED 每页列数，对应 128 像素宽度。 */
#define OLED_COLUMN_COUNT            (128U)

/* 说明：OLED 页数，对应 64 像素高度，每页 8 行像素。 */
#define OLED_PAGE_COUNT              (8U)

static void OLED_I2C_Delay(void);
static void OLED_W_SDA(uint8_t x);
static void OLED_W_SCL(uint8_t x);
static void OLED_I2C_Start(void);
static void OLED_I2C_Stop(void);
static void OLED_I2C_SendByte(uint8_t byte);
static void OLED_WriteCommand(uint8_t command);
static void OLED_WriteData(uint8_t data);
static void OLED_SetCursor(uint8_t y, uint8_t x);
static uint32_t OLED_Pow(uint32_t x, uint32_t y);

static void OLED_I2C_Delay(void)
{
    R_BSP_SoftwareDelay(OLED_I2C_DELAY_US, BSP_DELAY_UNITS_MICROSECONDS);
}

/**
 * 名称：OLED_W_SDA
 * 作用：写 OLED 软件 I2C 的 SDA 电平。
 * 接受值：x 为 0 时拉低，非 0 时释放为高电平。
 * 返回值：无。
 */
static void OLED_W_SDA(uint8_t x)
{
    g_ioport.p_api->pinWrite(g_ioport.p_ctrl,
                             SOC_BOARD_OLED_SDA_PIN,
                             (0U == x) ? BSP_IO_LEVEL_LOW : BSP_IO_LEVEL_HIGH);
    OLED_I2C_Delay();
}

/**
 * 名称：OLED_W_SCL
 * 作用：写 OLED 软件 I2C 的 SCL 电平。
 * 接受值：x 为 0 时拉低，非 0 时释放为高电平。
 * 返回值：无。
 */
static void OLED_W_SCL(uint8_t x)
{
    g_ioport.p_api->pinWrite(g_ioport.p_ctrl,
                             SOC_BOARD_OLED_SCL_PIN,
                             (0U == x) ? BSP_IO_LEVEL_LOW : BSP_IO_LEVEL_HIGH);
    OLED_I2C_Delay();
}

/**
 * 名称：OLED_I2C_Start
 * 作用：产生软件 I2C 起始信号。
 * 接受值：无。
 * 返回值：无。
 */
static void OLED_I2C_Start(void)
{
    OLED_W_SDA(1U);
    OLED_W_SCL(1U);
    OLED_W_SDA(0U);
    OLED_W_SCL(0U);
}

/**
 * 名称：OLED_I2C_Stop
 * 作用：产生软件 I2C 停止信号。
 * 接受值：无。
 * 返回值：无。
 */
static void OLED_I2C_Stop(void)
{
    OLED_W_SDA(0U);
    OLED_W_SCL(1U);
    OLED_W_SDA(1U);
}

/**
 * 名称：OLED_I2C_SendByte
 * 作用：通过软件 I2C 向 OLED 发送 1 字节数据，当前不读取 ACK。
 * 接受值：byte 为待发送字节。
 * 返回值：无。
 */
static void OLED_I2C_SendByte(uint8_t byte)
{
    for (uint8_t i = 0U; i < 8U; i++)
    {
        OLED_W_SDA((uint8_t) (byte & (0x80U >> i)));
        OLED_W_SCL(1U);
        OLED_W_SCL(0U);
    }

    OLED_W_SCL(1U);
    OLED_W_SCL(0U);
}

/**
 * 名称：OLED_WriteCommand
 * 作用：向 OLED 控制器写入 1 字节命令。
 * 接受值：command 为 SSD1306 命令字节。
 * 返回值：无。
 */
static void OLED_WriteCommand(uint8_t command)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(OLED_I2C_WRITE_ADDRESS);
    OLED_I2C_SendByte(OLED_CONTROL_BYTE_COMMAND);
    OLED_I2C_SendByte(command);
    OLED_I2C_Stop();
}

/**
 * 名称：OLED_WriteData
 * 作用：向 OLED 显存写入 1 字节显示数据。
 * 接受值：data 为待写入显示数据。
 * 返回值：无。
 */
static void OLED_WriteData(uint8_t data)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(OLED_I2C_WRITE_ADDRESS);
    OLED_I2C_SendByte(OLED_CONTROL_BYTE_DATA);
    OLED_I2C_SendByte(data);
    OLED_I2C_Stop();
}

/**
 * 名称：OLED_SetCursor
 * 作用：设置 OLED 显存写入位置。
 * 接受值：y 为页坐标 0-7；x 为列坐标 0-127。
 * 返回值：无。
 */
static void OLED_SetCursor(uint8_t y, uint8_t x)
{
    OLED_WriteCommand((uint8_t) (0xB0U | y));
    OLED_WriteCommand((uint8_t) (0x10U | ((x & 0xF0U) >> 4U)));
    OLED_WriteCommand((uint8_t) (0x00U | (x & 0x0FU)));
}

/**
 * 名称：OLED_Clear
 * 作用：清空 OLED 全屏显示内容。
 * 接受值：无。
 * 返回值：无。
 */
void OLED_Clear(void)
{
    for (uint8_t page = 0U; page < OLED_PAGE_COUNT; page++)
    {
        OLED_SetCursor(page, 0U);
        for (uint8_t column = 0U; column < OLED_COLUMN_COUNT; column++)
        {
            OLED_WriteData(0x00U);
        }
    }
}

/**
 * 名称：OLED_ShowChar
 * 作用：显示一个 8x16 ASCII 字符。
 * 接受值：Line 为 1-4 行；Column 为 1-16 列；Char 为 ASCII 可见字符。
 * 返回值：无。
 */

/**
 * 名称：OLED_SetContrast
 * 作用：设置 OLED 显示对比度，用于启动时的一次性渐显渐隐效果。
 * 接受值：contrast 为 0-255 的对比度值。
 * 返回值：无。
 */
void OLED_SetContrast(uint8_t contrast)
{
    OLED_WriteCommand(0x81U);
    OLED_WriteCommand(contrast);
}
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
    uint8_t char_index = (uint8_t) (Char - ' ');

    OLED_SetCursor((uint8_t) ((Line - 1U) * 2U), (uint8_t) ((Column - 1U) * 8U));
    for (uint8_t i = 0U; i < 8U; i++)
    {
        OLED_WriteData(OLED_F8x16[char_index][i]);
    }

    OLED_SetCursor((uint8_t) ((Line - 1U) * 2U + 1U), (uint8_t) ((Column - 1U) * 8U));
    for (uint8_t i = 0U; i < 8U; i++)
    {
        OLED_WriteData(OLED_F8x16[char_index][i + 8U]);
    }
}

/**
 * 名称：OLED_ShowString
 * 作用：从指定位置开始显示 ASCII 字符串。
 * 接受值：Line 为 1-4 行；Column 为 1-16 列；String 为以 '\0' 结尾的字符串。
 * 返回值：无。
 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char const * String)
{
    for (uint8_t i = 0U; String[i] != '\0'; i++)
    {
        OLED_ShowChar(Line, (uint8_t) (Column + i), String[i]);
    }
}

static uint32_t OLED_Pow(uint32_t x, uint32_t y)
{
    uint32_t result = 1U;
    while (y--)
    {
        result *= x;
    }

    return result;
}

void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    for (uint8_t i = 0U; i < Length; i++)
    {
        OLED_ShowChar(Line, (uint8_t) (Column + i), (char) (Number / OLED_Pow(10U, Length - i - 1U) % 10U + '0'));
    }
}

void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
    uint32_t number_abs;
    if (Number >= 0)
    {
        OLED_ShowChar(Line, Column, '+');
        number_abs = (uint32_t) Number;
    }
    else
    {
        OLED_ShowChar(Line, Column, '-');
        number_abs = (uint32_t) -Number;
    }

    for (uint8_t i = 0U; i < Length; i++)
    {
        OLED_ShowChar(Line,
                      (uint8_t) (Column + i + 1U),
                      (char) (number_abs / OLED_Pow(10U, Length - i - 1U) % 10U + '0'));
    }
}

void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    for (uint8_t i = 0U; i < Length; i++)
    {
        uint8_t single_number = (uint8_t) (Number / OLED_Pow(16U, Length - i - 1U) % 16U);
        if (single_number < 10U)
        {
            OLED_ShowChar(Line, (uint8_t) (Column + i), (char) (single_number + '0'));
        }
        else
        {
            OLED_ShowChar(Line, (uint8_t) (Column + i), (char) (single_number - 10U + 'A'));
        }
    }
}

void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    for (uint8_t i = 0U; i < Length; i++)
    {
        OLED_ShowChar(Line, (uint8_t) (Column + i), (char) (Number / OLED_Pow(2U, Length - i - 1U) % 2U + '0'));
    }
}

/**
 * 名称：OLED_Init
 * 作用：初始化 OLED 控制器并清屏。
 * 接受值：无。
 * 返回值：无。
 */
void OLED_Init(void)
{
    R_BSP_SoftwareDelay(100U, BSP_DELAY_UNITS_MILLISECONDS);

    OLED_WriteCommand(0xAEU);
    OLED_WriteCommand(0xD5U);
    OLED_WriteCommand(0x80U);
    OLED_WriteCommand(0xA8U);
    OLED_WriteCommand(0x3FU);
    OLED_WriteCommand(0xD3U);
    OLED_WriteCommand(0x00U);
    OLED_WriteCommand(0x40U);
    OLED_WriteCommand(0xA1U);
    OLED_WriteCommand(0xC8U);
    OLED_WriteCommand(0xDAU);
    OLED_WriteCommand(0x12U);
    OLED_WriteCommand(0x81U);
    OLED_WriteCommand(0xCFU);
    OLED_WriteCommand(0xD9U);
    OLED_WriteCommand(0xF1U);
    OLED_WriteCommand(0xDBU);
    OLED_WriteCommand(0x30U);
    OLED_WriteCommand(0xA4U);
    OLED_WriteCommand(0xA6U);
    OLED_WriteCommand(0x8DU);
    OLED_WriteCommand(0x14U);
    OLED_WriteCommand(0xAFU);

    OLED_Clear();
}