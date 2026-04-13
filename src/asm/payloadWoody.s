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
	je _decompressions
	cmp r9, r15 ; //if (j == key_len)
	jne _no_reset
	xor r9, r9 ; //j = 0


_no_reset:
	mov al, byte [r12 + r8] ; //load str[i]
	xor al, dl ; //str[i] ^= key[j]
	mov [r12 + r8], al ; //store back
	inc r8 ; //i++
	inc r9 ; //j++
	jmp _dexoring

;_search_in_table_hufman:
;	inc r8
;	cmp r8, r13
;	jmp _for_byte_text
;	
;	;Il faut introduire une variable temporaire pour comparere la len 
;	xor cl, cl
;	mov cl, dword[r13 + r9 * 8 + 4]//Ce qui charge dans le tableau de hufman la len 
;	cmp cl, rdi; 
;	jne _for_byte_text;
;	xor cl, cl;
;	cmp rsp, 
	
	
	
	
	

;_for_byte_text;IL faut verifier si on fait commencer r9 a 7 ou a 0 (incremente // decrementer)
;	inc r9
;	inc rdi
;	cmp r9, 7;Ne pas zapper d'incrmenter la variable
;	jmp _for_Hufman_nbr_fort;
;	shl r13, 1 ; Ce qui permet de decaler le byte vers la gauche pour ensuite introduire
;	movzx rsp, [r13 + r9 * 8 + 2]
;	mov r8, 0
;	jmp _search_in_table_hufman;
	
	

_for_Hufman_nbr_fort:;r10, r12, r9
	inc r10;Ne pas zapper d'incrementer 
	cmp r12, r10
	jmp _write;
	mov r9, 0;
	jmp _write_test;
	;jmp _for_byte_text;

	

_decompressions:
	;push r15;
	;push r14;
	;push r13;
	push r12;
	push r10;
	push r9;
	;push r8;
	;push rdi;
	;push rsp ;
	mov r12, 0x4040404040404040; nbr change;
	;mov r14, 0x4343434343434343;Adresse la structure
	;mov r13, 0x4141414141414141;Taille haufman 
	;mov r15, 0xBABEBABEBABEBABE;Adresse de text
	mov r10, -1; Iterateur sur nbr_change;
	xor r9, r9 ; Iterateur sur les bytes de .text;
	;xor r8, r8; Iterateur table hufman 
	;xor rsp, rsp ;Current
	;mov rdi, 0
	jmp _for_Hufman_nbr_fort;
	;cmp r12, r10
	
	
	;push r12;
	;push r11;
	;push r10;
	;mov r14, 45
	;mov r13, 45
	;mov r12, 45	
	;mov r11, 45
	;mov r10, 45
	;pop r8;

	;pop r13;
	;pop r14;
	;pop r15;
	;pop r11;
	;pop r10;
	jmp _write
	
_write_test:
	xor     eax, eax
	cdq
	mov     dl, 10         ;3eme argument (rdx)
	inc     eax            ;eax = 1 (syscall)
	mov     edi, eax       ;1er argument rdi = 1
	lea     rsi, [rel msg] ;2eme arg
	syscall
	jmp _for_Hufman_nbr_fort

	
_write:	
	pop r9;
	pop r10;
	pop r12;
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
tet		db "It,ok", 5
