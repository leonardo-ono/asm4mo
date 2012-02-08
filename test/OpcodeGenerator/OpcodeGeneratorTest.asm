;===============================================================================
; Opcode Generator Test
;===============================================================================
	bits	16
	org	32768
start:
	; ----------------------------------------------------------------------
	OpcodeGenerator_execute:

		mov	ax, generationInstructions_wb
		call	OpcodeGenerator.execute
		
		mov	ax, generationInstructions_ww
		call	OpcodeGenerator.execute

		; print generated instructions
		mov	si, [OpcodeGenerator.startMemoryAddress]
		call	os_print_string
		call	os_print_newline

		; print size of generated instructions
		call	private.OpcodeGenerator.debug.printIndex

		; --------------------------------------------------------------
		call	os_print_newline
		call	os_print_newline

		; stack save first ib
		mov	al, 123 
		call	OpcodeGenerator.pushIb

		; stack save second ib
		mov	al, 234 
		call	OpcodeGenerator.pushIb

		; print stack first ib
		mov	si, msg.stackEqual
		call	os_print_string
		mov	al, 0
		call	OpcodeGenerator.popIb
		mov	ah, 0
		call	os_int_to_string
		mov	si, ax
		call	os_print_string
		call	os_print_newline

		; print stack second ib
		mov	si, msg.stackEqual
		call	os_print_string
		mov	al, 0
		call	OpcodeGenerator.popIb
		mov	ah, 0
		call	os_int_to_string
		mov	si, ax
		call	os_print_string
		call	os_print_newline

		; --------------------------------------------------------------
		; stack save first iw
		mov	ax, 1234
		mov	cl, 3
		mov	dx, label1
		call	OpcodeGenerator.pushIw

		; call	OpcodeGenerator.clearIbIwRMStacks

		; stack save second iw
		mov	ax, 2345
		mov	cl, 4
		mov	dx, label2
		call	OpcodeGenerator.pushIw

		mov	ax, 0ffffh
		mov	cx, 0ffffh
		mov	dx, 0ffffh

		; print stack first iw
		mov	si, msg.stackEqual
		call	os_print_string
		mov	ax, 0
		call	OpcodeGenerator.popIw

		call	os_int_to_string
		mov	si, ax
		call	os_print_string
		call	os_print_newline

		mov	ch, 0
		mov	ax, cx
		call	os_int_to_string
		mov	si, ax
		call	os_print_string
		call	os_print_newline

		mov	si, dx
		call	os_print_string
		call	os_print_newline

		; print stack second iw
		mov	si, msg.stackEqual
		call	os_print_string
		mov	ax, 0
		call	OpcodeGenerator.popIw
		call	os_int_to_string
		mov	si, ax
		call	os_print_string
		call	os_print_newline

		mov	ch, 0
		mov	ax, cx
		call	os_int_to_string
		mov	si, ax
		call	os_print_string
		call	os_print_newline

		mov	si, dx
		call	os_print_string
		call	os_print_newline

;ret
		; --------------------------------------------------------------

		call	os_print_newline
		call	os_print_newline

		call	OpcodeGenerator.clearIbIwRMStacks
		mov	al, 65 ; A
		call	OpcodeGenerator.pushIb

		mov	ax, generationInstructions_mov_reg8_imm8
		call	OpcodeGenerator.execute
		

		; --------------------------------------------------------------

		call	os_print_newline
		call	os_print_newline

		call	OpcodeGenerator.clearIbIwRMStacks

		; MOV AX, 4142H
		; db "AX", 0, 0, 0, 0, 0, 158, Token.TYPE_REG16,   0, 11000000b
		; mov reg16 , imm16 ; wC7 /0 iw

		; Token: MOV -> Token.TYPE_INSTR
		; Token: AX  -> Token.TYPE_REG16

			;   al = Id
			;   ah = Type
			;   cl = RegIndex
			;   ch = R/M
			;   dl = has extra imm8
			;   dh = extra imm8
			;   bh = has extra imm16
			;   si = extra imm16
			mov	al, 158
			mov	ah, Token.TYPE_REG16
			mov	cl, 0
			mov	ch, 11000000b
			mov	dl, 0
			mov	dh, 41h
			mov	bh, 1
			mov	si, 0abcdh
			call	OpcodeGenerator.pushRM

		; Token: ,     -> Token.TYPE_VIRGULA
		; Token: 4142H -> Token.TYPE_IMM8

			mov	ax, 4142h ; BA
			call	OpcodeGenerator.pushIw

		; Token: <enter> -> Token.TYPE_END_OF_LINE

			mov	ax, generationInstructions_mov_reg16_imm16
			call	OpcodeGenerator.execute

		; --------------------------------------------------------------

		call	os_print_newline
		call	os_print_newline

		call	OpcodeGenerator.clearIbIwRMStacks

		; MOV [BX+SI+3H], 1234h
		; [ BX + SI + imm8 ] 01 000 40 48 50 58 60 68 70 78 ib
		; MOV mem , imm16 ; o16 wC7 /0 iw 
		
		; Token: MOV -> Token.TYPE_INSTR
		; Token: [BX+SI+3H]  -> Token.TYPE_MEM

			;   al = Id
			;   ah = Type
			;   cl = RegIndex
			;   ch = R/M
			;   dl = has extra imm8
			;   dh = extra imm8
			;   bh = has extra imm16
			;   si = extra imm16
			mov	al, 0
			mov	ah, Token.TYPE_MEM
			mov	cl, 0
			mov	ch, 01000000b
			mov	dl, 1
			mov	dh, 3h
			mov	bh, 0
			mov	si, 0
			call	OpcodeGenerator.pushRM

		; Token: ,     -> Token.TYPE_VIRGULA
		; Token: 1234h -> Token.TYPE_IMM16

			mov	ax, 1234h 
			call	OpcodeGenerator.pushIw

		; Token: <enter> -> Token.TYPE_END_OF_LINE

			mov	ax, generationInstructions_mov_mem_imm16
			call	OpcodeGenerator.execute


		; --------------------------------------------------------------

		call	os_print_newline
		call	os_print_newline

		call	OpcodeGenerator.clearIbIwRMStacks

		; cmp [bx+si+5], 1234h
		; [ BX + SI + imm8 ] 01 000 40 48 50 58 60 68 70 78 ib
		; mov mem , imm16
		; db  15, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0 ; CMP mem , imm16 
		; dw opcode_CMP_GenInstr_91
		
		; Token: CMP -> Token.TYPE_INSTR
		; Token: [BX+SI+5H]  -> Token.TYPE_MEM

			;   al = Id
			;   ah = Type
			;   cl = RegIndex
			;   ch = R/M
			;   dl = has extra imm8
			;   dh = extra imm8
			;   bh = has extra imm16
			;   si = extra imm16
			mov	al, Token.ID_MEM
			mov	ah, Token.TYPE_MEM
			mov	cl, 0
			mov	ch, 01000000b
			mov	dl, 1
			mov	dh, 5h
			mov	bh, 0
			mov	si, 0
			call	OpcodeGenerator.pushRM

		; Token: ,     -> Token.TYPE_VIRGULA
		; Token: 1234h -> Token.TYPE_IMM16

			mov	ax, 1234h 
			call	OpcodeGenerator.pushIw

		; Token: <enter> -> Token.TYPE_END_OF_LINE

			mov	ax, opcode_CMP_GenInstr_91t
			call	OpcodeGenerator.execute


		; --------------------------------------------------------------

		call	os_print_newline
		call	os_print_newline

		call	OpcodeGenerator.clearIbIwRMStacks

		; mov ch, bl
		; db  75, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; MOV reg8 , reg8 
		; dw  opcode_MOV_GenInstr_192t


		; Token: mov -> Token.TYPE_INSTR
		; Token: ch  -> Token.REG8
		; db "CH", 0, 0, 0, 0, 0, 155, Token.TYPE_REG8,    5, 11000101b

			;   al = Id
			;   ah = Type
			;   cl = RegIndex
			;   ch = R/M
			;   dl = has extra imm8
			;   dh = extra imm8
			;   bh = has extra imm16
			;   si = extra imm16
			mov	al, 155
			mov	ah, Token.TYPE_REG8
			mov	cl, 5
			mov	ch, 11000101b
			mov	dl, 0
			mov	dh, 0
			mov	bh, 0
			mov	si, 0
			call	OpcodeGenerator.pushRM

		; Token: ,  -> Token.TYPE_VIRGULA
		; Token: bl -> Token.TYPE_REG8
		;db "BL", 0, 0, 0, 0, 0, 153, Token.TYPE_REG8,    3, 11000011b

			;   al = Id
			;   ah = Type
			;   cl = RegIndex
			;   ch = R/M
			;   dl = has extra imm8
			;   dh = extra imm8
			;   bh = has extra imm16
			;   si = extra imm16
			mov	al, 153
			mov	ah, Token.TYPE_REG8
			mov	cl, 3
			mov	ch, 11000011b
			mov	dl, 0
			mov	dh, 0
			mov	bh, 0
			mov	si, 0
			call	OpcodeGenerator.pushRM

		; Token: <enter> -> Token.TYPE_END_OF_LINE

			mov	ax, opcode_MOV_GenInstr_192t
			call	OpcodeGenerator.execute


		; --------------------------------------------------------------

		; print size of generated instructions
		call	private.OpcodeGenerator.debug.printIndex

	; ----------------------------------------------------------------------
	OpcodeGenerator_end:
	ret

	; ----------------------------------------------------------------------

	%include	"../../include/mikedev.inc"
	%include	"../../include/FileInputStream.inc"
	%include	"../../include/Character.inc"
	%include	"../../include/Parser.inc"
	%include	"../../include/OpcodeGenerator.inc"
	%include	"../../include/Token.inc"
	%include	"../../include/Keywords.inc"
	%include	"../../include/InstructionsHandler.inc"
	%include	"../../include/Mem.inc"
	%include	"../../include/LabelResolver.inc"

	label1		db "aaaaaaaaaaaab", 0
	label2		db "ccccccccccccd", 0

	msg.stackEqual	db "Stack = ", 0

	generationInstructions_wb	db OpcodeGenerator.INSTR_WB, 41h	
					db OpcodeGenerator.INSTR_WB, 42h
					db OpcodeGenerator.INSTR_WB, 43h
					db 0h

	generationInstructions_ww	db OpcodeGenerator.INSTR_WW		
					dw 04142h,
					db OpcodeGenerator.INSTR_WW		
					dw 04344h, 
					db OpcodeGenerator.INSTR_WB, 0h
					db 0h

	; mov reg8, imm8 -> wC6 /0 ib 
	generationInstructions_mov_reg8_imm8	db OpcodeGenerator.INSTR_WB, 0C6h
						db OpcodeGenerator.INSTR_IB
						db 0h


	; MOV AX, 4142H
	; db "AX", 0, 0, 0, 0, 0, 158, Token.TYPE_REG16,   0, 11000000b
	; mov reg16 , imm16 ; wC7 /0 iw
	generationInstructions_mov_reg16_imm16	db OpcodeGenerator.INSTR_WB, 0C7h
						db OpcodeGenerator.INSTR_BARRA_7
						db OpcodeGenerator.INSTR_IW
						db 0h

	; MOV [BX+SI+3H], DX
	; [ BX + SI + imm8 ] 01 000 40 48 50 58 60 68 70 78 ib
	; MOV mem , imm16 ; o16 wC7 /0 iw 
	generationInstructions_mov_mem_imm16	db OpcodeGenerator.INSTR_WB, 0C0h
						db OpcodeGenerator.INSTR_BARRA_0
						db OpcodeGenerator.INSTR_IW
						db 0h

	; cmp [bx+si+5], 1234h
	; [ BX + SI + imm8 ] 01 000 40 48 50 58 60 68 70 78 ib
	; mov mem , imm16
	; db  15, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0 ; CMP mem , imm16 
	; dw opcode_CMP_GenInstr_91

	opcode_CMP_GenInstr_91t	db OpcodeGenerator.INSTR_WB, 081h
				db OpcodeGenerator.INSTR_BARRA_7
				db OpcodeGenerator.INSTR_IW, 0  ; o16 w81 /7 iw

	; mov ch, bl
	; db  75, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; MOV reg8 , reg8 
	; dw  opcode_MOV_GenInstr_192t
	opcode_MOV_GenInstr_192t	db OpcodeGenerator.INSTR_WB, 088h
					db OpcodeGenerator.INSTR_RM_1, 0  ; w88 /r1


;===============================================================================
