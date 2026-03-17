BITS 64

section .text
global _start

_start:
    mov rax, 1
    mov rdi, 1
    lea rsi, [rel msg]
    mov rdx, msglen
    syscall

    mov rax, 60
    xor rdi, rdi
    syscall

msg:
    db "Hello World !", 10
msglen equ $ - msg
