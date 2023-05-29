; ----------------------------------------------------------------------------------------
; This is an macOS console program that writes "Hola, mundo" on one line and then exits.
; It uses puts from the C library.  To assemble and run:
;
;     nasm -fmacho64 asmtoc.asm -o out/asmtoc.o && gcc out/asmtoc.o -o out/asmtoc && ./out/asmtoc
;
; In macOS, C functions (or any function that is exported from one module to another, really) 
; must be prefixed with underscores. 
; The call stack must be aligned on a 16-byte boundary. 
; And when accessing named variables, a rel prefix is required.
; ----------------------------------------------------------------------------------------

          global    _main
          extern    _puts   

          section   .text
_main:    push      rbx                     ; Call stack must be aligned
          lea       rdi, [rel message]      ; First argument is address of message
          call      _puts                   ; puts(message)
          pop       rbx                     ; Fix up stack before returning
          ret

          section   .data
message:  db        "Hola, mundo", 0        ; C strings need a zero byte at the end