section .text
    extern main
    extern printf

main:
    call main_hy

    mov rdi, rax                ; store return value of main_hy in rdi, which is exit code
    mov rax, 60         ; 60 means exit
    syscall

main_hy:
    mov rbp, rsp
    ; Write message to stdout
    mov rax, 1             ; syscall number for sys_write
    mov rdi, 1             ; file descriptor 1 (stdout)
    mov rsi, msg           ; pointer to the message
    mov rdx, msglen        ; message length
    syscall                ; invoke syscall


    ; Prepare arguments for printf
    mov rdi, fmt           ; format string
    mov rsi, 2             ; second argument for printf (just an example)
    xor rax, rax
    ; Call printf
    call printf wrt ..plt

    ; Set return value and clean up the stack
    mov rax, 21            ; Return value of 21 from main_hy
    pop rbp                ; Restore the previous base pointer
    ret                    ; Return from main_hy

section .data
    msg db 'Hello, World!', 0xa
    msglen equ $ - msg

    fmt:    db "%d", 10, 0
