# STM32 Experiment 6: ADC Voltage Sampling

## Project

- MCU: STM32F103ZET6
- Board: PRECHIN STM32-F103 Phoenix board
- Framework: STM32CubeMX/HAL + Makefile
- Programmer: CMSIS-DAP
- Flash tool: pyOCD

## Implemented Functions

- ADC1 channel 1 samples PA1.
- USART1 prints ADC raw value and converted voltage every 500 ms.
- USART1 parameters: 115200 baud, 8 data bits, 1 stop bit, no parity.
- ADC input voltage is converted with 3.3 V reference and 12-bit full scale.

## Important Pin Notes

| Function | Pin | Mode | Note |
| --- | --- | --- | --- |
| ADC1_IN1 | PA1 | Analog input | Do not connect to 5 V |
| USART1_TX | PA9 | AF push-pull | Board CH340 serial output |
| USART1_RX | PA10 | Input | Board CH340 serial input |

## Expected Serial Output

```text
Experiment 6 ADC ready.
ADC1 channel 1 samples PA1 every 500 ms.
USART1: 115200 8N1.
Do not connect PA1 to 5V. Use 0V to 3.3V only.
ADC1_CH1=3829, Voltage=3.084 V
```

## Build

```powershell
cd "D:\DESK\stm32 final test\experiment_06_adc"
mingw32-make clean
mingw32-make
```

## Flash

```powershell
pyocd load -t stm32f103ze --frequency 100000 build\firmware.elf
```

## Hardware Verification

- Open the CH340 serial port at 115200 8N1.
- Connect PA1 to GND: ADC value should be near 0 and voltage near 0 V.
- Connect PA1 to 3.3 V: ADC value should be near 4095 and voltage near 3.3 V.
- Do not connect PA1 to 5 V.
