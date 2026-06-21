# STM32 实验二 GPIO 控制实验

## 项目信息

- 项目名称：STM32 实验二 GPIO 控制实验
- MCU：STM32F103ZET6
- 开发板：普中 STM32-F103 凤凰开发板
- 工程目录：`final_test_2`

## 实验功能

- LED0/LED1 上电后交替闪烁
- WK_UP 翻转 LED0
- KEY0 翻转 LED1
- KEY1 同时翻转 LED0/LED1

## 引脚连接

| 功能 | 引脚 | 有效电平 |
| --- | --- | --- |
| LED0 | PB5 | 低电平点亮 |
| LED1 | PE5 | 低电平点亮 |
| WK_UP | PA0 | 按下高电平 |
| KEY0 | PE4 | 按下低电平 |
| KEY1 | PE3 | 按下低电平 |

## 编译

```powershell
mingw32-make
```

## 烧录

```powershell
pyocd load -t stm32f103ze build\firmware.elf
```

## 编译结果

- 0 Error，0 Warning

## 主要文件

- `Core/`：用户应用代码和 STM32 HAL 初始化代码
- `Drivers/`：CMSIS 与 STM32F1 HAL 驱动
- `cmake/`：CMake 工具链配置
- `Makefile`：MinGW Make 构建入口
- `final_test_2.ioc`：STM32CubeMX 工程配置
- `startup_stm32f103xe.s`：启动文件
- `STM32F103XX_FLASH.ld`：链接脚本
- `BOARD.md`：开发板与引脚说明
