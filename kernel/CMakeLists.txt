file(GLOB_RECURSE KERNEL_C_SOURCES
    CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/kernel/*.c
)

file(GLOB_RECURSE KERNEL_ASM_SOURCES
    CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/kernel/*.asm
    ${PROJECT_SOURCE_DIR}/kernel/*.s
)

file(GLOB_RECURSE BOOT_SOURCES
    CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/boot/*.s
    ${PROJECT_SOURCE_DIR}/boot/*.asm
)

add_executable(spoolyos.bin
    ${KERNEL_C_SOURCES}
    ${KERNEL_ASM_SOURCES}
    ${BOOT_SOURCES}
)

target_include_directories(spoolyos.bin
    PRIVATE
        ${PROJECT_SOURCE_DIR}/kernel/include
)

target_compile_options(spoolyos.bin
    PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-ffreestanding -Wall -Wextra>
)

target_link_options(spoolyos.bin
    PRIVATE
        -T ${PROJECT_SOURCE_DIR}/linker/linker.ld
        -ffreestanding
        -nostdlib
)

target_link_libraries(spoolyos.bin
    PRIVATE
        gcc
)
