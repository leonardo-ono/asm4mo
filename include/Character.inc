;===============================================================================
; class Character
;===============================================================================

	; private static properties
	; ----------------------------------------------------------------------
	Character.validCharacters	db "abcdefghijklmnopqrstuvxwyz"
					db "ABCDEFGHIJKLMNOPQRSTUVXWYZ"
					db "1234567890", ":;[+]", 0

	; public static methods
	; ----------------------------------------------------------------------

		Character.isValid:
		; --------------------------------------------------------------
		; al = ASCII char code
		; ah = 0 - false
		;      1 - true
				mov	si, Character.validCharacters
				mov	bx, 0
			Character.isValid.next:
				mov	dl, [bx + si]
				cmp	dl, 0
				jz	Character.isValid.false
				cmp	al, dl
				jz	Character.isValid.true
				inc	bx
				jmp	Character.isValid.next
			Character.isValid.false:
				mov    ah, 0
				ret
			Character.isValid.true:
				mov    ah, 1
				ret

		Character.isDigit:
		; --------------------------------------------------------------
		; al = ASCII char code
		; ah = 0 - false
		;      1 - true
				cmp	al, 48
				jge	Character.isDigit.lowOk
				mov	ah, 0
				ret
			Character.isDigit.lowOk:
				cmp	al, 57
				jle	Character.isDigit.highOk
				mov	ah, 0
				ret
			Character.isDigit.highOk:
				mov	ah, 1
				ret

		Character.isLetter:
		; --------------------------------------------------------------
		; al = ASCII char code
		; ah = 0 - false
		;      1 - true
				cmp	al, 65
				jge	Character.isLetter.upper.lowOk
				jmp	Character.isLetter.low
			Character.isLetter.upper.lowOk:
				cmp	al, 90
				jle	Character.isLetter.highOk
			Character.isLetter.low:
				cmp	al, 97
				jge	Character.isLetter.lowOk
				mov	ah, 0
				ret
			Character.isLetter.lowOk:
				cmp	al, 122
				jle	Character.isLetter.highOk
				mov	ah, 0
				ret
			Character.isLetter.highOk:
				mov	ah, 1
				ret

		Character.isLetterA_F:
		; --------------------------------------------------------------
		; al = ASCII char code
		; ah = 0 - false
		;      1 - true
				cmp	al, 65
				jge	.upper.lowOk
				jmp	.low
			.upper.lowOk:
				cmp	al, 70
				jle	.highOk
			.low:
				cmp	al, 97
				jge	.lowOk
				mov	ah, 0
				ret
			.lowOk:
				cmp	al, 102
				jle	.highOk
				mov	ah, 0
				ret
			.highOk:
				mov	ah, 1
				ret

		Character.toUpper:
		; --------------------------------------------------------------
		; al = ASCII char code to upper
		; return al = ASCII char code upped
				call	Character.isLetter
				cmp	ah, 1
				jne	Character.toUpper.notIsLetter
			Character.toUpper.IsLetter:
				cmp	al, 97
				jl	Character.isLetter.alreadyUpper
				sub	al, 32	; to upper
			Character.isLetter.alreadyUpper:	
			Character.toUpper.notIsLetter:
				ret

;===============================================================================

