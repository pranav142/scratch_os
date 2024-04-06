ASM:=nasm
ASMFLAGS:=-f elf32

CC:=gcc
CFLAGS:=-ffreestanding -c -m32 -fno-pic -fno-pie -g -fno-stack-protector -fno-builtin

LD:=ld
LDFLAGS:=-m elf_i386 -T kernel.ld -Ttext 0x1000

BUILD_DIR:=../build/kernel

ASM_SOURCES:=$(wildcard *.asm)
C_SOURCES:=$(wildcard *.c)
ASM_OBJECTS:=$(addprefix $(BUILD_DIR)/, $(ASM_SOURCES:.asm=.o))
C_OBJECTS:=$(addprefix $(BUILD_DIR)/, $(C_SOURCES:.c=.o))
DRIVER_OBJECTS:=$(wildcard ../build/drivers/*.o)

KERNEL_BIN=../build/kernel.bin
KERNEL_ELF:=$(BUILD_DIR)/kernel.elf

all: build_dir build

build_dir: 
	mkdir -p $(BUILD_DIR)

build: $(ASM_OBJECTS) $(C_OBJECTS) $(DRIVER_OBJECTS)
	$(LD) $(LDFLAGS) -o $(KERNEL_ELF) $^
	objcopy -O binary $(KERNEL_ELF) $(KERNEL_BIN)

$(BUILD_DIR)/%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(C_OBJECTS) $(ASM_OBJECTS)
