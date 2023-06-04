; ----------------------------------------------------------------------------------------
; Assembly program to calculate pi.
;
;   nasm -g -fmacho64 pi.asm -o out/pi.o && gcc out/pi.o -o out/pi && out/pi
;   ld -no_pie -L /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib -macosx_version_min 10.16.0 -lSystem -o out/pi out/pi.o
;
; ----------------------------------------------------------------------------------------

        global    _main
        default rel
        extern _printf

        section   .text ;align=16
_main:
        push rbp
        mov rbp, rsp

        mov rcx, [num_steps]    ; loop index
        
        cvtsi2sd xmm0, rcx      ; e.g. if rcx=0x0000000000000002, xmm0=0x4000000000000000
        movsd xmm3, [float1]        
        divsd xmm3, xmm0   ; xmm3 = step size

        xorpd xmm0, xmm0    ; xmm0 is the final result; initialized to 0
                        
lp:
        movsd xmm2, [floathalf] ; xmm2 is the result of each step
        mov r11, rcx
        sub r11,1
        cvtsi2sd xmm4, r11
        addsd xmm2, xmm4
        mulsd xmm2, xmm3
        mulsd xmm2, xmm2
        addsd xmm2, [float1]
        movsd xmm4, [float4]
        divsd xmm4, xmm2
        addsd xmm0, xmm4      
        loop lp

        mulsd xmm0, xmm3
        
        lea rdi, [format]
        ; instead of rsi as 2nd argument, xmm0 is used                
        mov rax, 1      ; rax holds the number of variadic arguments being passed to the function

        ;sub rsp, 0x8    ; to align the stack              
        call _printf
        pop rbp
       
        ;add rsp, 0x8        
        xor rax, rax    ; return value of main = 0
        ret

        section   .text
num_steps:  dq  1000000
float1 dq 1.0
float2 dq 2.0
float4 dq 4.0
floathalf dq 0.5
format: db  "pi=%0.20f",0xa,0
