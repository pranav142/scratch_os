ASM:=nasm
ASMFLAGS:=-f elf32

CC:=gcc
CFLAGS:=-ffreestanding -m32 -nostdlib -g

BUILD_DIR:=../build/drivers
ASM_SOURCES:=$(wildcard *.asm)
C_SOURCES:=$(wildcard *.c)
ASM_OBJECTS:=$(addprefix $(BUILD_DIR)/, $(ASM_SOURCES:.asm=.o))
C_OBJECTS:=$(addprefix $(BUILD_DIR)/, $(C_SOURCES:.c=.o))

build: build_dir $(ASM_OBJECTS) $(C_OBJECTS)

build_dir:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

