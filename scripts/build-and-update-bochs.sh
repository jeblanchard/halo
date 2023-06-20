gcc -ffreestanding -c -Werror -Wextra -Wall -nostdlib -lgcc kernel.c -o kernel.o

ld -T NUL -o kernel.tmp -Ttext 0x1000 kernel_entry.o kernel.o

objcopy -O binary -j .text  kernel.tmp kernel.bin

cat boot_sect.bin kernel.bin > os-image

nasm boot_sect.asm -f bin -o boot_sect.bin
