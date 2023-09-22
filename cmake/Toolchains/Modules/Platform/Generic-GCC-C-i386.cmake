# configure gcc options
set(CMAKE_C_FLAGS_INIT "-c -ffreestanding -Werror -Wextra -Wall -nostdlib -lgcc")

# configure ld options
set(CMAKE_EXE_LINKER_FLAGS_INIT "-T NUL -o kernel.tmp.o -Ttext 0x1000")


