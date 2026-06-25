TARGET := firmware
BUILD_DIR := build

PREFIX := arm-none-eabi-
CC := $(PREFIX)gcc
AS := $(PREFIX)gcc
SIZE := $(PREFIX)size
OBJCOPY := $(PREFIX)objcopy

MCU := -mcpu=cortex-m3 -mthumb
DEFS := -DUSE_HAL_DRIVER -DSTM32F103xE
INCLUDES := \
	-ICore/Inc \
	-IDrivers/STM32F1xx_HAL_Driver/Inc \
	-IDrivers/STM32F1xx_HAL_Driver/Inc/Legacy \
	-IDrivers/CMSIS/Device/ST/STM32F1xx/Include \
	-IDrivers/CMSIS/Include

C_SOURCES := \
	Core/Src/main.c \
	Core/Src/stm32f1xx_it.c \
	Core/Src/stm32f1xx_hal_msp.c \
	Core/Src/system_stm32f1xx.c \
	Core/Src/syscalls.c \
	Core/Src/sysmem.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
	Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c

ASM_SOURCES := startup_stm32f103xe.s

OBJECTS := $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES:.s=.o))

CFLAGS := $(MCU) $(DEFS) $(INCLUDES) -std=gnu11 -Wall -fdata-sections -ffunction-sections -O0 -g3 -MMD -MP
ASFLAGS := $(MCU) $(DEFS) $(INCLUDES) -x assembler-with-cpp -MMD -MP
LDFLAGS := $(MCU) -TSTM32F103XX_FLASH.ld --specs=nano.specs -Wl,-Map=$(BUILD_DIR)/$(TARGET).map -Wl,--gc-sections -Wl,--print-memory-usage
LIBS := -lc -lm -lnosys

.PHONY: all clean

all: $(BUILD_DIR)/$(TARGET).elf

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) STM32F103XX_FLASH.ld
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/%.o: %.c
	if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	$(AS) $(ASFLAGS) -c $< -o $@

clean:
	if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"

-include $(OBJECTS:.o=.d)
