cmake_minimum_required(VERSION 3.20)

# please speed we NEED compile_commands.json
# also set this before project()
# otherwise it won't work
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(spoolyos LANGUAGES C ASM_NASM)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

enable_language(ASM_NASM)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)

add_subdirectory(kernel)
