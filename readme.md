# OS From Scratch

```bash
nasm -f bin boot_sect.asm -o boot_sect.bin
```

```bash
qemu-system-x86_64 -drive file=boot_sect.bin,format=raw 
```