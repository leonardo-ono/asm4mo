;===============================================================================
; class ParserTest
;===============================================================================
	bits	16
	org	32768
start:
	; ----------------------------------------------------------------------
	parser_setFileName:
		mov	si, fileName
		call	parser.setFileName

	; ----------------------------------------------------------------------
	parser_showFileName:
		mov	si, msg.fileNameEqual
		call	os_print_string
		mov	si, parser.file.name
		call	os_print_string
		call	os_print_newline

	; ----------------------------------------------------------------------
	parser_start:
			call	parser.start
			cmp	ah, 1
			je	parser_start_ok
		parser_start_fileNotFound:
			mov	si, msg.fileNotFound
			call	os_print_string
			call	os_print_newline
			ret
		parser_start_ok:
			mov	si, msg.parserStartOk
			call	os_print_string
			call	os_print_newline

	; ----------------------------------------------------------------------
	parser_getNextToken:
			mov	si, msg.tokenEqual
			call	os_print_string
			call	parser.getNextToken
			cmp	ah, 0
			je	parser_getNextToken_end
			mov	si, token.value
			call	os_print_string

			mov	si, msg.tokenTypeEqual
			call	os_print_string

			mov	ah, 0
			mov	al, [token.type]
			call	os_int_to_string
			mov	si, ax
			call	os_print_string

			mov	si, msg.tokenIdEqual
			call	os_print_string
				
			mov	ah, 0
			mov	al, [token.id]
			call	os_int_to_string
			mov	si, ax
			call	os_print_string

			call	os_print_newline

			mov	al, [token.type]
			cmp	al, 0 ; Token.TYPE_INSTR
			jne	parser_getNextToken_not_invoke_handler
		parser_getNextToken_invoke_handler:

			mov	si, msg.invokingHandler
			call	os_print_string
			call	os_print_newline
			call	[token.handler]

		parser_getNextToken_not_invoke_handler:
			jmp	parser_getNextToken
		parser_getNextToken_end:
	; ----------------------------------------------------------------------
	parser_end:
	ret

	; ----------------------------------------------------------------------

	%include	"../../include/mikedev.inc"
	%include	"../../include/FileInputStream.inc"
	%include	"../../include/Token.inc"
	%include	"../../include/Character.inc"
	%include	"../../include/Parser.inc"
	%include	"../../include/Keywords.inc"
	%include	"../../include/OpcodeGenerator.inc"
	%include	"../../include/InstructionsHandler.inc"

	fileName		db "TESTE.ASM", 0

	msg.fileNameEqual	db "File = ", 0
	msg.fileNotFound	db "File not found !", 0

	msg.parserStartOk	db "parser.start ok !", 0
	msg.tokenEqual		db "Token = ", 0
	msg.tokenTypeEqual	db " / Type = ", 0
	msg.tokenIdEqual	db " / Id = ", 0
	msg.invokingHandler	db "Invoking handler ...", 0

;===============================================================================
