;===============================================================================
; class TokenTest
;===============================================================================
	bits	16
	org	32768
start:
	call	token.clear

	mov	al, 'a'
	call	token.addChar
	mov	al, 'D'
	call	token.addChar
	mov	al, 'C'
	call	token.addChar

	mov	si, msg.token_equal
	call	os_print_string

	mov	si, token.value
	call	os_print_string
	call	os_print_newline

	call	token.classify

	mov	si, msg.token_id_equal
	call	os_print_string

	mov	ah, 0
	mov	al, [token.id]
	call	os_int_to_string
	mov	si, ax
	call	os_print_string
	call	os_print_newline

	mov	si, msg.token_type_equal
	call	os_print_string

	mov	ah, 0
	mov	al, [token.type]
	call	os_int_to_string
	mov	si, ax
	call	os_print_string
	call	os_print_newline

	; call	os_wait_for_key

	ret
	%include	"../../include/Character.inc"
	%include	"../../include/mikedev.inc"
	%include	"../../include/FileInputStream.inc"
	%include	"../../include/Token.inc"
	%include	"../../include/Parser.inc"
	%include	"../../include/Keywords.inc"
	%include	"../../include/OpcodeGenerator.inc"
	%include	"../../include/InstructionsHandler.inc"

	msg.token_equal		db "Token = ", 0
	msg.token_id_equal	db "Token id = ", 0
	msg.token_type_equal	db "Token type = ", 0
;===============================================================================
