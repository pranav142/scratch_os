ASM=nasm
ASMFLAGS=-f bin
DD=dd

SRC_DIR=.
BUILD_DIR=./build
BOOT_SECT_SRC=$(SRC_DIR)/boot/boot_sect.asm
BOOT_SECT_BIN=$(BUILD_DIR)/boot_sect.bin
FLOPPY_IMG=$(BUILD_DIR)/floppy.img

all: build_dir $(FLOPPY_IMG)

build_dir:
	mkdir -p $(BUILD_DIR)

$(BOOT_SECT_BIN): $(BOOT_SECT_SRC) | build_dir
	$(ASM) $(ASMFLAGS) $< -o $@

$(FLOPPY_IMG): $(BOOT_SECT_BIN)
	$(DD) if=/dev/zero of=$@ bs=1024 count=1440
	$(DD) if=$(BOOT_SECT_BIN) of=$@ conv=notrunc

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all build_dir clean
