bits 64
default rel
global _start

;     rdi   rsi    rdx
;      v     v      v
;write(fd,   msg,   len);

_start:
    call _addr

_addr:
    pop rbx
    sub rbx, 0x5
	mov r11, 0xDEADDEADDEADDEAD
	sub rbx, r11

_patching:
	mov r10, 0xCAFECAFECAFECAFE
	add r10, rbx

_save_regis:
	push rdi
	push rsi
	push rdx

	xor r8, r8 ; //int i = 0
	xor r9, r9 ; //int j = 0
	mov r12, 0xBABEBABEBABEBABE
	add r12, rbx
	mov r13, 0xBEEFBEEFBEEFBEEF
	mov r14, 0xFEEDFEEDFEEDFEED
	add r14, rbx
	mov r15, 0x4242424242424242

_dexoring:
	cmp r8, r13 ; //if (i == str_len)
	je _write
	cmp r9, r15 ; //if (j == key_len)
	jne _no_reset
	xor r9, r9 ; //j = 0

_no_reset:
	mov al, byte [r12 + r8] ; //load str[i]
	mov dl, byte [r14 + r9] ; //load key[j]
	xor al, dl ; //str[i] ^= key[j]
	mov [r12 + r8], al ; //store back
	inc r8 ; //i++
	inc r9 ; //j++
	jmp _dexoring

_write:
	xor     eax, eax
	cdq
	mov     dl, 10         ;3eme argument (rdx)
	inc     eax            ;eax = 1 (syscall)
	mov     edi, eax       ;1er argument rdi = 1
	lea     rsi, [rel msg] ;2eme arg
	syscall

_reset_regis:
	pop rdx
	pop rsi
	pop rdi

_jmp:
	jmp r10

msg     db "..WOODY..",10
