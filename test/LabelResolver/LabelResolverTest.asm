;===============================================================================
; class LabelResolverTest
;===============================================================================
		bits	16
		org	32768
start:
	; ----------------------------------------------------------------------
		mov	si, msg.testLabel
		call	printString

		mov	si, msg.sep
		call 	printString
	; ----------------------------------------------------------------------

		; ax = pointer to label string
		; dx = address (offset) of label (org)
		; bx = address where value must be corrected
		; cl = LabelResolver.TYPE_XX
		mov	ax, labelName
		mov	dx, [labelOffset]
		mov	bx, 1111
		mov	cl, LabelResolver.TYPE_RB
		call	LabelResolver.add

		mov	si, labelName
		call	printString
		mov	si, [LabelResolver.startMemoryAddress]
		call	printString

		mov	bx, [LabelResolver.startMemoryAddress]

		mov	ax, [bx+14]
		call	os_int_to_string
		mov	si, ax
		call	printString

		mov	ax, [bx+16]
		call	os_int_to_string
		mov	si, ax
		call	printString

		mov	ah, 0
		mov	al, [bx+18]
		call	os_int_to_string
		mov	si, ax
		call	printString

		mov	si, msg.sep
		call 	printString

	; ----------------------------------------------------------------------
		mov	ax, labelName2
		mov	dx, [labelOffset2]
		mov	bx, 2222
		mov	cl, LabelResolver.TYPE_IB
		call	LabelResolver.add

		mov	si, labelName
		call	printString
		mov	si, [LabelResolver.startMemoryAddress]
		add	si, 32
		call	printString

		mov	bx, [LabelResolver.startMemoryAddress]

		mov	ax, [bx+14+32]
		call	os_int_to_string
		mov	si, ax
		call	printString

		mov	ax, [bx+16+32]
		call	os_int_to_string
		mov	si, ax
		call	printString

		mov	ah, 0
		mov	al, [bx+18+32]
		call	os_int_to_string
		mov	si, ax
		call	printString

		mov	si, msg.sep
		call 	printString

	; ----------------------------------------------------------------------
		mov	ax, labelName3
		mov	dx, [labelOffset3]
		mov	bx, 333
		mov	cl, LabelResolver.TYPE_IW
		call	LabelResolver.add

		mov	si, labelName
		call	printString
		mov	si, [LabelResolver.startMemoryAddress]
		add	si, 64
		call	printString

		mov	bx, [LabelResolver.startMemoryAddress]

		mov	ax, [bx+14+64]
		call	os_int_to_string
		mov	si, ax
		call	printString

		mov	ax, [bx+16+64]
		call	os_int_to_string
		mov	si, ax
		call	printString

		mov	ah, 0
		mov	al, [bx+18+64]
		call	os_int_to_string
		mov	si, ax
		call	printString

		mov	si, msg.sep
		call 	printString

	; ----------------------------------------------------------------------
		; ret dx = address (offset) of label (org)
		;     bx = address where value must be corrected
		;     cl = LabelResolver.TYPE_XX
		;     si = 0 - label not found !
		;          1 - ok
		mov	ax, labelToSearch
		call	LabelResolver.getOffset ; return offset of label in DX
		cmp	si, 1
		jne	.notFound
	.found:
		mov	si, msg.found
		call	printString

		mov	si, msg.dxEqual
		call	printString
		mov	ax, dx
		call	os_int_to_string
		mov	si, ax
		call	printString

		mov	si, msg.bxEqual
		call	printString
		mov	ax, Bx
		call	os_int_to_string
		mov	si, ax
		call	printString

		mov	si, msg.clEqual
		call	printString
		mov	ch, 0
		mov	ax, cx
		call	os_int_to_string
		mov	si, ax
		call	printString

		jmp	.end
	.notFound:
		mov	si, msg.notFound
		call	printString
		jmp	.end


	; ----------------------------------------------------------------------
	.end:
		mov	si, msg.sep
		call 	printString
		ret

	; ----------------------------------------------------------------------
	printString:
		call	os_print_string
		call	os_print_newline
		; call	os_wait_for_key
		ret

	; ----------------------------------------------------------------------
	%include	"../../include/mikedev.inc"
	%include	"../../include/LabelResolver.inc"

	msg.sep			db "------------------------", 0

	msg.testLabel		db "Test label resolver", 0
	msg.found		db "Found !", 0
	msg.notFound		db "Not found !", 0
	msg.dxEqual		db "DX=", 0
	msg.bxEqual		db "BX=", 0
	msg.clEqual		db "CL=", 0

	labelName		db "myLabel", 0
	labelOffset		dw 1234

	labelName2		db "abc", 0
	labelOffset2		dw 5678

	labelName3		db "l234567890123", 0
	labelOffset3		dw 33333

	labelToSearch		db "l234567890123", 0

;===============================================================================
