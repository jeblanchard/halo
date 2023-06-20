# Compile all C source files to object files
gcc_command="gcc -ffreestanding -o {}.o -c -Werror -Wextra -Wall -nostdlib -lgcc {}"

# (for IntelliJ)
# shellcheck disable=SC2086
find source -type f -name "*.c" -exec $gcc_command \;

# Move all object files to out_dir
out_dir=./output
obj_dir="${out_dir}/object-files"
mv_command="mv {} ${obj_dir}"

# shellcheck disable=SC2086
find source -type f -name "*.o" -exec $mv_command \;

# Collect list of all object files to link
kernel_entry_filename="kernel_entry.o"
o_files_to_link=""
for full_filename in ${obj_dir}/*
  do
    filename="${full_filename##*/}"
    extension="${filename##*.}"

    # We filter out the kernel entry point because
    # we later include it at the beginning of our
    # binary
    if [ ${filename} != ${kernel_entry_filename} ] && [ ${extension} = "o" ]; then
      o_files_to_link="${o_files_to_link} ${full_filename}"
    fi

  done

# Link all object files

# (for IntelliJ)
# shellcheck disable=SC2086
kernel_entry_full_filename="${obj_dir}/${kernel_entry_filename}"
kernel_tmp_filename="${obj_dir}/kernel.tmp.o"
ld -T NUL -o ${kernel_tmp_filename} -Ttext 0x1000 ${kernel_entry_full_filename} ${o_files_to_link}

# Take only the machine instruction binary from the kernel's
# object file.
bin_dir="${out_dir}/bin"
kernel_bin_name="${bin_dir}/kernel.bin"
objcopy -O binary -j .text ${kernel_tmp_filename} ${kernel_bin_name}

# Include the boot sector and finalize the OS image
boot_sector_bin_filename="${bin_dir}/boot_sect_kernel_bootstrap.bin"
os_bin_name="${bin_dir}/os-image.bin"
cat ${boot_sector_bin_filename} ${kernel_bin_name} > ${os_bin_name}
