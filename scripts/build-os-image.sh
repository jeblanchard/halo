# Compile all C source files to object files
gcc_command="gcc -ffreestanding -c -Werror -Wextra -Wall -nostdlib -lgcc {} -o {}.o"
find source -type f -name "*.c" -exec sh "$gcc_command" \;

# Collect all object files to link
out_dir=./output
o_files_to_link=""
for o_filename in "$out_dir"/*
do
  o_files_to_link="${o_files_to_link} ${o_filename}"
done

# Link all object files
kernel_o_filename="kernel.tmp"
ld -T NUL -o ${kernel_o_filename} -Ttext 0x1000 "${o_files_to_link}"

# Take only the machine instruction binary from the kernel's
# object file.
kernel_bin_name="kernel.bin"
objcopy -O binary -j .text ${kernel_o_filename} ${kernel_bin_name}

# Include the boot sector and finalize the OS image
boot_sector_bin_name="boot_sect.bin"
os_bin_name="os-image"
cat ${boot_sector_bin_name} ${kernel_bin_name} > os_bin_name
