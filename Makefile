MAKE:=make
DD := dd
FLOPPY_IMG:=build/floppy.img
STAGE1_BIN:=build/stage1.bin
STAGE2_BIN:=build/stage2.bin
KERNEL_BIN:=build/kernel.bin
# This needs to be consistent with reserved sectors entry in FAT BPB and with the number of sectors Stage1 loads
STAGE2_SECTORS:=9

all: build_dir build_floppy

run: all 
	qemu-system-i386 -boot order=a -drive file=$(FLOPPY_IMG),format=raw,index=0,if=floppy

qemu_debug: all
	qemu-system-i386 -fda $(FLOPPY_IMG) -s -S

debug: all
	bochs -f bochs_config

build_dir:
	mkdir -p build

generate_interrupts:
	python3 ./scripts/interrupts.py

stage1:
	$(MAKE) -C boot/stage1

stage2:
	$(MAKE) -C boot/stage2

kernel:
	$(MAKE) -C kernel

drivers:
	$(MAKE) -C drivers

build_floppy: generate_interrupts stage1 stage2 kernel drivers
	$(DD) if=/dev/zero of=$(FLOPPY_IMG) bs=512 count=2880
	mkfs.fat -F 12 -n "NBOS" $(FLOPPY_IMG)
	$(DD) if=$(STAGE1_BIN) of=$(FLOPPY_IMG) bs=512 count=1 conv=notrunc
	$(DD) if=$(STAGE2_BIN) of=$(FLOPPY_IMG) bs=512 seek=1 count=$(STAGE2_SECTORS) conv=notrunc
	mcopy -i $(FLOPPY_IMG) $(KERNEL_BIN) "::kernel.bin"
 
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run qemu_debug debug build_dir generate_interrupts stage1 stage2 kernel drivers build_floppy clean
