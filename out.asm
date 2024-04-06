section .data
    _hy_fmt_digit    db "%d", 10, 0
section .text
    extern main
    extern printf

main:
    call _hy_main       ; Call the extra main label

    mov edi, eax    ; mov return value from main
    mov rax, 60     ; syscall number for sys_exit
    syscall         ; invoke syscall

_hy_main:              ; main
    mov rbp, rsp
    mov eax, 5
    mov rsi, rax
    mov rdi, _hy_fmt_digit
    xor rax, rax
    call printf wrt ..plt
    mov eax, 70
    pop rbp
    ret

