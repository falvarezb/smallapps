        global     _main
        default rel
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
        jge factorial   ; yes, new recursive call to factorial

done:
        mov       rcx, output
        mov       qword [rcx], rax
        inc       rcx
        mov       byte [rcx], 10
        mov       rax, 0x02000004         ; system call for write
        mov       rdi, 1                  ; file handle 1 is stdout
        mov       rsi, output             ; address of string to output
        mov       rdx, dataSize           ; number of bytes
        syscall                           ; invoke operating system to do the write

        mov       rax, 0x02000001         ; system call for exit
        xor       rdi, rdi                ; exit code 0
        syscall                           ; invoke operating system to exit

        section .bss  
dataSize  equ   16
output:   resb  dataSize