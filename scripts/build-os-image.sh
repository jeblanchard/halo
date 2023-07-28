# Compile all C source files to object files
gcc_command="gcc -ffreestanding -o {}.o -c -Werror -Wextra -Wall -nostdlib -lgcc {}"

# (for IntelliJ)
# shellcheck disable=SC2086
find source -type f -name "*.c" -exec $gcc_command \;

out_dir=./output
obj_dir="${out_dir}/object-files"

# Compile kernel entry ASM to an object file
kernel_entry_asm_filename="./source/boot/multiboot2.asm"
kernel_entry_obj_filename="multiboot2.o"
kernel_entry_obj_full_filename="${obj_dir}/${kernel_entry_obj_filename}"
nasm -f elf ${kernel_entry_asm_filename} -o ${kernel_entry_obj_full_filename}

# Move all object files to out_dir
mv_command="mv {} ${obj_dir}"

# shellcheck disable=SC2086
find source -type f -name "*.o" -exec $mv_command \;

# Collect list of all object files to link
o_files_to_link=""
kernel_tmp_filename="kernel.tmp.o"
for full_filename in ${obj_dir}/*
  do
    filename="${full_filename##*/}"
    extension="${filename##*.}"

    # We filter out the kernel entry point because
    # we later include it at the beginning of our
    # binary
    if [ ${filename} != ${kernel_entry_obj_filename} ] &&
    [ ${filename} != ${kernel_tmp_filename} ] &&
    [ ${extension} = "o" ]; then
      o_files_to_link="${o_files_to_link} ${full_filename}"
    fi

  done

# Link all object files

# (for IntelliJ)
# shellcheck disable=SC2086
kernel_tmp_full_filename="${obj_dir}/${kernel_tmp_filename}"
ld -T NUL -o ${kernel_tmp_full_filename} -Ttext 0x1000 ${kernel_entry_obj_full_filename} ${o_files_to_link}

# Take only the machine instruction binary from the kernel's
# object file.
bin_dir="${out_dir}/bin"
kernel_bin_name="${bin_dir}/kernel.bin"
objcopy -O binary -j .text ${kernel_tmp_full_filename} ${kernel_bin_name}

# Compile bootsector ASM to binary file
boot_sect_asm_filename="./source/boot/boot_sector.asm"
boot_sector_bin_filename="${bin_dir}/boot_sector.bin"
nasm ${boot_sect_asm_filename} -f bin -o ${boot_sector_bin_filename}

# Include the boot sector and finalize the OS image
os_bin_filename="${bin_dir}/os-image.bin"
cat ${boot_sector_bin_filename} ${kernel_bin_name} > ${os_bin_filename}
