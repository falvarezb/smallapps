  global _main
  default rel

  section .text
_main:
  mov rax, 1 ; immediate values, rax=1
  mov rax, op1 ; memory address, rax=memory address of op1
  lea rax, op1 ; same as above
  mov rax, [op1] ; memory values, rax=content of memory address of op1
  lea rax, [op1]
 
  mov [count], rax
  add rax,rax

  mov       rax, 0x02000001         ; system call for exit
  xor       rdi, rdi                ; exit code 0
  syscall                           ; invoke operating system to exit



section .data
  op1:  db 2
  count: db 0