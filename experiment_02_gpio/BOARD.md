# STM32F103ZET6 Phoenix Board

Project: final_test_2

This board definition is based on the confirmed hardware information and the
current project source code. Do not use older board notes as the source of
truth.

## Confirmed Hardware

### MCU

- Part: STM32F103ZET6
- Package: LQFP144
- Flash: 512 KB, high-density device
- RAM: 64 KB

### Debug And Download

- Debug interface: SWD
- Downloader: CMSIS-DAP
- Interface: SWD
- Max clock: 100 kHz
- Connect mode: Normal
- Flash algorithm: STM32F10x High-density Flash 512k

### LED

- LED0: PB5
- LED1: PE5
- Active level in current source: low level on

### KEY

- WKUP: PA0
- KEY0: PE4
- KEY1: PE3

## Current Project Configuration

### Clock

- Current SYSCLK source in source code: HSI
- Current SYSCLK frequency in source code: 8 MHz
- PLL: disabled
- HSE value defined by HAL configuration: 8 MHz
- HSE pins appear in CubeMX configuration, but HSE is not used by
  `SystemClock_Config()` in the current source code.

### GPIO Usage In Source

- LED0 PB5: output push-pull
- LED1 PE5: output push-pull
- WKUP PA0: input with pull-down, active high
- KEY0 PE4: input with pull-up, active low
- KEY1 PE3: input with pull-up, active low

## Confirmed Resources

- RCC: present in CubeMX configuration and source code
- SYSCLK: present; currently HSI 8 MHz
- HSE: value and pins present; not used as current SYSCLK source
- LED: present; LED0 PB5, LED1 PE5
- KEY: present; WKUP PA0, KEY0 PE4, KEY1 PE3
- SWD: present; PA13/PA14 Serial Wire, JTAG disabled and SWD kept enabled

## Resources To Confirm

The following resources are not configured or not used by the current
application source code:

- USART
- SPI
- I2C
- BEEP

Before adding these peripherals, confirm their exact board pins and update the
CubeMX `.ioc` file and source code together.
