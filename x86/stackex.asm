; ----------------------------------------------------------------------------------------
; Assembly program to illustrate the use of stack frames when calling functions.
;
; This program is equivalent to the C program stackhex.c. Therefore, if stackhex.c is compiled and then
; disassembled, the result should be similar to the assembly code in this file
;
; gcc stackhex.c
; otool -v -t stackex
;
;   nasm -g -fmacho64 stackex.asm -o out/stackex.o && gcc out/stackex.o -o out/stackex && out/stackex
;
; ----------------------------------------------------------------------------------------

        global    _main
        default rel
        extern _printf
           

        section   .text

myfunc:
        mov rax, 123
        ret

_main:
        ;-------------------------
        ; begin function prologue 
        ;-------------------------    
        push rbp
        mov rbp, rsp    ; %rbp points to the beginning of this function's stack frame
        sub rsp, 0x20   ; making room for 32 bytes                
        ;-------------------------
        ; end function prologue 
        ;------------------------- 
        

        ; passing up to 6 arguments via registers
        mov rdi, 1
        mov rsi, 2
        mov rdx, 3
        mov rcx, 4
        mov r8, 5
        mov r9, 6 

        ; rest of arguments are to be passed through the stack
        mov qword [rsp], 7
        mov qword [rsp+8], 8   

        ; when calling a function, stack must be 16-bit aligned, on other words, at this point, 
        ; rsp must be such that rsp+8 is a multiple of 16
        ; (since 'call' pushes the return address onto the stack, by the time the instruction pointer jumps to the first instruction
        ; of the function, the rsp will be multiple of 16)
        ; We meet that criterion as the instructions 'push rbp' and 'sub rsp, 0x20' have displaced the top of the stack
        ; 8 + 32 = 40 bytes (and pushing the return address will displace it another 8 bytes)
        call myfunc 

        lea rdi, [format]
        mov rsi, rax
        mov rax, 0      ; rax holds the number of variadic arguments being passed to the function
                        
        call _printf

        ;-------------------------
        ; begin function epilogue 
        ;-------------------------        
        add rsp, 0x20
        pop rbp
        xor rax, rax    ; return value of main = 0
        ret
        ;------------------------
        ; end function epilogue 
        ;------------------------

        section   .data
format: db  "result=%ld",0xa
