BITS 64

; str, str_len, key, key_len, entry
; %rdi, %rsi, %rdx, %rcx, %r10

section .text
global _start

_start:
    call _addr

_addr:
    pop rbx
    sub rbx, 0x5
	mov r11, 0xDEADDEADDEADDEAD
	sub rbx, r11
	xor r8, r8 ; //int i = 0
	xor r9, r9 ; //int j = 0
	mov r10, 0xDEADBEEFCAFEBABE
	add r10, rbx
	;jmp _end
	mov rdi, 0xCAFECAFECAFECAFE
	add rdi, rbx
	mov rsi, 0xBABEBABEBABEBABE
	mov rdx, 0xBEEFBEEFBEEFBEEF
	add rdx, rbx
	mov rcx, 0x4242424242424242

_loop:
	cmp r8, rsi ; //if (i == str_len)
	je _end
	cmp r9, rcx ; //if (j == key_len)
	jne _no_reset
	xor r9, r9 ; //j = 0

_no_reset:
	mov al, byte [rdi + r8] ; //load str[i]
	mov dl, byte [rdx + r9] ; //load key[j]
	xor al, dl ; //str[i] ^= key[j]
	mov [rdi + r8], al ; //store back
	inc r8 ; //i++
	inc r9 ; //j++
	jmp _loop

_end:
	jmp r10;
