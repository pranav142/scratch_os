ASM:=nasm
ASMFLAG:=-f bin

SOURCES:=boot_sect.asm
STAGE1_BIN:=../../build/stage1.bin

build: $(SOURCES)
	$(ASM) $(ASMFLAGS) $< -o $(STAGE1_BIN)

