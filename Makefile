ROMNAME := savemgr

BUILD_DIR := build

ifeq ($(N64_INST),)
  $(error N64_INST is not set)
endif

include $(N64_INST)/include/n64.mk

all: $(ROMNAME)_4k.z64 $(ROMNAME)_16k.z64
.PHONY: all

C_FILES := $(shell find src -name '*.c')
OBJS := $(addprefix $(BUILD_DIR)/,$(C_FILES:.c=.o))

$(BUILD_DIR)/$(ROMNAME)_4k.elf: $(OBJS)
$(BUILD_DIR)/$(ROMNAME)_16k.elf: $(OBJS)

$(ROMNAME)_4k.z64: N64_ROM_TITLE = "Save Manager 4K"
$(ROMNAME)_4k.z64: N64_ROM_SAVETYPE = eeprom4k

$(ROMNAME)_16k.z64: N64_ROM_TITLE = "Save Manager16K"
$(ROMNAME)_16k.z64: N64_ROM_SAVETYPE = eeprom16k

clean:
	$(RM) -r $(BUILD_DIR) *.z64
.PHONY: clean

ifneq ($(wildcard $(BUILD_DIR)),)
  -include $(shell find $(BUILD_DIR) -name '*.d')
endif
