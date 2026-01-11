BITS 32

; multiboot header
section .multiboot
align 4
MB_MAGIC      equ 0x1BADB002
MB_FLAGS      equ 0x00000003                ; mem info + module alignment
MB_CHECKSUM   equ -(MB_MAGIC + MB_FLAGS)

  dd MB_MAGIC
  dd MB_FLAGS
  dd MB_CHECKSUM

; stack
section .bss
align 16
stack_bottom:
  resb 16384;   ; 16kb stack (should be enough right?)
stack_top:

; main
section .text
global _start
extern kernel_main        ; call kernel_main from kernel.c

_start:
  mov esp, stack_top      ; set stack pointer
  call kernel_main

  ; loop halt
.halt:
  cli
  hlt
  jmp .halt
