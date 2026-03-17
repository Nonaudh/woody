bits 64

section .text
global print

print:
    mov rdi, 1
    lea rsi, [rel msg]
    mov rdx, 11
    mov rax, 1
    syscall
    mov rax, 0x4444444444444444
    jmp rax

msg:
    db "helloWorld", 10