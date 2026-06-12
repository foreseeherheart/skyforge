#include "display_task.h"
#include "OLED.h"
#include <string.h>

/* 说明：OLED 每行最多显示 16 个 8x16 ASCII 字符。 */
#define DISPLAY_TASK_MAX_COLUMNS          (16U)

/* 说明：SkyForge 字符数，用于计算第一行居中显示起始列。 */
#define DISPLAY_TASK_BOOT_TEXT_COLUMNS    (8U)

/* 说明：第一行居中显示 SkyForge 的列号，OLED 字符列从 1 开始。 */
#define DISPLAY_TASK_BOOT_TEXT_COLUMN     (((DISPLAY_TASK_MAX_COLUMNS - DISPLAY_TASK_BOOT_TEXT_COLUMNS) / 2U) + 1U)

/* 说明：启动渐显渐隐每一步之间的等待时间。 */
#define DISPLAY_TASK_FADE_STEP_DELAY_MS   (35U)

/* 说明：SkyForge 达到最高亮度后的停留时间，便于开机时看清队名。 */
#define DISPLAY_TASK_BOOT_HOLD_MS         (1200U)

/* 说明：启动渐显渐隐的对比度步进值，数值越大动画越短。 */
#define DISPLAY_TASK_FADE_CONTRAST_STEP   (32U)

static void display_task_show_boot_logo(void);
static void display_task_show_line(uint8_t line, char const * text);
static void display_task_fill_line(char * line_text);
static void display_task_put_text(char * line_text, uint8_t column, char const * text);
static void display_task_put_two_digits(char * line_text, uint8_t column, uint8_t value);

/**
 * 名称：display_task_init
 * 作用：初始化 OLED 显示任务，第一行居中显示 SkyForge 并完成一次渐显渐隐。
 * 接受值：无。
 * 返回值：FSP_SUCCESS 表示初始化完成。
 */
fsp_err_t display_task_init(void)
{
    OLED_Init();
    display_task_show_boot_logo();
    OLED_Clear();
    OLED_SetContrast(0xFFU);

    return FSP_SUCCESS;
}

/**
 * 名称：display_task_show_status
 * 作用：将温湿度、光敏电阻、语音模块连接状态和边缘 AI 输出刷新到 OLED。
 * 接受值：p_sensor_status 为传感器状态；p_ai_result 为边缘 AI 结果。
 * 返回值：无。
 */
void display_task_show_status(sensor_task_status_t const * p_sensor_status,
                              edge_ai_app_result_t const * p_ai_result)
{
    char line_text[DISPLAY_TASK_MAX_COLUMNS + 1U];

    display_task_fill_line(line_text);
    if ((NULL != p_sensor_status) && (FSP_SUCCESS == p_sensor_status->dht11_status))
    {
        display_task_put_text(line_text, 1U, "T:");
        display_task_put_two_digits(line_text, 3U, p_sensor_status->dht11_data.temperature_integer);
        display_task_put_text(line_text, 5U, "C H:");
        display_task_put_two_digits(line_text, 9U, p_sensor_status->dht11_data.humidity_integer);
        display_task_put_text(line_text, 11U, "%");
    }
    else
    {
        display_task_put_text(line_text, 1U, "DHT11 ERR");
    }
    display_task_show_line(1U, line_text);

    display_task_fill_line(line_text);
    display_task_put_text(line_text, 1U, "L:");
    if ((NULL != p_sensor_status) && (FSP_SUCCESS == p_sensor_status->light_sensor_status))
    {
        if (BSP_IO_LEVEL_LOW == p_sensor_status->light_sensor_data.raw_level)
        {
            display_task_put_text(line_text, 3U, "LOW  TRG");
        }
        else
        {
            display_task_put_text(line_text, 3U, "HIGH WAIT");
        }
    }
    else
    {
        display_task_put_text(line_text, 3U, "ERR");
    }
    display_task_show_line(2U, line_text);

    display_task_fill_line(line_text);
    display_task_put_text(line_text, 1U, "VOICE:");
    if ((NULL != p_sensor_status) && (US516P6_LINK_IDLE_HIGH == p_sensor_status->voice_link_state))
    {
        display_task_put_text(line_text, 7U, "IDLE");
    }
    else if ((NULL != p_sensor_status) && (US516P6_LINK_BUSY_LOW == p_sensor_status->voice_link_state))
    {
        display_task_put_text(line_text, 7U, "LOW");
    }
    else
    {
        display_task_put_text(line_text, 7U, "UNKNOWN");
    }
    display_task_show_line(3U, line_text);

    display_task_fill_line(line_text);
    if ((NULL != p_ai_result) && p_ai_result->input_valid)
    {
        display_task_put_text(line_text, 1U, "AI:");
        display_task_put_text(line_text, 4U, p_ai_result->output.state_text);
    }
    else
    {
        display_task_put_text(line_text, 1U, "AI:WAIT");
    }
    display_task_show_line(4U, line_text);
}

/**
 * 名称：display_task_show_boot_logo
 * 作用：第一行居中显示 SkyForge，并执行一次渐显、停留、渐隐启动动画。
 * 接受值：无。
 * 返回值：无。
 */
static void display_task_show_boot_logo(void)
{
    OLED_Clear();
    OLED_SetContrast(0x00U);
    OLED_ShowString(1U, DISPLAY_TASK_BOOT_TEXT_COLUMN, "SkyForge");

    for (uint16_t contrast = 0U; contrast <= 0xFFU; contrast += DISPLAY_TASK_FADE_CONTRAST_STEP)
    {
        OLED_SetContrast((uint8_t) contrast);
        R_BSP_SoftwareDelay(DISPLAY_TASK_FADE_STEP_DELAY_MS, BSP_DELAY_UNITS_MILLISECONDS);
    }

    OLED_SetContrast(0xFFU);
    R_BSP_SoftwareDelay(DISPLAY_TASK_BOOT_HOLD_MS, BSP_DELAY_UNITS_MILLISECONDS);

    for (int16_t contrast = 0xFF; contrast >= 0; contrast -= (int16_t) DISPLAY_TASK_FADE_CONTRAST_STEP)
    {
        OLED_SetContrast((uint8_t) contrast);
        R_BSP_SoftwareDelay(DISPLAY_TASK_FADE_STEP_DELAY_MS, BSP_DELAY_UNITS_MILLISECONDS);
    }
}

/**
 * 名称：display_task_show_line
 * 作用：覆盖写 OLED 指定行的 16 个字符，避免每次刷新先整屏清空造成闪烁和慢刷感。
 * 接受值：line 为显示行；text 为待显示字符串，超过 16 字符自动截断，不足补空格。
 * 返回值：无。
 */
static void display_task_show_line(uint8_t line, char const * text)
{
    for (uint8_t i = 0U; i < DISPLAY_TASK_MAX_COLUMNS; i++)
    {
        char ch = ((NULL != text) && ('\0' != text[i])) ? text[i] : ' ';
        OLED_ShowChar(line, (uint8_t) (i + 1U), ch);
    }
}

/**
 * 名称：display_task_fill_line
 * 作用：将状态行缓存填充为空格并补字符串结束符。
 * 接受值：line_text 为长度至少 17 字节的字符缓存。
 * 返回值：无。
 */
static void display_task_fill_line(char * line_text)
{
    memset(line_text, ' ', DISPLAY_TASK_MAX_COLUMNS);
    line_text[DISPLAY_TASK_MAX_COLUMNS] = '\0';
}

/**
 * 名称：display_task_put_text
 * 作用：把字符串写入状态行缓存，超过 OLED 行尾时自动截断。
 * 接受值：line_text 为状态行缓存；column 为 1-16 起始列；text 为待写入字符串。
 * 返回值：无。
 */
static void display_task_put_text(char * line_text, uint8_t column, char const * text)
{
    if ((NULL == line_text) || (NULL == text) || (0U == column) || (column > DISPLAY_TASK_MAX_COLUMNS))
    {
        return;
    }

    uint8_t index = (uint8_t) (column - 1U);
    for (uint8_t i = 0U; ('\0' != text[i]) && ((index + i) < DISPLAY_TASK_MAX_COLUMNS); i++)
    {
        line_text[index + i] = text[i];
    }
}

/**
 * 名称：display_task_put_two_digits
 * 作用：把 0-99 的数值以两位十进制写入状态行缓存。
 * 接受值：line_text 为状态行缓存；column 为 1-16 起始列；value 为待显示数值。
 * 返回值：无。
 */
static void display_task_put_two_digits(char * line_text, uint8_t column, uint8_t value)
{
    if ((NULL == line_text) || (0U == column) || ((column + 1U) > DISPLAY_TASK_MAX_COLUMNS))
    {
        return;
    }

    if (value > 99U)
    {
        value = 99U;
    }

    line_text[column - 1U] = (char) ('0' + (value / 10U));
    line_text[column] = (char) ('0' + (value % 10U));
}