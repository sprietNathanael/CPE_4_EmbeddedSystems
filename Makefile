NAME = blink
CROSS_COMPILE ?= arm-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc
CPU = cortex-m0
FOPTS = -fno-builtin -ffunction-sections -fdata-sections -ffreestanding
LDOPTS = -Wl,--gc-sections -Wl,--sort-section=alignment -Wl,--build-id=none

LDFLAGS = -static -nostartfiles -nostdlib -Tlpc_link_lpc1224.ld $(LDOPTS)
CFLAGS = -Wall -Wextra -mthumb -mcpu=$(CPU) $(FOPTS)

.PHONY: all
all: $(NAME).bin

$(NAME).bin: $(NAME)
	$(CROSS_COMPILE)objcopy -O binary $^ $@

clean: 
	rm
	 -f $(NAME)

