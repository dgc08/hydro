section .data
    msg db 'Hello, World!', 0xa
    msglen equ $ - msg

section .text
    global _start
    extern main

_start:
    call main       ; Call the extra main label
    mov rbx, rax    ; Store the return value from main in rbx

    mov rax, 60     ; syscall number for sys_exit
    mov rdi, rbx    ; return the value returned by main
    syscall         ; invoke syscall

main:             ; Extra main label containing the Hello, World! stuff
    mov rax, 1     ; syscall number for sys_write
    mov rdi, 1     ; file descriptor 1 (stdout)
    mov rsi, msg   ; pointer to the message
    mov rdx, msglen ; message length
    syscall        ; invoke syscall
    mov rax, 21    ; Return value of 21 from main
    ret            ; Return from main
