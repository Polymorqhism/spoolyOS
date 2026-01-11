nasm -felf32 boot.s -o boot.o
nasm -felf32 kernel.asm -o kernel.o
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -T linker.ld -o spoolyos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
grub-file --is-x86-multiboot spoolyos.bin
echo $?
mkdir -p isodir/boot/grub
cp myos.bin isodir/boot/spoolyos.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o spoolyos.iso isodir
