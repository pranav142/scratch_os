# OS From Scratch

Compile boot sector to binary
```bash
nasm -f bin boot_sect.asm -o boot_sect.bin
```

Emulate boot sector in qemu
```bash
qemu-system-x86_64 -drive file=boot_sect.bin,format=raw 
```