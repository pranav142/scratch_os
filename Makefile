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

KERNEL_BIN=$(BUILD_DIR)/kernel.bin
KERNEL_LD_SCRIPT=$(SRC_DIR)/kernel.ld

KERNEL_ENTRY_SRC=$(SRC_DIR)/kernel/kernel_entry.asm
KERNEL_ENTRY_OBJ=$(BUILD_DIR)/kernel_entry.o
OS_BIN=$(BUILD_DIR)/os-image
FLOPPY_IMG=$(BUILD_DIR)/floppy.img

C_SOURCES=$(wildcard kernel/*.c drivers/*.c)
C_HEADERS=$(wildcard kernel/*.h drivers/*.h)
C_OBJECTS=$(patsubst %, $(BUILD_DIR)/%.o, $(notdir $(basename $(C_SOURCES))))

ASM_SOURCES=$(wildcard kernel/*.asm drivers/*.asm)
ASM_OBJECTS=$(patsubst %, $(BUILD_DIR)/%.o, $(notdir $(basename $(ASM_SOURCES))))

all: build_dir build_floppy

run: all 
	qemu-system-x86_64 -boot order=a -drive file=$(FLOPPY_IMG),format=raw,index=0,if=floppy

build_dir:
	mkdir -p $(BUILD_DIR)

build_boot: $(BOOT_SECT_SRC) | build_dir
	$(ASM) $(ASMFLAGS) $< -o $(BOOT_SECT_BIN)

$(ASM_OBJECTS): $(ASM_SOURCES)
	$(foreach src,$(ASM_SOURCES), $(ASM) $(src) -f elf -o $(BUILD_DIR)/$(notdir $(src:.asm=.o));)

$(C_OBJECTS): $(C_SOURCES) $(C_HEADERS)
	$(foreach src,$(C_SOURCES), $(CC) $(CFLAGS) -c $(src) -o $(BUILD_DIR)/$(notdir $(src:.c=.o));)

compile_kernel: $(KERNEL_ENTRY_OBJ) $(C_OBJECTS) $(ASM_OBJECTS)
	ld -m elf_i386 -o $(KERNEL_BIN) -Ttext 0x1000 $^ --oformat binary

create_os_image: build_boot compile_kernel
	cat $(BOOT_SECT_BIN) $(KERNEL_BIN) > $(OS_BIN)

build_floppy: create_os_image
	$(DD) if=/dev/zero of=$(FLOPPY_IMG) bs=1024 count=1440
	$(DD) if=$(OS_BIN) of=$(FLOPPY_IMG) conv=notrunc

kernel.dis : $(KERNEL_BIN)
	ndisasm -b 32 $< > $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run build_dir build_boot compile_kernel create_os_image build_floppy clean
