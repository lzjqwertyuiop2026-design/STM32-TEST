# STM32 Experiment Repository

This repository stores STM32F103ZET6 laboratory projects in separate folders.

## Experiments

- `experiment_02_gpio/`: GPIO LED and key experiment.
- `experiment_03_exti/`: External interrupt experiment.
- `experiment_04_usart/`: USART communication experiment.
- `experiment_05_timer/`: Timer, PWM and input capture experiment.
- `experiment_06_adc/`: ADC voltage sampling experiment.
- `experiment_07_dma/`: DMA USART transmit experiment.
- `final_temperature_adc_usart/`: ADC temperature, timer, GPIO and USART integrated project.

Each experiment folder is an independent STM32CubeMX/HAL + CMake/Makefile project.

## Common Files

- `BOARD.md`: board pin notes.
- `微机原理与单片机技术（Cortex-M3）实验指导书v4.0(1).docx`: lab guide.
- `普中STM32-F103-凤凰开发板原理图.pdf`: board schematic.

## Common Commands

Build inside an experiment folder:

```powershell
mingw32-make clean
mingw32-make
```

Flash inside an experiment folder:

```powershell
pyocd load -t stm32f103ze --frequency 100000 build\firmware.elf
```
