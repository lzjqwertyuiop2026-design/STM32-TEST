# STM32 Experiment 7: DMA USART Transmit

## Project

- MCU: STM32F103ZET6
- Board: PRECHIN STM32-F103 Phoenix board
- Framework: STM32CubeMX/HAL + Makefile
- Programmer: CMSIS-DAP
- Flash tool: pyOCD

## Implemented Functions

- LED0 on PB5 toggles every 500 ms as the program running indicator.
- KEY0 on PE4 starts one USART1 DMA transmission after software debounce.
- USART1 works at 115200 baud, 8 data bits, 1 stop bit, no parity.
- DMA1 Channel 4 is linked to USART1 TX.
- The transmit buffer contains 3000 bytes generated in code.
- Buffer pattern repeats lowercase letters with spaces, for example:

```text
a b c d e f g h i j k l m n o p q r s t u v w x y z ...
```

## Important Pin Notes

| Function | Pin | Mode | Note |
| --- | --- | --- | --- |
| LED0 | PB5 | GPIO output | Active low |
| KEY0 | PE4 | GPIO input pull-up | Pressed low |
| USART1_TX | PA9 | AF push-pull | Board CH340 serial output |
| USART1_RX | PA10 | Input | Board CH340 serial input |
| USART1_TX DMA | DMA1 Channel 4 | Memory to peripheral | 3000-byte transfer |

## Expected Serial Output

After reset:

```text
Experiment 7 DMA ready.
KEY0 on PE4 starts one USART1 DMA transmission.
DMA buffer length: 3000 bytes.
USART1: 115200 8N1.
```

After pressing KEY0:

```text
DMA USART1 TX start, 3000 bytes:
a b c d e f g h i ...
DMA USART1 TX complete.
```

## Build

```powershell
cd "D:\DESK\stm32 final test\experiment_07_dma"
mingw32-make clean
mingw32-make
```

## Flash

```powershell
pyocd load -t stm32f103ze --frequency 100000 build\firmware.elf
```

## Hardware Verification

- Open the CH340 serial port at 115200 8N1.
- Reset the board and confirm the startup text is printed.
- Confirm LED0 blinks every 500 ms.
- Press KEY0 once and confirm the 3000-byte letter stream is printed.
- Confirm the serial output ends with `DMA USART1 TX complete.`
