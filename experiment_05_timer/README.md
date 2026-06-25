# STM32 Experiment 5: Timer, PWM and Input Capture

## Project

- MCU: STM32F103ZET6
- Board: PRECHIN STM32-F103 Phoenix board
- Framework: STM32CubeMX/HAL + CMake + Makefile
- Programmer: CMSIS-DAP
- Flash tool: pyOCD

## Implemented Functions

- TIM2 update interrupt is used as a software divider. In normal mode, LED0 on PB5 toggles every 0.5 s.
- TIM3 update interrupt is used as a software divider. LED1 on PE5 toggles every 1 s.
- TIM3_CH2 outputs 8 kHz PWM on PB5 for LED0 breathing mode.
- TIM5_CH1 captures the low-level pulse width on PA0 and prints the result through USART1.
- TIM2_CH3 outputs 40 kHz PWM with 25% duty cycle on PB10.
- USART1 works at 115200 8N1 and prints status/debug messages.

## Important Pin Notes

| Function | Pin | Mode | Note |
| --- | --- | --- | --- |
| LED0 | PB5 | GPIO output or TIM3_CH2 PWM | Active low |
| LED1 | PE5 | GPIO output | Active low |
| TIM2_CH3 | PB10 | PWM output | 40 kHz, 25% duty |
| TIM3_CH2 | PB5 | PWM output | 8 kHz breathing PWM |
| TIM5_CH1 | PA0 | Input capture | Low pulse width capture |
| USART1_TX | PA9 | AF push-pull | Connect to USB-TTL RX |
| USART1_RX | PA10 | Input | Connect to USB-TTL TX |

PB5 is shared by LED0 GPIO control and TIM3_CH2 PWM. The program therefore uses two modes:

- Normal mode: PB5 works as GPIO, and TIM2 toggles LED0 every 0.5 s.
- Breathing mode: send `b` over USART1, PB5 switches to TIM3_CH2 PWM and LED0 breathes.
- Send `n` over USART1 to return PB5 to normal GPIO mode.

## USART Commands

| Command | Function |
| --- | --- |
| `b` | Enable LED0 breathing mode on PB5/TIM3_CH2 |
| `n` | Return LED0 to normal 0.5 s toggle mode |
| `p` | Print the latest TIM5 low pulse width |
| `h` | Print help text |

## Build

```powershell
cd "D:\DESK\stm32 final test\experiment_05_timer"
mingw32-make clean
mingw32-make
```

Build result:

```text
RAM:    2328 B / 64 KB    3.55%
FLASH: 22608 B / 512 KB   4.31%
text data bss dec hex filename
22500 108 2220 24828 60fc build/firmware.elf
```

## Flash

```powershell
pyocd load -t stm32f103ze --frequency 100000 build\firmware.elf
```

Flash result:

```text
Erased 24576 bytes (12 sectors), programmed 23552 bytes (23 pages), skipped 0 bytes (0 pages)
```

## Hardware Verification

- After reset, LED0 toggles every 0.5 s in normal mode.
- LED1 toggles every 1 s.
- USART1 prints the experiment startup help text at 115200 8N1.
- Send `b`: LED0 changes to breathing mode.
- Send `n`: LED0 returns to 0.5 s toggle mode.
- Connect PB10 to PA0 with a jumper wire: USART1 should repeatedly print low pulse width values captured by TIM5_CH1.

