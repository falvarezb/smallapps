;
; find first element in array of integers that is equal to 0
;
; Comment on data types
; - if we use 64-bit data types, then int_array is 'dq' and array_elem_size is 8 and we can use register 'rbx'
; - however, if we use different data types, like 'dw', then array_elem_size is 2 and we need to use 'bx' to make
;   sure the zero flag (ZF) is set based on the value of lowest 2 bytes
;

        global     _main
        default rel
        section   .text
_main:
        mov rdi,int_array       ; pointer to beginning of array
        mov rcx,array_length    ; implicit loop counter        

lp:                             ; loop to traverse array
        mov bx,-1              ; register to store the result: if 0 is found, rbx=0, otherwise rbx!=0
                                ; initialized to identity element of 'and'
        and bx,[rdi]
        je exit
        add rdi,array_elem_size ; advancing array pointer to the next element
        loop lp

exit:
        mov rax, 0x02000001         ; system call for exit
        mov rdi, 0                ; exit code 0
        syscall
        

        section .data  
array_elem_size equ     2  ; size in bytes (this value must correspond to the datatype of int_array)
int_array       dw      1,6,0,3
array_length    equ     ($-int_array)/array_elem_size        