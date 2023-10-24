# Collect list of all source files to compile

source_dir=source/kernel/networking/ip-v6/trial
src_files_to_compile=""
for full_filename in ${source_dir}/*
  do
    filename="${full_filename##*/}"
    extension="${filename##*.}"

    # Ensure we only grab .c files.
    if [ "${extension}" = "c" ]; then
      src_files_to_compile="${src_files_to_compile} ${full_filename}"
    fi

  done

printf "${src_files_to_compile}"

cmocka_include_directory=C:/MinGW/external-libs/cmocka/include
cmocka_lib_directory=C:/MinGW/external-libs/cmocka/lib
out_dir=./output/test

gcc -o $out_dir/sample.exe -I$cmocka_include_directory $src_files_to_compile -L$cmocka_lib_directory -lcmocka.dll
