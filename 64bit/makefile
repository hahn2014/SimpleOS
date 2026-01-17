TOOLCHAIN = aarch64-none-elf-
CC      = $(TOOLCHAIN)gcc
LD      = $(TOOLCHAIN)ld
OBJCOPY = $(TOOLCHAIN)objcopy
CFLAGS  = -Wall -O2 -ffreestanding -nostdlib -nostartfiles -march=armv8-a -mcpu=cortex-a72
OBJS    = boot.o kernel.o uart.o


all: kernel8.img

kernel8.img: kernel8.elf
	$(OBJCOPY) kernel8.elf -O binary kernel8.img

kernel8.elf: $(OBJS) linker.ld
	$(CC) $(CFLAGS) -T linker.ld -o kernel8.elf $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.elf *.img *.log

run: kernel8.img
	qemu-system-aarch64 -M raspi4b -kernel kernel8.img -nographic -serial stdio

run_alt: kernel8.img
	qemu-system-aarch64 -M raspi4b -kernel kernel8.img -serial mon:stdio -nographic

run_debug: kernel8.img
	qemu-system-aarch64 -M raspi4b -kernel kernel8.img -nographic -serial stdio -d unimp,guest_errors,int -D qemu.log

real: CFLAGS += -DREAL_HARDWARE
real: clean kernel8.img

.PHONY: all clean run run_alt run_debug real