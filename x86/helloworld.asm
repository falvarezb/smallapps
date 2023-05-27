global _main
section .text
_main:
  mov rax, 0x2000004 ; write
  mov rdi, 1 ; stdout
  mov rsi, msg
  mov rdx, msg.len
  syscall
  mov rax, 0x2000001 ; exit
  mov rdi, 0
  syscall
section .data
msg:    db      "Hello, world!", 10
.len:   equ     $ - msg


; nasm -g -f macho64 helloworld.asm
; ld -no_pie -L /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib -macosx_version_min 10.16.0 -lSystem -o helloworld helloworld.o
; ./helloworld