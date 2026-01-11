set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR i686)

set(CMAKE_C_COMPILER i686-elf-gcc)
set(CMAKE_ASM_NASM_COMPILER nasm)

# prevent try-run during compiler checks
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-i686-elf.cmake -DCMAKE_BUILD_TYPE=Release
# cmake --build build

