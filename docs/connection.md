# RA6M5 阶段框架接线说明

## DHT11 温湿度模块

- VCC -> 开发板 3V3
- GND -> 开发板 GND
- DATA -> J23 排针 P313，对应 RA6M5 P313 / `BSP_IO_PORT_03_PIN_13`

DHT11 为单总线通信，代码中会在读取时动态切换 P313 的输入/输出方向。建议 DATA 外接 4.7k-10k 上拉到 3V3；如果模块板已经带上拉，可先直接接入。

## US516P6 语音模块

- VCC -> 开发板 5V
- GND -> 开发板 GND
- US516P6 PB6/RXD -> J23 排针 P805，对应 RA6M5 P805 / SCI5 TXD5
- US516P6 PB7/TXD -> J23 排针 P513，对应 RA6M5 P513 / SCI5 RXD5

US516P6 规格书给出的工作电源范围为 3.6V-5V，使用说明中建议通过 UART 接口 VCC/GND 或板上 5V/GND 供电。请确认模块 UART 输出电平不超过 RA6M5 引脚允许范围；若测得 TXD 为 5V 电平，应加电平转换或分压后再接入 P513。

## 光敏电阻传感器模块

- VCC -> 开发板 3V3
- GND -> 开发板 GND
- DO -> J23 排针 P304，对应 RA6M5 P304 / `BSP_IO_PORT_03_PIN_04`
- AO -> 当前阶段暂不接入

当前版本按资料中的 51 例程处理数字触发脚：DO 为低电平时视为达到阈值触发，OLED 会显示 `L:LOW TRG`；DO 为高电平时显示 `L:HIGH WAIT`。若遮挡前后一直 HIGH，请优先检查 DO/AO 是否接错、GND 是否共地、模块电位器阈值是否调到会翻转的位置。模块请使用 3V3 供电，避免 DO 输出超过 RA6M5 IO 电平；如果后续需要连续光照强度，再把 AO 接到 ADC 引脚并在 RASC 中启用 ADC。


## CAN 板间通信

当前已加入 SkyForge CAN/CANFD 上层协议封装和发送任务，帧 ID 暂定如下：

- 测试帧：标准 ID `0x320`
- 数据帧：标准 ID `0x321`
- 数据区长度：预留 `16` 字节，最后 2 字节为 CRC16-CCITT
- 帧头：`0x53 0x46`，即 ASCII `SF`

数据帧目前只发送 AI 晾晒状态：`DRYING_SUITABLE` 时标志位 bit0 为 1，其余状态为 0；后续温湿度、光照、动作建议等可继续放入预留字节。测试帧携带帧头、协议版本、序号、固定花码和 CRC16，用于和队友同型号板验证链路。

注意：RA6M5 的 CANFD TX/RX 引脚不能直接两板互连，必须外接 CAN 收发器，再通过 CANH/CANL、共地和终端电阻组成总线。当前 RASC 工程还没有生成 CANFD 实例，`drivers/can_comm.c` 的底层发送口会返回 `FSP_ERR_NOT_OPEN`；在 RASC 中启用 CANFD、选择实际 TX/RX 引脚并生成 `g_canfd` 后，再把 `can_comm_transport_send()` 接到 FSP CANFD Write API 即可真正发帧。
## 当前代码边界

当前阶段完成引脚选择、DHT11 读取、光敏 DO 读取、语音模块 UART 引脚空闲电平检查、CAN 上层帧封装和边缘 AI 占位分级。下一步建议在 RASC 中添加 SCI5 UART，再把 `drivers/us516p6.*` 从电平检查扩展为真实串口收发。

## drying_ai_project 移植

drying_ai_project/edge_ai 中导出的 drying_tree_model.* 和 drying_ai_runtime.* 已复制到本工程 edge_ai/model。当前 edge_ai_app_update() 已部署 drying_ai_project 导出的 C 决策树模型。DHT11 有效时使用真实温湿度；DHT11 暂未接通或读取失败时，使用默认 25C/55%RH 继续运行模型，避免 OLED 长期显示 AI:NO DATA。光敏 DO 触发状态会映射到 solar_radiation；风速、雨雪、重量和限位字段暂用默认值，后续接入对应模块后替换这些默认宏即可。

## OLED 显示模块

- VCC -> 开发板 3V3
- GND -> 开发板 GND
- SCL -> RA6M5 P008，对应 `BSP_IO_PORT_00_PIN_08`
- SDA -> RA6M5 P009，对应 `BSP_IO_PORT_00_PIN_09`

OLED 驱动来自 `D:\RASC_pj\soc\GPIO_LED\GPIO_LED\src\oled`，已移植到本工程 `drivers/oled`。当前采用 GPIO 开漏输出模拟 I2C，不占用 FSP IIC 外设，也不与 DHT11 的 P313、光敏 DO 的 P304、US516P6 的 P805/P513 冲突。开机时第一行居中显示 `SkyForge` 并做一次渐显渐隐，随后直接进入状态界面。