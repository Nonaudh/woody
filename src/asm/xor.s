BITS 64

; str, str_len, key, key_len
; %rdi, %rsi, %rdx, %rcx

section .text
global _start

_start:
	mov rdi, 0x42DEAD4242DEAD42
	mov rsi, 0x69CAFE6969CAFE69
	mov rdx, 0x21BABE2121BABE21
	mov rcx, 0x99FEED9999FEED99
	xor r8, r8 ; int i = 0
	xor r9, r9 ; int j = 0

_loop:
	cmp r8, rsi ; if (i == str_len)
	je _end
	cmp r9, rcx ; if (j == key_len)
	jne _no_reset
	xor r9, r9 ; j = 0

_no_reset:
	mov al, byte [rdi + r8] ; load str[i]
	mov dl, byte [rdx + r9] ; load key[j]
	xor al, dl ; str[i] ^= key[j]
	mov [rdi + r8], al ; store back
	inc r8 ; i++
	inc r9 ; j++
	jmp _loop

_end:
	ret
