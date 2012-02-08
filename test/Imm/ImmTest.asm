;===============================================================================
; class TokenTest
;===============================================================================
	bits	16
	org	32768
start:
	;---------------------------------------------------------
	testBinaryValue:
			mov	si, binaryValue
			call	private.Imm.isValidBinary
			cmp	ah, 1
			jne	.invalid
		.valid:
			mov	si, msg.binaryValue.valid
			call	os_print_string
			call	os_print_newline

			mov	si, msg.binary.stringLengthEqual
			call	os_print_string

			mov	ax, cx
			call	os_int_to_string
			mov	si, ax
			call	os_print_string
			call	os_print_newline

			jmp	testBinaryValue.end
		.invalid:
			mov	si, msg.binaryValue.invalid
			call	os_print_string
			call	os_print_newline
			jmp	testBinaryValue.end
	testBinaryValue.end:
			call	os_print_newline

	;---------------------------------------------------------
	testHexValue:
			mov	si, hexValue
			call	private.Imm.isValidHex
			cmp	ah, 1
			jne	.invalid
		.valid:
			mov	si, msg.hexValue.valid
			call	os_print_string
			call	os_print_newline

			mov	si, msg.hex.stringLengthEqual
			call	os_print_string

			mov	ax, cx
			call	os_int_to_string
			mov	si, ax
			call	os_print_string
			call	os_print_newline

			jmp	testHexValue.end
		.invalid:
			mov	si, msg.hexValue.invalid
			call	os_print_string
			call	os_print_newline
			jmp	testHexValue.end
	testHexValue.end:
			call	os_print_newline

	;---------------------------------------------------------
	testDecimalValue:
			mov	si, decimalValue
			call	private.Imm.isValidDecimal
			cmp	ah, 1
			jne	.invalid
		.valid:
			mov	si, msg.decimalValue.valid
			call	os_print_string
			call	os_print_newline

			mov	si, msg.decimal.stringLengthEqual
			call	os_print_string

			mov	ax, cx
			call	os_int_to_string
			mov	si, ax
			call	os_print_string
			call	os_print_newline

			jmp	testDecimalValue.end
		.invalid:
			mov	si, msg.decimalValue.invalid
			call	os_print_string
			call	os_print_newline
			jmp	testDecimalValue.end
	testDecimalValue.end:
			call	os_print_newline

	;---------------------------------------------------------
	testGetBinaryStringValue:
			mov	si, binaryValue
			call	private.Imm.getBinaryStringValue
			cmp	ah, 2
			je	.outOfRange
			cmp	ah, 1
			jne	.invalid
		.valid:
			mov	ax, dx
			call	os_int_to_string
			mov	si, ax
			call	os_print_string
			call	os_print_newline
			jmp	testGetBinaryStringValue.end
		.invalid:
			mov	si, msg.binaryValue.invalid
			call	os_print_string
			call	os_print_newline
			jmp	testGetBinaryStringValue.end
		.outOfRange:
			mov	si, msg.binaryValue.outOfRange
			call	os_print_string
			call	os_print_newline
			jmp	testGetBinaryStringValue.end
	testGetBinaryStringValue.end:
			call	os_print_newline

	;---------------------------------------------------------
	testGetHexStringValue:
			mov	si, hexValue
			call	private.Imm.getHexStringValue
			cmp	ah, 2
			je	.outOfRange
			cmp	ah, 1
			jne	.invalid
		.valid:
			mov	ax, dx
			call	os_int_to_string
			mov	si, ax
			call	os_print_string
			call	os_print_newline
			jmp	testGetHexStringValue.end
		.invalid:
			mov	si, msg.hexValue.invalid
			call	os_print_string
			call	os_print_newline
			jmp	testGetHexStringValue.end
		.outOfRange:
			mov	si, msg.hexValue.outOfRange
			call	os_print_string
			call	os_print_newline
			jmp	testGetHexStringValue.end
	testGetHexStringValue.end:
			call	os_print_newline

	;---------------------------------------------------------
	testGetDecimalStringValue:
			mov	si, decimalValue
			call	private.Imm.getDecimalStringValue
			cmp	ah, 2
			je	.outOfRange
			cmp	ah, 1
			jne	.invalid
		.valid:
			call	os_print_newline
			mov	ax, dx
			call	os_int_to_string
			mov	si, ax
			call	os_print_string
			call	os_print_newline
			jmp	testGetDecimalStringValue.end
		.invalid:
			mov	si, msg.decimalValue.invalid
			call	os_print_string
			call	os_print_newline
			jmp	testGetDecimalStringValue.end
		.outOfRange:
			mov	si, msg.decimalValue.outOfRange
			call	os_print_string
			call	os_print_newline
			jmp	testGetDecimalStringValue.end
	testGetDecimalStringValue.end:
			call	os_print_newline

;---------------------------------------------------------
end:

	ret

	%include	"../../include/Character.inc"
	%include	"../../include/Imm.inc"
	%include	"../../include/mikedev.inc"

	binaryValue			db "1111111111111111B", 0
	msg.binary.stringLengthEqual	db "Binary string length=", 0
	msg.binaryValue.valid		db "Binary value valid !", 0
	msg.binaryValue.invalid		db "Binary value invalid !", 0
	msg.binaryValue.outOfRange	db "Binary value out of range !", 0

	hexValue			db "0FFFFH", 0
	msg.hex.stringLengthEqual	db "Hex string length=", 0
	msg.hexValue.valid		db "Hex value valid !", 0
	msg.hexValue.invalid		db "Hex value invalid !", 0
	msg.hexValue.outOfRange		db "Hex value out of range !", 0

	decimalValue			db "1", 0
	msg.decimal.stringLengthEqual	db "Decimal string length=", 0
	msg.decimalValue.valid		db "Decimal value valid !", 0
	msg.decimalValue.invalid	db "Decimal value invalid !", 0
	msg.decimalValue.outOfRange	db "Decimal value out of range !", 0

;===============================================================================
