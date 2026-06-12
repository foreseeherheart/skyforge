# EBF_RA6M5

当前代码主入口为 `src/hal_entry.c`，实际应用入口为 `application/application_entry.c`。系统无 RTOS，采用主循环轮询方式运行。

## 工程信息

- MCU：Renesas RA6M5 `R7FA6M5BH3CFC`
- 封装：LQFP-176
- Flash：2 MB
- SRAM：512 KB
- Data Flash：8 KB
- FSP/RASC：`C:\Renesas\RA\sc_v2025-12_fsp_v6.4.0\eclipse\rascc.exe`
- Keil 编译器：ARMCLANG V6.10.1
- 当前 FSP 实例：`g_ioport` / I/O Port
- 当前未生成的外设实例：SCI5 UART、CANFD

## 目录结构

```text
EBF_RA6M5/
├── application/              # 应用层入口，组织初始化与主循环
│   ├── application_entry.c
│   └── application_entry.h
├── bsp/                      # 当前阶段板级引脚抽象与 GPIO 初始化
│   ├── soc_board.c
│   └── soc_board.h
├── docs/                     # 接线说明等补充文档
│   └── connection.md
├── drivers/                  # 外设驱动与协议封装
│   ├── dht11.c/.h            # DHT11 单总线温湿度读取
│   ├── light_sensor.c/.h     # 光敏电阻模块 DO 读取
│   ├── us516p6.c/.h          # US516P6 语音模块 UART 引脚状态检查
│   ├── can_comm.c/.h         # SkyForge CAN/CANFD 协议封装
│   └── oled/                 # OLED GPIO 模拟 I2C 驱动
├── edge_ai/                  # 晾晒场景边缘 AI 应用
│   ├── edge_ai_app.c/.h
│   └── model/
│       ├── drying_ai_runtime.c/.h
│       └── drying_tree_model.c/.h
├── task/                     # 任务层封装
│   ├── sensor_task.c/.h      # 传感器统一初始化与轮询
│   ├── display_task.c/.h     # OLED 显示任务
│   └── can_comm_task.c/.h    # CAN 数据帧/测试帧发送任务
├── src/                      # FSP 调用入口与 warm start
│   ├── hal_entry.c
│   └── hal_warmstart.c
├── ra_gen/                   # RASC/FSP 自动生成代码
│   ├── common_data.c/.h
│   ├── hal_data.c/.h
│   ├── pin_data.c
│   └── vector_data.c/.h
├── ra_cfg/                   # FSP 配置头文件
├── ra/                       # FSP 与 CMSIS 源码
├── Objects/                  # Keil 构建产物与日志
├── Listings/                 # map 等链接输出
├── configuration.xml         # RASC 工程配置
├── EBF_RA6M5.uvprojx         # Keil 工程文件
└── ra_cfg.txt                # FSP 配置导出文本
```

## 引脚和接线

| 模块 | 模块引脚 | 开发板/RA6M5 引脚 | 代码宏 | 当前用途 |
| --- | --- | --- | --- | --- |
| DHT11 温湿度 | VCC | 3V3 | - | 模块供电 |
| DHT11 温湿度 | GND | GND | - | 共地 |
| DHT11 温湿度 | DATA | J23 P313 / RA6M5 P313 | `BSP_IO_PORT_03_PIN_13` | 单总线数据，读取时动态切换输入/输出 |
| US516P6 语音模块 | VCC | 5V | - | 模块供电 |
| US516P6 语音模块 | GND | GND | - | 共地 |
| US516P6 语音模块 | PB6/RXD | J23 P805 / RA6M5 P805 / SCI5 TXD5 | `BSP_IO_PORT_08_PIN_05` | RA6M5 发送线，当前保持 GPIO 高电平 |
| US516P6 语音模块 | PB7/TXD | J23 P513 / RA6M5 P513 / SCI5 RXD5 | `BSP_IO_PORT_05_PIN_13` | RA6M5 接收线，当前读取空闲/忙状态 |
| 光敏电阻模块 | VCC | 3V3 | - | 模块供电 |
| 光敏电阻模块 | GND | GND | - | 共地 |
| 光敏电阻模块 | DO | J23 P304 / RA6M5 P304 | `BSP_IO_PORT_03_PIN_04` | 数字阈值触发输入，低电平视为触发 |
| 光敏电阻模块 | AO | 暂不接入 | - | 后续如需连续光照值，可接 ADC |
| OLED | VCC | 3V3 | - | 模块供电 |
| OLED | GND | GND | - | 共地 |
| OLED | SCL | RA6M5 P008 | `BSP_IO_PORT_00_PIN_08` | GPIO 开漏模拟 I2C 时钟 |
| OLED | SDA | RA6M5 P009 | `BSP_IO_PORT_00_PIN_09` | GPIO 开漏模拟 I2C 数据 |
| CAN/CANFD | TX/RX | 尚未在 RASC 中选择 | - | 需先启用 CANFD，并外接 CAN 收发器 |

注意事项：

- DHT11 DATA 建议外接 4.7 kΩ 到 10 kΩ 上拉到 3V3；如果模块板自带上拉，可先直接接入。
- US516P6 使用 5V 供电时，需要确认模块 TXD 输出电平不超过 RA6M5 IO 允许范围；若为 5V 电平，应加电平转换或分压后接入 P513。
- 光敏模块请使用 3V3 供电，避免 DO 输出超过 RA6M5 IO 电平。
- RA6M5 CANFD TX/RX 不能直接两板互连，必须经 CAN 收发器转换为 CANH/CANL，并保证共地与合适终端电阻。

## 程序运行流程

`hal_entry()` 调用 `application_entry()` 后，系统按以下顺序初始化：

1. `soc_board_init()`：配置当前外接模块需要的 GPIO。
2. `sensor_task_init()`：初始化 DHT11、US516P6 引脚状态、光敏 DO。
3. `edge_ai_app_init()`：初始化边缘 AI 应用框架。
4. `display_task_init()`：初始化 OLED，并显示 `SkyForge` 启动画面。
5. `can_comm_task_init()`：初始化 CAN 协议层状态。

进入主循环后，每 2 秒执行一次：

1. 读取 DHT11 温湿度。
2. 读取光敏模块 DO 电平。
3. 检查 US516P6 TX 线在 RA6M5 RX 引脚上的当前电平。
4. 将传感器数据转换为晾晒 AI 输入，并运行决策树模型。
5. 刷新 OLED 四行状态。
6. 封装并尝试发送 CAN 数据帧。
7. 每 5 次主循环额外封装并尝试发送 1 帧 CAN 测试帧。

## 当前已实现功能

- DHT11 温湿度读取：
  - 使用 P313 单总线通信。
  - 支持起始信号、响应等待、40 bit 数据读取和校验和检查。
  - 读取失败时返回 `FSP_ERR_TIMEOUT` 或 `FSP_ERR_INVALID_DATA`。

- 光敏电阻数字触发读取：
  - 使用 P304 读取 DO 数字输出。
  - DO 为低电平时视为达到阈值触发。
  - DO 为高电平时视为等待/未触发。

- US516P6 语音模块阶段性接入：
  - 使用 P805/P513 预留 SCI5 TXD5/RXD5 连接。
  - 当前尚未启用 SCI5 UART，只做 GPIO 空闲电平检查。
  - OLED 可显示 `VOICE:IDLE`、`VOICE:LOW` 或 `VOICE:UNKNOWN`。

- OLED 状态显示：
  - 使用 P008/P009 通过 GPIO 开漏输出模拟 I2C。
  - 开机显示 `SkyForge` 渐显渐隐。
  - 运行时显示温湿度、光敏状态、语音连接状态和 AI 状态。

- 晾晒边缘 AI：
  - 已移植 `drying_ai_project` 导出的 C 决策树模型。
  - 模型输出状态包括 `DRYING_UNSUITABLE`、`DRYING_MODERATE`、`DRYING_SUITABLE`。
  - 动作建议包括 `None`、`Lower`、`Raise`、`Stop`、`Alarm`。
  - DHT11 有效时使用真实温湿度；读取失败时使用默认 25 ℃ / 55%RH 保持模型持续运行。
  - 光敏 DO 低电平触发时映射为较高光照特征。

- SkyForge CAN/CANFD 协议封装：
  - 测试帧 ID：`0x320`
  - 数据帧 ID：`0x321`
  - 数据区长度：16 字节
  - 帧头：`0x53 0x46`，即 ASCII `SF`
  - 协议版本：`1`
  - CRC：CRC16-CCITT，初值 `0xFFFF`，多项式 `0x1021`
  - 数据帧目前携带 AI 晾晒状态、动作建议和 `DRYING_SUITABLE` 标志位。

## 当前边界

- RASC/FSP 当前只生成了 `g_ioport`，还没有生成 SCI5 UART 实例，因此 US516P6 尚不能真实串口收发。
- RASC/FSP 当前未生成 CANFD 实例，`drivers/can_comm.c` 中 `can_comm_transport_send()` 仍返回 `FSP_ERR_NOT_OPEN`。
- 光敏模块当前只读取 DO 数字阈值，AO 模拟量尚未接入 ADC。
- AI 输入中的风速、雨量、雪量、重量、限位、避障等字段当前使用默认值或 0，后续接入对应模块后再替换为真实采样。
- 当前工程无 RTOS，所有任务由主循环按 2 秒周期轮询。

## 构建状态

已有 Keil 构建日志显示工程可以生成 `.axf` 和 `.hex`。最近日志中出现过一次：

```text
".\Objects\EBF_RA6M5.axf" - 0 Error(s), 0 Warning(s).
```

主要构建产物位于：

- `Objects/EBF_RA6M5.axf`
- `Objects/EBF_RA6M5.hex`
- `Objects/EBF_RA6M5.sbd`
- `Listings/EBF_RA6M5.map`

