section .data
    prompt1 db "Enter the first number: ", 0
    prompt2 db "Enter the second number: ", 0
    result_msg db "The result is: ", 0
    newline db 10, 0

section .bss
    num1 resb 8  ; 8 bytes for the first number
    num2 resb 8  ; 8 bytes for the second number
    result resb 16  ; 16 bytes for the result

section .text
    global start
    extern printf, scanf

start:
    ; Prompt user for the first number
    mov rdi, prompt1
    mov rax, 0
    call printf

    ; Read the first number from standard input
    mov rdi, num1
    mov rax, 0
    call scanf

    ; Prompt user for the second number
    mov rdi, prompt2
    mov rax, 0
    call printf

    ; Read the second number from standard input
    mov rdi, num2
    mov rax, 0
    call scanf

    ; Multiply the two numbers
    mov rax, qword [num1]
    imul rax, qword [num2]

    ; Store the result
    mov qword [result], rax

    ; Print the result message
    mov rdi, result_msg
    mov rax, 0
    call printf

    ; Print the result
    mov rdi, result
    mov rax, 0
    call printf

    ; Print a newline
    mov rdi, newline
    mov rax, 0
    call printf

    ; Exit the program
    mov eax, 0x2000001  ; system call number for exit
    xor edi, edi        ; exit status code 0
    syscall
