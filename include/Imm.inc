;===============================================================================
; class Imm
;===============================================================================

	; includes -> needs "../../include/Character.inc"
	; ----------------------------------------------------------------------

	; public static methods
	; ----------------------------------------------------------------------

		Imm.getValue:
		; --------------------------------------------------------------
		; si = imm string
		; dx = value
		; ah = 0 - invalid format !
		;      1 - ok
				call	private.Imm.getBinaryStringValue
				cmp	ah, 2
				je	.outOfRange
				cmp	ah, 1
				je	.ok

				call	private.Imm.getHexStringValue
				cmp	ah, 2
				je	.outOfRange
				cmp	ah, 1
				je	.ok

				call	private.Imm.getDecimalStringValue
				cmp	ah, 2
				je	.outOfRange
				cmp	ah, 1
				je	.ok

			.invalidFormat:
				mov	ah, 0
				ret
			.ok:
				mov	ah, 1
				ret
			.outOfRange:
				mov	ah, 2
				ret

	; private static methods
	; ----------------------------------------------------------------------

		private.Imm.isValidBinary:
		; --------------------------------------------------------------
		; si = imm string
		; cx = string length
		; ah = 0 - not is binary
		;      1 - is binary
				mov	cx, 0
				push	si
			.nextChar:
				mov	al, [si]
				inc	si
				inc	cx
				cmp	al, 'B'
				je	short .endOfString
				cmp	al, '1'
				je	short .nextChar
				cmp	al, '0'
				je	short .nextChar
				jmp	short .false
			.endOfString:
				mov	al, [si]
				cmp	al, 0
				jne	.false
			.true:
				mov	ah, 1
				pop	si
				ret	
			.false:
				mov	ah, 0
				pop	si
				ret	

		private.Imm.isValidHex:
		; --------------------------------------------------------------
		; si = imm string
		; cx = string length
		; ah = 0 - not is hex
		;      1 - is hex
				mov	cx, 0
				push	si
				; first char must be a digit
				mov	al, [si]
				inc	si
				inc	cx
				call	Character.isDigit
				cmp	ah, 1
				jne	.false
			.nextChar:
				mov	al, [si]
				inc	si
				inc	cx
				cmp	al, 'H'
				je	.endOfString
				call	Character.isDigit
				cmp	ah, 1
				je	.nextChar
				call	Character.isLetterA_F
				cmp	ah, 1
				je	.nextChar
				jmp	short .false
			.endOfString:
				mov	al, [si]
				cmp	al, 0
				jne	.false
			.true:
				mov	ah, 1
				pop	si
				ret	
			.false:
				mov	ah, 0
				pop	si
				ret	

		private.Imm.isValidDecimal:
		; --------------------------------------------------------------
		; si = imm string
		; cx = string length
		; ah = 0 - not is decimal
		;      1 - is decimal
				mov	cx, 0
				push	si
				mov	al, [si]
				inc	si
				inc	cx
				call	Character.isDigit
				cmp	ah, 1
				jne	.false
			.nextChar:
				mov	al, [si]
				inc	si
				inc	cx
				cmp	al, 0
				je	.true
				call	Character.isDigit
				cmp	ah, 1
				je	.nextChar
				jmp	short .false
			.true:
				mov	ah, 1
				pop	si
				dec	cx
				ret	
			.false:
				mov	ah, 0
				pop	si
				ret	

		private.Imm.getBinaryStringValue:
		; --------------------------------------------------------------
		; si = imm string
		; dx = value
		; ah = 0 - binary invalid format ! 
		;      1 - ok
		;      2 - binary out of range !
				call	private.Imm.isValidBinary
				cmp	ah, 1
				jne	.false
				cmp	cx, 17
				jg	.outOfRange
				mov	dx, 0
				mov	di, 1
				sub	cx, 2
				shl	di, cl
			.nextChar:
				mov	al, [si]
				inc	si
				cmp	al, 'B'
				je	short .true
				cmp	al, '1'
				je	short .set
				cmp	al, '0'
				je	short .notSet
				jmp	short .false
			.set:
				or	dx, di
			.notSet:
				shr	di, 1
				jmp	short	.nextChar
			.true:
				mov	ah, 1
				ret	
			.false:
				mov	ah, 0
				ret	
			.outOfRange:
				mov	ah, 2
				ret	

		private.Imm.getHexStringValue:
		; --------------------------------------------------------------
		; si = imm string
		; dx = value
		; ah = 0 - hex invalid format ! 
		;      1 - ok
		;      2 - hex out of range !
				call	private.Imm.isValidHex
				cmp	ah, 1
				jne	.false
				cmp	cx, 6
				jg	.outOfRange
				mov	dx, 0
				sub	cx, 1
				mov	ax, 4
				mul	cx
				mov	cx, ax
			.nextChar:
				sub	cx, 4
				mov	al, [si]
				inc	si
				cmp	al, 'H'
				je	short .true
				call	Character.isDigit
				cmp	ah, 1
				je	.digit
				call	Character.isLetterA_F
				cmp	ah, 1
				je	.letter
				jmp	short .false
			.digit:
				mov	ah, 0
				sub	al, 48
				shl	ax, cl
				or	dx, ax
				jmp	short	.nextChar
			.letter:
				mov	ah, 0
				sub	al, 55
				shl	ax, cl
				or	dx, ax
				jmp	short	.nextChar
			.true:
				mov	ah, 1
				ret	
			.false:
				mov	ah, 0
				ret	
			.outOfRange:
				mov	ah, 2
				ret	

		private.Imm.getDecimalStringValue:
		; --------------------------------------------------------------
		; si = imm string
		; dx = value
		; ah = 0 - decimal invalid format ! 
		;      1 - ok
		;      2 - decimal out of range !
				mov	word [.provResult], 0
				call	private.Imm.isValidDecimal
				cmp	ah, 1
				jne	.false
				cmp	cx, 5
				jg	.outOfRange
				dec	cx
				mov	dx, 0
			.nextChar:
				mov	al, [si]
				inc	si
				cmp	al, 0
				je	short .true
				call	Character.isDigit
				cmp	ah, 1
				je	.digit
				jmp	short .false
			.digit:
					push	cx
					push	ax
					mov	ax, 1
					mov	bx, 10
				.repeat:
					mul	bx
					loop	.repeat
					mov	bx, ax
					cmp	bx, 0
					jne	.notZeroAdjust
				.zeroAdjust:
					mov	bx, 1
				.notZeroAdjust:
					pop	ax
					pop	cx

				mov	ah, 0
				sub	al, 48
				mul	bx
				mov	dx, [.provResult]
				add	dx, ax
				jc	.outOfRange
				mov	word [.provResult], dx
				dec	cx
				jmp	short	.nextChar
			.true:
				mov	ah, 1
				ret	
			.false:
				mov	ah, 0
				ret	
			.outOfRange:
				mov	ah, 2
				ret	
			.provResult	dw 0

;===============================================================================

