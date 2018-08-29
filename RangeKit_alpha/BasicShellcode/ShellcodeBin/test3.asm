; ---------------------------------------------------------------------------
; Symantec ATP Sirius Test Tools
; Author: Greg Hoglund
; Note: shellcode based on prior published work / training materials owned by Greg
; Note: modified for Symantec purposes, free to use for any purpose
; 
; simple code to get bearings and put absolute location into ebx
; ---------------------------------------------------------------------------
BITS 32

	jmp	SHORT	C2	; remember to use SHORT or you get NULLs	
C1:	pop	ebx		; get the data location in memory
	jmp	SHORT	C3
C2:	call	C1
C3:	