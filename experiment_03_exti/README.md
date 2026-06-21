# STM32 实验三：外部中断实验

## 项目信息

- MCU：STM32F103ZET6
- 开发板：普中 STM32-F103 凤凰开发板
- 工程类型：STM32CubeMX/HAL + CMake + Makefile
- 烧录器：CMSIS-DAP
- 烧录工具：pyOCD

## 实验功能

本实验使用 GPIO 外部中断控制 2 个板载 LED，不使用主循环轮询按键作为主要方案，不实现蜂鸣器。

- WK_UP 按下一次，LED0 翻转一次。
- KEY0 按下一次，LED1 翻转一次。
- KEY1 按下一次，LED0 和 LED1 同时翻转一次。
- 按键中断回调中使用 `HAL_GetTick()` 做简单软件消抖。

## 引脚连接

| 功能 | 引脚 | GPIO 配置 | 有效电平 |
| --- | --- | --- | --- |
| LED0 | PB5 | 推挽输出，默认高电平 | 低电平点亮 |
| LED1 | PE5 | 推挽输出，默认高电平 | 低电平点亮 |
| WK_UP | PA0 | EXTI 上升沿，内部下拉 | 按下高电平 |
| KEY0 | PE4 | EXTI 下降沿，内部上拉 | 按下低电平 |
| KEY1 | PE3 | EXTI 下降沿，内部上拉 | 按下低电平 |

## NVIC 配置

- `EXTI0_IRQn`
- `EXTI3_IRQn`
- `EXTI4_IRQn`

中断服务函数调用：

- `HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0)`
- `HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3)`
- `HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4)`

## 编译

```powershell
mingw32-make clean
mingw32-make
```

## 烧录

```powershell
pyocd load -t stm32f103ze build\firmware.elf
```

## 实验现象

上电后 LED0 和 LED1 默认熄灭。按下 WK_UP 时 LED0 状态翻转；按下 KEY0 时 LED1 状态翻转；按下 KEY1 时两个 LED 同时翻转。主循环不再执行自动闪烁逻辑，LED 状态只由外部中断按键事件改变。
