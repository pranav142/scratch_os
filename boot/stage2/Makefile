ASM:=nasm
ASMFLAGS:=-f elf32

CC:=gcc
CFLAGS:=-ffreestanding -m32 -fno-PIC

LD:=ld
LDFLAGS:=-m elf_i386 -T stage2.ld -nostdlib

BUILD_DIR:=../../build/stage2

ASM_SOURCES:=$(wildcard *.asm)
C_SOURCES:=$(wildcard *.c)
ASM_OBJECTS:=$(addprefix $(BUILD_DIR)/, $(ASM_SOURCES:.asm=.o))
C_OBJECTS:=$(addprefix $(BUILD_DIR)/, $(C_SOURCES:.c=.o))
DRIVER_OBJECTS:=../../build/drivers/screen.o ../../build/drivers/ports.o

STAGE2_BIN=../../build/stage2.bin
STAGE2_ELF=$(BUILD_DIR)/stage2.elf

all: build_dir build

build: $(ASM_OBJECTS) $(C_OBJECTS) $(DRIVER_OBJECTS) 
	$(LD) $(LDFLAGS) -o $(STAGE2_ELF) $^
	objcopy -O binary $(STAGE2_ELF) $(STAGE2_BIN)

build_dir:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(C_OBJECTS) $(ASM_OBJECTS)
