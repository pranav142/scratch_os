ASM:=nasm
ASMFLAGS+=-f elf32

CC:=gcc
CFLAGS:=-m32 -ffreestanding -nostdlib -fno-PIC -fno-PIE -g

LD:=ld
LDFLAGS:=-m elf_i386 -T kernel.ld

BUILD_DIR:=../build/kernel

ASM_SOURCES:=$(wildcard *.asm)
ASM_HEADERS:=$(wildcard *.inc)
C_SOURCES:=$(wildcard *.c)
C_HEADERS:=$(wildcard *.h)

ASM_OBJECTS:=$(addprefix $(BUILD_DIR)/, $(ASM_SOURCES:.asm=.o))
C_OBJECTS:=$(addprefix $(BUILD_DIR)/, $(C_SOURCES:.c=.o))
DRIVER_OBJECTS:=$(wildcard ../build/drivers/*.o)
UTIL_OBJECTS:=$(wildcard ../build/utils/*.o)


KERNEL_BIN=../build/kernel.bin
KERNEL_ELF:=$(BUILD_DIR)/kernel.elf

all: build_dir build

build_dir: 
	mkdir -p $(BUILD_DIR)

build: $(C_OBJECTS) $(DRIVER_OBJECTS) $(ASM_OBJECTS) $(UTIL_OBJECTS)
	$(LD) $(LDFLAGS) -o $(KERNEL_ELF) $^
	objcopy -O binary $(KERNEL_ELF) $(KERNEL_BIN)

$(BUILD_DIR)/%.o: %.asm $(ASM_HEADERS)
	$(ASM) $(ASMFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: %.c $(C_HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(C_OBJECTS) $(ASM_OBJECTS)
