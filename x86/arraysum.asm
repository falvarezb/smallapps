;
; sum of the elements of an array of integers
;

        global     _main
        default rel
        section   .text
_main:
        mov rdi,int_array       ; pointer to beginning of array
        mov rcx,array_length    ; implicit loop counter
        mov rbx,0               ; register to store the result, initialized to identity element of 'add'

lp:                             ; loop to traverse array
        add rbx,[rdi]
        add rdi,array_elem_size ; advancing array pointer to the next element
        loop lp

exit:
        mov rax, 0x02000001         ; system call for exit
        mov rdi, 0                ; exit code 0
        syscall
        

        section .data  
array_elem_size equ     8  ; size in bytes
int_array       dq      100h,200h,300h,400h
array_length    equ     ($-int_array)/array_elem_size        