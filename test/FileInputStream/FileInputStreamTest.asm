;===============================================================================
; class FileInputStreamTest
;===============================================================================
	bits	16
	org	32768
start:
	; ----------------------------------------------------------------------
	fileInputStream_setFileName:
		mov	si, fileName
		call	fileInputStream.setFileName

	; ----------------------------------------------------------------------
	fileInputStream_showFileName:
		mov	si, msg.fileNameEqual
		call	os_print_string
		mov	si, fileInputStream.file.name
		call	os_print_string
		call	os_print_newline

	; ----------------------------------------------------------------------
	fileInputStream_openFile:
			call	fileInputStream.openFile
			cmp	ah, 1
			je	fileInputStream_openFile_ok
		fileInputStream_openFile_notFound:
			mov	si, msg.fileNotFound
			call	os_print_string
			call	os_print_newline
			ret
		fileInputStream_openFile_ok:

	; ----------------------------------------------------------------------
	fileInputStream_showFileSize:
		mov	si, msg.fileSizeEqual
		call	os_print_string
		mov	ax, [fileInputStream.file.size]
		call	os_int_to_string
		mov	si, ax
		call	os_print_string
		call	os_print_newline
		call	os_wait_for_key

	; ----------------------------------------------------------------------
	fileInputStream_seeNextChar:
			call	fileInputStream.getNextChar
			call	fileInputStream.getNextChar

			mov	cx, 5
		fileInputStream_seeNextChar_again:
			push	cx
			call	fileInputStream.seeNextChar
			cmp	ah, 0
			je	fileInputStream_seeNextChar_endOfFile
			mov	si, msg.seeNextChar
			call	os_print_string
			mov	byte [character], al
			mov	si, character
			call	os_print_string
			call	os_print_newline
			pop	cx
			loop	fileInputStream_seeNextChar_again
		fileInputStream_seeNextChar_endOfFile:

	; ----------------------------------------------------------------------
	fileInputStream_showNextChar:
			call	fileInputStream.getNextChar
			cmp	ah, 0
			je	fileInputStream_getNextChar_endOfFile
			cmp	al, 13
			je	fileInputStream_getNextChar_cr
			cmp	al, 10
			je	fileInputStream_getNextChar_lf
			mov	byte [character], al
			mov	si, character
			call	os_print_string
			jmp	fileInputStream_showNextChar
		fileInputStream_getNextChar_cr:
		fileInputStream_getNextChar_lf:
			call	os_print_newline
			jmp	fileInputStream_showNextChar
		fileInputStream_getNextChar_endOfFile:

	; ----------------------------------------------------------------------
	fileInputStream.endTest:
		ret

	; ----------------------------------------------------------------------

	%include	"../../include/mikedev.inc"
	%include	"../../include/FileInputStream.inc"

	fileName		db "TESTE.ASM", 0

	msg.fileNameEqual	db "File = ", 0
	msg.fileSizeEqual	db "File size = ", 0
	msg.fileNotFound	db "File not found !", 0
	msg.seeNextChar		db "See next char = ", 0

	character		db 0, 0

;===============================================================================
