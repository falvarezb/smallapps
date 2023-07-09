  global _main
  default rel

  section .text align=16
_main:
  movsd xmm0, [tt]
  movsd xmm0, [one]
  movsd xmm1, [three]
  divsd xmm0, xmm1

  mov rax, 1 ; immediate values, rax=1
  mov rax, op1 ; memory address, rax=memory address of op1
                ; triggers warning: ld: warning: PIE disabled. Absolute addressing (perhaps -mdynamic-no-pic) not allowed in code signed PIE, but used in _main from out/test.o. To fix this warning, don't compile with -mdynamic-no-pic or link with -Wl,-no_pie
  lea rax, op1 ; same as above but without warning
  lea rax, [op1] ; similar to 'lea rax op1'
  mov rax, [op1] ; memory values, rax=content of memory address of op1  
 
  mov [count], rax
  add rax,rax

  mov       rax, 0x02000001         ; system call for exit
  xor       rdi, rdi                ; exit code 0
  syscall                           ; invoke operating system to exit



section .data
  op1  db 2
  count db 0
  one dq 1.0
  three dq 3.0
  tt dq 3.602879701896397e+16