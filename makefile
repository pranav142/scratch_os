ASM=nasm
ASMFLAGS=-f bin
DD=dd

CC=gcc
CFLAGS = -ffreestanding -c -m32 -fno-pic -fno-pie
LDFLAGS = -Ttext 0x1000 --oformat binary

SRC_DIR=.
BUILD_DIR=./build
BOOT_SECT_SRC=$(SRC_DIR)/boot/boot_sect.asm
BOOT_SECT_BIN=$(BUILD_DIR)/boot_sect.bin

KERNEL_SRC=$(SRC_DIR)/kernel/kernel.c
KERNEL_OBJ=$(BUILD_DIR)/kernel.o
KERNEL_BIN=$(BUILD_DIR)/kernel.bin
KERNEL_LD_SCRIPT=$(SRC_DIR)/kernel.ld

KERNEL_ENTRY_SRC=$(SRC_DIR)/kernel/kernel_entry.asm
KERNEL_ENTRY_OBJ=$(BUILD_DIR)/kernel_entry.o
OS_BIN=$(BUILD_DIR)/os-image
FLOPPY_IMG=$(BUILD_DIR)/floppy.img


all: build_dir build_floppy

build_dir:
	mkdir -p $(BUILD_DIR)

build_boot: $(BOOT_SECT_SRC) | build_dir
	$(ASM) $(ASMFLAGS) $< -o $(BOOT_SECT_BIN)

compile_kernel: $(KERNEL_ENTRY_SRC) $(KERNEL_SRC)
	$(ASM) $(KERNEL_ENTRY_SRC) -f elf -o $(KERNEL_ENTRY_OBJ)
	$(CC) $(CFLAGS) -c $(KERNEL_SRC) -o $(KERNEL_OBJ)
	ld -m elf_i386 -o $(KERNEL_BIN) -Ttext 0x1000 $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ) --oformat binary

create_os_image: build_boot compile_kernel
	cat $(BOOT_SECT_BIN) $(KERNEL_BIN) > $(OS_BIN)

build_floppy: create_os_image
	$(DD) if=/dev/zero of=$(FLOPPY_IMG) bs=1024 count=1440
	$(DD) if=$(OS_BIN) of=$(FLOPPY_IMG) conv=notrunc

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all build_dir build_boot compile_kernel create_os_image build_floppy clean
