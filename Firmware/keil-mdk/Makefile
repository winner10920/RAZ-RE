# ------------------------------------------------
#
# @file Makefile (based on gcc)
#
# ------------------------------------------------

TARGET = output
BUILD_DIR = build

DEBUG = 1
OPT = -O0 -g3

TARGET_PLATFORM := n32g031
DEFS += -DN32G031
DEFS += -DUSE_STDPERIPH_DRIVER

CROSS_COMPILE = arm-none-eabi-

C_DIRS += src
C_DIRS += firmware/CMSIS/device
C_DIRS += firmware/$(TARGET_PLATFORM)_std_periph_driver/src
SRC_OBJS_DIRS += $(foreach DIR, $(C_DIRS), $(wildcard $(DIR)/*.c))
C_SOURCES = $(SRC_OBJS_DIRS)

ASM_SOURCES = firmware/CMSIS/device/startup/startup_$(TARGET_PLATFORM).s

C_INCLUDES += -Iinc
C_INCLUDES += -Ifirmware/CMSIS/core
C_INCLUDES += -Ifirmware/CMSIS/device
C_INCLUDES += -Ifirmware/$(TARGET_PLATFORM)_std_periph_driver/inc

CFLAGS += -mthumb -mcpu=cortex-m0
CFLAGS += $(OPT) $(DEFS)

LFLAGS += -mthumb -mcpu=cortex-m0
LFLAGS += --specs=nano.specs -lc -lm -lnosys -Wl,--gc-sections
LFLAGS += -Xlinker -Map=$(BUILD_DIR)/$(TARGET).map

LDSCRIPT = firmware/CMSIS/device/$(TARGET_PLATFORM)_flash.ld

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CROSS_COMPILE)gcc $(CFLAGS) $(DEFS) $(C_INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(CROSS_COMPILE)gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CROSS_COMPILE)gcc $(OBJECTS) $(LFLAGS) -T$(LDSCRIPT) -o $@
	$(CROSS_COMPILE)size $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(CROSS_COMPILE)objcopy -O binary -S $< $@

$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf $(BUILD_DIR)
