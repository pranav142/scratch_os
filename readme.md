# OS From Scratch

Compile boot sector to binary
```bash
nasm -f bin boot_sect.asm -o boot_sect.bin
```

Emulate boot sector in qemu
```bash
qemu-system-x86_64 -drive file=boot_sect.bin,format=raw 
```

Making empty Floppy Img
```bash
dd if=/dev/zero of=floppy.img bs=1024 count=1440
```

Filling Floppy With Boot Loader
```bash
dd if=boot_sect.bin of=floppy.img conv=notrunc 
```

Booting From Floppy Img
```bash
qemu-system-x86_64 -boot order=a -fda floppy.img
```
