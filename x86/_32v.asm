global _main
section .text
_main:
;------ x86-32bit -------
  mov eax, 2 ; eax --> alias to the least significant 32 bits of rax
  mov ebx, 3 ; ebx --> alias to the least significant 32 bits of rbx
  mov ecx, 4 ; ecx --> lowalias to the least significanter 32 bits of rcx

;------ x86-64bit -------
  mov rax, 0x200000000
  mov rbx, 0x300000000
  mov rcx, 0x400000000

;------ 16-bit (legacy registers, accessible for backwards compatibility) -------
  mov ax, 2 ; ax --> alias to the least significant 16 bits of rax
  mov bx, 3 ; bx --> alias to least significant 16 bits of rbx
  mov cx, 4 ; cx --> alias to the least significant 16 bits of rbx

  mov       rax, 0x02000001         ; system call for exit
  xor       rdi, rdi                ; exit code 0
  syscall                           ; invoke operating system to exit