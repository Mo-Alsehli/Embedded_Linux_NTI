#
# Bare-metal Raspberry Pi 3 Makefile
#

# Path to your cross-compiler (adjust if you move your x-tools)
CROSS_COMPILE := $(HOME)/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-

# Tool definitions
CC      := $(CROSS_COMPILE)gcc
LD      := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy

# Compiler flags for bare-metal ARM
CFLAGS := -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

# Source files
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

# Default target
all: clean kernel8.img

start.o: start.S
	$(CC) $(CFLAGS) -c start.S -o start.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.img: start.o $(OBJS)
	$(LD) -nostdlib -nostartfiles start.o $(OBJS) -T link.ld -o kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

clean:
	rm -f kernel8.elf *.o

run:
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial null -serial stdio
