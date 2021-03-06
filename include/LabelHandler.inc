;===============================================================================
; class LabelHandler
;
; Guarda todos os labels e seus respectivos offsets 
; que forem aparecendo no codigo fonte
;===============================================================================

	; includes
	; ----------------------------------------------------------------------

	; private static constants
	; ----------------------------------------------------------------------
	LabelHandler:
		.LABEL_LENGTH		equ 16    ; 13 bytes for label + 1 byte (end of string) + 2 bytes for address
		.MAX_SIZE		equ 1600  ; allows max 100 labels
		.ORIGINAL_DS		equ 2000h ; //TODO


	; public static methods
	; ----------------------------------------------------------------------
		.index			dw 0
		.startMemoryAddress	dw 63935 ; 0FFFFh - 1600d 

		LabelHandler.add:
		; --------------------------------------------------------------
		; ax = pointer to label string
		; dx = offset
		; bl = 0 - stack overflow !
		;      1 - ok
		;      2 - label length too long
				pusha
				call	os_string_length
				cmp	ax, (LabelHandler.LABEL_LENGTH-3)
				popa
				jg	.labelLengthTooLong
				mov	si, ax
				mov	ax, [LabelHandler.index]
				cmp	ax, LabelHandler.MAX_SIZE
				jge	.stackOverflow
				mov	bx, [LabelHandler.startMemoryAddress]
				mov	di, [LabelHandler.index]
			.nextChar:
				mov	al, [si]
				mov	byte [bx + di], al
				inc	si	
				inc	di
				inc	word [LabelHandler.index]
				cmp	al, 0
				je	.ok
				jmp	.nextChar
			.ok:
				mov	cx, di
				and	cx, 0fh
				mov	ax, 14
				sub	ax, cx
				add	di, ax
				mov	word [bx + di], dx
				add	di, 2
				mov	word [LabelHandler.index], di
				mov	bl, 1
				ret
			.labelLengthTooLong:
				mov	bl, 2
				ret
			.stackOverflow:
				mov	bl, 0
				ret

		LabelHandler.getOffset:
		; --------------------------------------------------------------
		; ax = pointer to label string
		; ret dx = offset
		;     bl = 0 - label not found !
		;          1 - ok
				mov	si, ax
				mov	di, [LabelHandler.startMemoryAddress]
			.nextLabel:
				mov	al, [di]
				cmp	al, 0 ; end of list ?
				je	.notFound
				call	os_string_compare
				jc	.found
				add	di, LabelHandler.LABEL_LENGTH
				jmp	.nextLabel
			.found:
				mov	dx, [di+14]
				mov	bl, 1
				ret
			.notFound:
				mov	bl, 0
				ret

		LabelHandler.list:
		; --------------------------------------------------------------
				mov	di, [LabelHandler.startMemoryAddress]
			.nextLabel:
				mov	al, [di]
				cmp	al, 0 ; end of list ?
				je	.end

				mov	si, .msg.labelEqual
				call	os_print_string
				mov	si, di
				call	os_print_string

				mov	si, .msg.offsetEqual
				call	os_print_string
				mov	ax, [di + 14]
				call	os_int_to_string
				mov	si, ax
				call	os_print_string
				call	os_print_newline

				add	di, LabelHandler.LABEL_LENGTH
				jmp	.nextLabel
			.end:
				ret
			.msg.labelEqual		db "label=", 0
			.msg.offsetEqual	db " / offset=", 0

;===============================================================================

