; -----------------------------------------------------------------------------
; A 64-bit command line application to compute x^y.
;
; Syntax: power x y
; x and y are (32-bit) integers
;
; nasm -g -fmacho64 power.asm -o out/power.o && gcc out/power.o -o out/power && out/power 2 3
; -----------------------------------------------------------------------------

        global  _main
        default rel
        extern  _printf
        extern  _puts
        extern  _atoi

        section .text ;align=16
_main:
        push    r12                     ; save callee-save registers
        push    r13
        push    r14
        ; By pushing 3 registers our stack is already aligned for calls

        cmp     rdi, 3                  ; must have exactly two arguments
        jne     error1

        mov     r12, rsi                ; argv

; We will use ecx to count down form the exponent to zero, esi to hold the
; value of the base, and eax to hold the running product.

        mov     rdi, [r12+16]           ; argv[2]
        call    _atoi                    ; y in eax
        cmp     eax, 0                  ; disallow negative exponents
        jl      error2
        mov     r13d, eax               ; y in r13d

        mov     rdi, [r12+8]            ; argv
        call    _atoi                    ; x in eax
        mov     r14d, eax               ; x in r14d

        mov     eax, 1                  ; start with answer = 1
check:
        test    r13d, r13d              ; we're counting y downto 0
        jz      gotit                   ; done
        imul    eax, r14d               ; multiply in another x
        dec     r13d
        jmp     check
gotit:                                  ; print report on success
        lea     rdi, [answer]
        movsxd  rsi, eax
        xor     rax, rax
        call    _printf
        jmp     done
error1:                                 ; print error message
        lea     rdi, [badArgumentCount]
        call    _puts
        jmp     done
error2:                                 ; print error message
        lea     rdi, [negativeExponent]
        call    _puts
done:                                   ; restore saved registers
        pop     r14
        pop     r13
        pop     r12
        ret

        section .data
answer:
        db      "%d", 10, 0
badArgumentCount:
        db      "Requires exactly two arguments", 10, 0
negativeExponent:
        db      "The exponent may not be negative", 10, 0