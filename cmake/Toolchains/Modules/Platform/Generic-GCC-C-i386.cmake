# configure gcc options
set(CMAKE_C_FLAGS_INIT "-c -ffreestanding -Werror -Wextra -Wall -nostdlib")

# configure nasm options
set(CMAKE_ASM_NASM_FLAGS_INIT "-f elf")
