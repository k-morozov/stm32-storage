BUILD_ROOT=../..

OBJS = main.o init.o test.o

BINARY_NAME = main

include $(BUILD_ROOT)/common_defs.mk

.PHONY: $(BINARY_NAME)

$(BINARY_NAME): $(BINARY_NAME).bin

clean:
	@rm -f $(OBJS) main.elf main.bin main.elf.map *.d
	clean main.elf

flash: $(BINARY_NAME)
	$(FLASH_BINARY)