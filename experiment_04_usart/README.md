# STM32 实验四：串口通信实验

## 项目信息

- MCU：STM32F103ZET6
- 开发板：普中 STM32-F103 凤凰开发板
- 工程类型：STM32CubeMX/HAL + CMake + Makefile
- 烧录器：CMSIS-DAP
- 烧录工具：pyOCD

## 实验功能

本实验使用 USART1 进行串口通信，串口参数为 115200 bps、8 位数据、1 位停止位、无奇偶校验、无硬件流控。

- 上电初始化完成后，通过 `printf` 从 USART1 发送 `Receive form STM32!`。
- USART1 使用中断方式每次接收 1 字节数据。
- 串口收到字符 `0` 时，翻转 LED0。
- 串口收到字符 `1` 时，翻转 LED1。
- 主循环不执行 LED 自动闪烁或按键逻辑，避免干扰串口实验。

## 引脚连接

| 功能 | 引脚 | 配置 | 说明 |
| --- | --- | --- | --- |
| LED0 | PB5 | 推挽输出，默认高电平 | 低电平点亮 |
| LED1 | PE5 | 推挽输出，默认高电平 | 低电平点亮 |
| USART1_TX | PA9 | 复用推挽输出 | 连接 USB-TTL RX |
| USART1_RX | PA10 | 输入 | 连接 USB-TTL TX |
| GND | GND | 共地 | 连接 USB-TTL GND |

## 编译

```powershell
mingw32-make clean
mingw32-make
```

## 烧录

```powershell
pyocd load -t stm32f103ze build\firmware.elf
```

## 串口助手验证

1. 将 USB-TTL 与开发板连接：PA9 接 USB-TTL RX，PA10 接 USB-TTL TX，GND 接 GND。
2. 打开串口助手，选择对应串口号，参数设置为 115200、8N1、无硬件流控。
3. 复位或重新上电开发板，应看到串口输出：`Receive form STM32!`。
4. 在串口助手发送字符 `0`，LED0 状态应翻转一次。
5. 在串口助手发送字符 `1`，LED1 状态应翻转一次。
