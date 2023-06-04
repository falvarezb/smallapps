; -----------------------------------------------------------------------------
; A 64-bit Linux application to calculate the factorial of the value initially
; store in %r8. 

; To assemble and run:
;
;     nasm -fmacho64 factorial.asm -o out/factorial.o && gcc out/factorial.o -o out/factorial && ./out/factorial
; -----------------------------------------------------------------------------

        global     _main
        default rel
        extern  _printf

        section   .text
_main:
        mov r8, 0   ; factorial argument
        mov rax, 1   ; factorial result, initialized to 1

case0:
        cmp r8, 0
        cmove r8, rax     ; if arg=0 => arg=1 since 0! = 1

factorial:
        mul r8          ; mul stores the result implicitly on rax
        dec r8          ; decrement factorial argument
        cmp r8, 2       ; is factorial argument >= 2
        jge factorial   ; yes, new loop iteration

done:
        lea rdi, [format]
        mov rsi, rax
        xor rax, rax
        sub rsp, 0x8   ; align stack
        call _printf
        add rsp, 0x8

        mov       rax, 0x02000001         ; system call for exit
        xor       rdi, rdi                ; exit code 0
        syscall                           ; invoke operating system to exit

        section .data  
format:   db  "%ld", 10, 0