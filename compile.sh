i686-elf-as boot/boot.s -o builds/boot.o
i686-elf-gcc -c kernel.c -o builds/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra;
i686-elf-gcc -T linker.ld -o builds/myos.bin -ffreestanding -O2 -nostdlib builds/boot.o builds/kernel.o -lgcc;
cp builds/myos.bin isodir/boot/myos.bin;
grub-mkrescue -o builds/myos.iso isodir;
qemu-system-i386 -cdrom builds/myos.iso;
