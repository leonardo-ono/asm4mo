;===============================================================================
; class Instructions Handler
;===============================================================================

	; public static constants
	; ----------------------------------------------------------------------
	InstructionHandler.VALUE_LENGTH		equ 8


	testInstr	db OG.WB, 0C7h	
			db OG.B0
			db OG.IW
			db 0h

	; ----------------------------------------------------------------------
	InstructionHandler.value			db 0, 0, 0, 0, 0, 0, 0
	InstructionHandler.index			db 0
	InstructionHandler.opcodeGenerationPointer	dw 0


	; public methods
	; ----------------------------------------------------------------------

		InstructionHandler.clear:
		; --------------------------------------------------------------
				mov	byte [InstructionHandler.index], 0
				mov	cx, InstructionHandler.VALUE_LENGTH
				mov	bx, InstructionHandler.value
			.clearNextChar:
				mov	byte [bx], 0
				inc	bx
				loop	.clearNextChar
				ret

		InstructionHandler.addTokenType:
		; --------------------------------------------------------------
		; al = token type
		; ah = 0 - overflow !
		;      1 - success
				mov	bh, 0
				mov	bl, [InstructionHandler.index]
				cmp	bl, InstructionHandler.VALUE_LENGTH
				je	.overflow
			.success:
				mov	di, InstructionHandler.value
				mov	byte [bx + di], al
				inc	byte [InstructionHandler.index]
				mov	ah, 1
				ret
			.overflow:
				mov	ah, 0
				ret

		InstructionHandler.classify:
		; --------------------------------------------------------------
		; ah = 0 - not is valid instruction !
		;      1 - success
				mov	si, InstructionHandler.possibleInstructions
				mov	di, InstructionHandler.value
			.compareNext:
				cmp	byte [si], 0
				jz	.false
				call	os_string_compare
				jc	.true
				add	si, InstructionHandler.VALUE_LENGTH
				jmp	.compareNext
			.true:
				mov	ax, [si + InstructionHandler.VALUE_LENGTH - 2]
				mov	word [InstructionHandler.opcodeGenerationPointer], ax
				mov	ah, 1
				ret
			.false:
				mov	ah, 0
				ret

;===============================================================================

		DEFAULT_handler:
		; --------------------------------------------------------------
		; ah = 0 - means that default handler must be called
		;      1 - the instruction was handled here and must not call default handler
				jmp	short DEFAULT_handler_start
				DEFAULT_handler_msg	db "   DEFAULT_handler invoked.", 0
			DEFAULT_handler_start:
				mov	si, DEFAULT_handler_msg
				call	os_print_string
				call	os_print_newline
				mov	ah, 0 ; <--------------- important !
				ret

		ORG_handler:
		; --------------------------------------------------------------
				mov	si, ORG_handler_msg
				call	os_print_string
				call	os_print_newline

				call	parser.getNextToken
				mov	al, [token.type]
				cmp	al, Token.TYPE_IMM8
				je	.orgImm
				cmp	al, Token.TYPE_IMM16
				je	.orgImm
				jmp	.error

			.orgImm:
				mov	si, token.value
				mov	ax, [token.iw]
				mov	word [OpcodeGenerator.offset], ax

				call	parser.getNextToken
				mov	al, [token.type]
				cmp	al, Token.TYPE_END_OF_LINE
				je	.ok

			.error:
				mov	ah, 2
				ret
			.ok:
				mov	ah, 1
				ret
			ORG_handler_msg	db "ORG_handler invoked !", 0


		DB_handler:
		; --------------------------------------------------------------
				mov	si, DB_handler_msg
				call	os_print_string
				call	os_print_newline
			.nextToken:
				call	parser.getNextToken
				mov	al, [token.type]
				cmp	al, Token.TYPE_IMM8
				je	.dbImm8
				cmp	al, Token.TYPE_STRING
				je	.dbString
				jmp	.error

			.dbImm8:
				mov	si, token.value
				mov	ax, [token.iw]
				call	private.OpcodeGenerator.writeByte
				jmp	.verifyHasMoreArguments

			.dbString:
					mov	bx, token.value
					mov	cl, [bx] ; single-quote or double-quote ?
					inc	bx
					mov	si, bx
					call	os_print_string
					call	os_print_newline

				.dbString.nextChar:
					mov	al, [bx]
					cmp	al, cl
					je	.dbString.end
					push	bx
					call	private.OpcodeGenerator.writeByte
					pop	bx
					inc	bx
					jmp	short .dbString.nextChar
				.dbString.end:
					jmp	.verifyHasMoreArguments

			.verifyHasMoreArguments:
				call	parser.getNextToken
				mov	al, [token.type]
				cmp	al, Token.TYPE_VIRGULA
				je	.nextToken
			.noMoreArgument:
				; expected now end of line
				mov	al, [token.type]
				cmp	al, Token.TYPE_END_OF_LINE
				je	.ok
			.error:
				mov	ah, 2
				ret
			.ok:
				mov	ah, 1
				ret
			DB_handler_msg	db "DB_handler invoked !", 0


		DW_handler:
		; --------------------------------------------------------------
				mov	si, DW_handler_msg
				call	os_print_string
				call	os_print_newline
			.nextToken:
				call	parser.getNextToken
				mov	al, [token.type]
				cmp	al, Token.TYPE_IMM8
				je	.dbImm8
				mov	al, [token.type]
				cmp	al, Token.TYPE_IMM16
				je	.dbImm16

			.dbImm8:
				mov	si, token.value
				mov	ax, [token.iw]
				mov	ah, 0
				call	private.OpcodeGenerator.writeWord
				jmp	.verifyHasMoreArguments

			.dbImm16:
				mov	si, token.value
				mov	ax, [token.iw]
				call	private.OpcodeGenerator.writeWord
				jmp	.verifyHasMoreArguments

			.verifyHasMoreArguments:
				call	parser.getNextToken
				mov	al, [token.type]
				cmp	al, Token.TYPE_VIRGULA
				je	.nextToken
			.noMoreArgument:
				; expected now end of line
				mov	al, [token.type]
				cmp	al, Token.TYPE_END_OF_LINE
				je	.ok
			.error:
				mov	ah, 2
				ret
			.ok:
				mov	ah, 1
				ret
			DW_handler_msg	db "DW_handler invoked !", 0


		IN_handler:
		; --------------------------------------------------------------
		; ah = 0 - means that default handler must be called
		;      1 - the instruction was handled here and must not call default handler
		;      2 - Error
				mov	si, IN_handler_msg
				call	os_print_string
				call	os_print_newline

				call	parser.getNextToken

				mov	al, [token.id]
				cmp	al, ID_AL   ; 1st arg AL ?
				je	.arg1IsAL

				cmp	al, ID_AX   ; 1st arg AX ?
				je	.arg1IsAX

				jmp	short .error.expectedALorAX

			.arg1IsAL:
				call	parser.getNextToken

				mov	al, [token.type]
				cmp	al, Token.TYPE_VIRGULA ; 2nd token must be comma
				jne	.error.expectedComma

				call	parser.getNextToken

				mov	al, [token.id]
				cmp	al, ID_DX           ; 2nd arg DX ?
				je	.arg1IsALarg2IsDX

				mov	al, [token.type]
				cmp	al, Token.TYPE_IMM8 ; 2nd arg imm8 ?
				je	.arg1IsALarg2IsImm8

				jmp	short .error.expectedDXorImm8

			.arg1IsAX:
				call	parser.getNextToken

				mov	al, [token.type]
				cmp	al, Token.TYPE_VIRGULA ; 2nd token must be comma
				jne	.error.expectedComma

				call	parser.getNextToken

				mov	al, [token.id]
				cmp	al, ID_DX           ; 2nd arg DX ?
				je	.arg2IsAXarg2IsDX

				mov	al, [token.type]
				cmp	al, Token.TYPE_IMM8 ; 2nd arg imm8 ?
				je	.arg2IsAXarg2IsIm8

				jmp	short .error.expectedDXorImm8

			.arg1IsALarg2IsImm8:
				mov	ax, [token.iw]
				call	OpcodeGenerator.pushIb
				mov	ax, opcode_IN_GenInstr_120
				jmp	short .generateOpcode
			.arg2IsAXarg2IsIm8:
				mov	ax, [token.iw]
				call	OpcodeGenerator.pushIb
				mov	ax, opcode_IN_GenInstr_121
				jmp	short .generateOpcode
			.arg1IsALarg2IsDX:
				mov	ax, opcode_IN_GenInstr_122
				jmp	short .generateOpcode
			.arg2IsAXarg2IsDX:
				mov	ax, opcode_IN_GenInstr_123
				jmp	short .generateOpcode

			.generateOpcode:
				call	OpcodeGenerator.execute
				mov	ah, 1
				ret

			.error.expectedALorAX:
				mov	si, .msg.error.expectedALorAX
				call	os_print_string
				call	os_print_newline
				mov	ah, 2
				ret

			.error.expectedDXorImm8:
				mov	si, .msg.error.expectedDXorImm8
				call	os_print_string
				call	os_print_newline
				mov	ah, 2
				ret

			.error.expectedComma:
				mov	si, .msg.error.expectedComma
				call	os_print_string
				call	os_print_newline
				mov	ah, 2
				ret
			.msg.error.expectedALorAX db " Error: 1st arg expected AL or AX !", 0
			.msg.error.expectedDXorImm8 db " Error: 2nd arg expected DX or imm8 !", 0
			.msg.error.expectedComma db " Error: expected comma !", 0
			IN_handler_msg	db "IN_handler invoked !", 0
				
		; IN AL,imm8                    ; E4 ib                [8086]
		; IN AX,imm8                    ; E5 ib                [8086]
		; IN AL,DX                      ; EC                   [8086]
		; IN AX,DX                      ; ED                   [8086]				

		; opcode_IN_GenInstr_120 db 3, OG.WB, 0E4h, OG.IB ; wE4 ib amb1AL
		; opcode_IN_GenInstr_121 db 3, OG.WB, 0E5h, OG.IB ; wE5 ib amb1AX
		; opcode_IN_GenInstr_122 db 2, OG.WB, 0ECh        ; wEC amb1AL
		; opcode_IN_GenInstr_123 db 2, OG.WB, 0EDh        ; wED amb1AX

		OUT_handler:
		; --------------------------------------------------------------
		; ah = 0 - means that default handler must be called
		;      1 - the instruction was handled here and must not call default handler
		;      2 - Error
				mov	si, OUT_handler_msg
				call	os_print_string
				call	os_print_newline

				call	parser.getNextToken

				mov	al, [token.id]
				cmp	al, ID_DX           ; 1st arg DX ?
				je	.arg1IsDX

				mov	al, [token.type]
				cmp	al, Token.TYPE_IMM8 ; 1st arg imm8 ?
				je	.arg1IsImm8

				jmp	short .error.expectedDXorImm8

			.arg1IsDX:
				call	parser.getNextToken

				mov	al, [token.type]
				cmp	al, Token.TYPE_VIRGULA ; 2nd token must be comma
				jne	.error.expectedComma

				call	parser.getNextToken

				mov	al, [token.id]
				cmp	al, ID_AL           ; 2nd arg AL ?
				je	.arg1IsDXarg2IsAL

				cmp	al, ID_AX           ; 2nd arg AX ?
				je	.arg1IsDXarg2IsAX

				jmp	short .error.expectedALorAX

			.arg1IsImm8:
				call	parser.getNextToken

				mov	al, [token.type]
				cmp	al, Token.TYPE_VIRGULA ; 2nd token must be comma
				jne	.error.expectedComma

				call	parser.getNextToken

				mov	al, [token.id]
				cmp	al, ID_AL           ; 2nd arg AL ?
				je	.arg1IsImm8arg2IsAL

				cmp	al, ID_AX           ; 2nd arg AX ?
				je	.arg1IsImm8arg2IsAX

				jmp	short .error.expectedALorAX

			.arg1IsImm8arg2IsAL:
				mov	ax, [token.iw]
				call	OpcodeGenerator.pushIb
				mov	ax, opcode_OUT_GenInstr_251
				jmp	short .generateOpcode
			.arg1IsImm8arg2IsAX:
				mov	ax, [token.iw]
				call	OpcodeGenerator.pushIb
				mov	ax, opcode_OUT_GenInstr_252
				jmp	short .generateOpcode
			.arg1IsDXarg2IsAL:
				mov	ax, opcode_OUT_GenInstr_253
				jmp	short .generateOpcode
			.arg1IsDXarg2IsAX:
				mov	ax, opcode_OUT_GenInstr_254
				jmp	short .generateOpcode

			.generateOpcode:
				call	OpcodeGenerator.execute
				mov	ah, 1
				ret

			.error.expectedALorAX:
				mov	si, .msg.error.expectedALorAX
				call	os_print_string
				call	os_print_newline
				mov	ah, 2
				ret

			.error.expectedDXorImm8:
				mov	si, .msg.error.expectedDXorImm8
				call	os_print_string
				call	os_print_newline
				mov	ah, 2
				ret

			.error.expectedComma:
				mov	si, .msg.error.expectedComma
				call	os_print_string
				call	os_print_newline
				mov	ah, 2
				ret
			.msg.error.expectedDXorImm8 db " Error: 1st arg expected DX or imm8 !", 0
			.msg.error.expectedALorAX db " Error: 2nd arg expected AL or AX !", 0
			.msg.error.expectedComma db " Error: expected comma !", 0
			OUT_handler_msg	db "OUT_handler invoked !", 0
				
		; OUT imm8,AL                   ; E6 ib                [8086]	
		; OUT imm8,AX                   ; E7 ib                [8086]
		; OUT DX,AL                     ; EE                   [8086]
		; OUT DX,AX                     ; EF                   [8086]		

                ; opcode_OUT_GenInstr_251 db 3, OG.WB, 0E6h, OG.IB ; E6 ib
                ; opcode_OUT_GenInstr_252 db 3, OG.WB, 0E7h, OG.IB ; E7 ib
                ; opcode_OUT_GenInstr_253 db 2, OG.WB, 0EEh,       ; EE
                ; opcode_OUT_GenInstr_254 db 2, OG.WB, 0EFh,       ; EF

		POP_handler:
		; --------------------------------------------------------------
		; Description: handles -> POP DS, POP ES or POP SS
		; ah = 0 - means that default handler must be called
		;      1 - the instruction was handled here and must not call default handler
		;      2 - Error
				mov	si, POP_handler_msg
				call	os_print_string
				call	os_print_newline

				call	parser.getNextToken

				mov	al, [token.id]
				cmp	al, ID_DS   ; 1st arg DS ?
				je	.arg1IsDS

				cmp	al, ID_ES   ; 1st arg ES ?
				je	.arg1IsES

				cmp	al, ID_SS   ; 1st arg SS ?
				je	.arg1IsSS

				jmp	short .notIsDS_ES_SS

			.arg1IsDS:
				mov	ax, opcode_POP_GenInstr_258
				jmp	short .verifyNextTokenEndOfLine
			.arg1IsES:
				mov	ax, opcode_POP_GenInstr_259
				jmp	short .verifyNextTokenEndOfLine
			.arg1IsSS:
				mov	ax, opcode_POP_GenInstr_260
				jmp	short .verifyNextTokenEndOfLine

			.generateOpcode:
				call	OpcodeGenerator.execute
				mov	ah, 1
				ret

			.error.expectedEndOfLine:
				mov	si, .msg.error.expectedEndOfLine
				call	os_print_string
				call	os_print_newline
				mov	ah, 2
				ret

			.notIsDS_ES_SS:
				; so must try DEFAULT HANDLER
				call	parser.backToPreviousToken
				mov	ah, 0
				ret

			.verifyNextTokenEndOfLine:
				push	ax
				call	parser.getNextToken
				pop	ax
				mov	bl, [token.type]
				cmp	bl, Token.TYPE_END_OF_LINE ; 2nd token must be end of line
				jne	.error.expectedEndOfLine
				jmp	short .generateOpcode

			.msg.error.expectedEndOfLine db " Error: expected end of line !", 0
			POP_handler_msg	db "POP_handler invoked !", 0
				
		; POP DS                        ; 1F                   [8086]
		; POP ES                        ; 07                   [8086]
		; POP SS                        ; 17                   [8086]

		; opcode_POP_GenInstr_258 db 2, OG.WB, 01Fh ; 1F amb1DS
		; opcode_POP_GenInstr_259 db 2, OG.WB, 007h ; 07 amb1ES
		; opcode_POP_GenInstr_260 db 2, OG.WB, 017h ; 17 amb1SS

		PUSH_handler:
		; --------------------------------------------------------------
		; Description: handles -> PUSH CS, POP DS, POP ES or POP SS
		; ah = 0 - means that default handler must be called
		;      1 - the instruction was handled here and must not call default handler
		;      2 - Error
				mov	si, PUSH_handler_msg
				call	os_print_string
				call	os_print_newline

				call	parser.getNextToken

				mov	al, [token.id]
				cmp	al, ID_CS   ; 1st arg CS ?
				je	.arg1IsCS

				cmp	al, ID_DS   ; 1st arg DS ?
				je	.arg1IsDS

				cmp	al, ID_ES   ; 1st arg ES ?
				je	.arg1IsES

				cmp	al, ID_SS   ; 1st arg SS ?
				je	.arg1IsSS

				jmp	short .notIsCS_DS_ES_SS

			.arg1IsCS:
				mov	ax, opcode_PUSH_GenInstr_264
				jmp	short .verifyNextTokenEndOfLine
			.arg1IsDS:
				mov	ax, opcode_PUSH_GenInstr_265
				jmp	short .verifyNextTokenEndOfLine
			.arg1IsES:
				mov	ax, opcode_PUSH_GenInstr_266
				jmp	short .verifyNextTokenEndOfLine
			.arg1IsSS:
				mov	ax, opcode_PUSH_GenInstr_267
				jmp	short .verifyNextTokenEndOfLine

			.generateOpcode:
				call	OpcodeGenerator.execute
				mov	ah, 1
				ret

			.error.expectedEndOfLine:
				mov	si, .msg.error.expectedEndOfLine
				call	os_print_string
				call	os_print_newline
				mov	ah, 2
				ret

			.notIsCS_DS_ES_SS:
				; so must try DEFAULT HANDLER
				call	parser.backToPreviousToken
				mov	ah, 0
				ret

			.verifyNextTokenEndOfLine:
				push	ax
				call	parser.getNextToken
				pop	ax
				mov	bl, [token.type]
				cmp	bl, Token.TYPE_END_OF_LINE ; 2nd token must be end of line
				jne	.error.expectedEndOfLine
				jmp	short .generateOpcode

			.msg.error.expectedEndOfLine	db " Error: expected end of line !", 0
			PUSH_handler_msg		db "PUSH_handler invoked !", 0

		; PUSH CS                       ; 0E                   [8086]
		; PUSH DS                       ; 1E                   [8086]
		; PUSH ES                       ; 06                   [8086]
		; PUSH SS                       ; 16                   [8086]				

		; opcode_PUSH_GenInstr_264 db 2, OG.WB, 00Eh ; w0E amb1CS
		; opcode_PUSH_GenInstr_265 db 2, OG.WB, 01Eh ; w1E amb1DS
		; opcode_PUSH_GenInstr_266 db 2, OG.WB, 006h ; w06 amb1ES
		; opcode_PUSH_GenInstr_267 db 2, OG.WB, 016h ; w16 amb1SS


;===============================================================================

	InstructionHandler.possibleInstructions:

		db   1, 0, 0, 0, 0, 0 ; AAA 
		dw opcode_AAA_GenInstr_1 ; TESTED OK 08/02/2012 14:06

		db   2, 0, 0, 0, 0, 0 ; AAS 
		dw opcode_AAS_GenInstr_2 ; TESTED OK 08/02/2012 14:24

		db   3, 0, 0, 0, 0, 0 ; AAD 
		dw opcode_AAD_GenInstr_3 ; TESTED OK 08/02/2012 15:16

		db   3, Token.TYPE_IMM8, 0, 0, 0, 0 ; AAD imm8 
		dw opcode_AAD_GenInstr_4 ; ---> NOT TESTED YET

		db   4, 0, 0, 0, 0, 0 ; AAM 
		dw opcode_AAM_GenInstr_5 ; TESTED OK 08/02/2012 15:47

		db   4, Token.TYPE_IMM8, 0, 0, 0, 0 ; AAM imm8 
		dw opcode_AAM_GenInstr_6 ; ---> NOT TESTED YET

		db   5, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; ADC reg8 , reg8 
		dw opcode_ADC_GenInstr_7 ; TESTED OK 08/02/2012 16:34

		db   5, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; ADC mem , reg8 
		dw opcode_ADC_GenInstr_8 ; TESTED OK 08/02/2012 17:18

		db   5, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; ADC reg16 , reg16 
		dw opcode_ADC_GenInstr_9
		db   5, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; ADC mem , reg16 
		dw opcode_ADC_GenInstr_10
		db   5, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; ADC reg8 , reg8 
		dw opcode_ADC_GenInstr_11
		db   5, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; ADC reg8 , mem 
		dw opcode_ADC_GenInstr_12
		db   5, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; ADC reg16 , reg16 
		dw opcode_ADC_GenInstr_13
		db   5, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; ADC reg16 , mem 
		dw opcode_ADC_GenInstr_14
		db   5, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADC reg8 , imm8 
		dw opcode_ADC_GenInstr_15
		db   5, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADC mem , imm8 
		dw opcode_ADC_GenInstr_16
		db   5, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; ADC reg16 , imm16 
		dw opcode_ADC_GenInstr_17
		db   5, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADC reg16 , imm8 
		dw opcode_ADC_GenInstr_18
		db   5, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; ADC mem , imm16 
		dw opcode_ADC_GenInstr_19
		db   5, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADC mem , imm8 
		dw opcode_ADC_GenInstr_20
		db   5, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADC reg16 , imm8 
		dw opcode_ADC_GenInstr_21
		db   5, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADC mem , imm8 
		dw opcode_ADC_GenInstr_22
		db   5, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADC reg8 , imm8 
		dw opcode_ADC_GenInstr_23
		db   5, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; ADC reg16 , imm16 
		dw opcode_ADC_GenInstr_24
		db   5, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADC reg16 , imm8 
		dw opcode_ADC_GenInstr_25




		db   6, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; ADD reg8 , reg8 
		dw opcode_ADD_GenInstr_26
		db   6, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; ADD mem , reg8 
		dw opcode_ADD_GenInstr_27
		db   6, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; ADD reg16 , reg16 
		dw opcode_ADD_GenInstr_28
		db   6, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; ADD mem , reg16 
		dw opcode_ADD_GenInstr_29
		db   6, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; ADD reg8 , reg8 
		dw opcode_ADD_GenInstr_30
		db   6, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; ADD reg8 , mem 
		dw opcode_ADD_GenInstr_31
		db   6, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; ADD reg16 , reg16 
		dw opcode_ADD_GenInstr_32
		db   6, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; ADD reg16 , mem 
		dw opcode_ADD_GenInstr_33
		db   6, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADD reg8 , imm8 
		dw opcode_ADD_GenInstr_34
		db   6, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADD mem , imm8 
		dw opcode_ADD_GenInstr_35
		db   6, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; ADD reg16 , imm16 
		dw opcode_ADD_GenInstr_36
		db   6, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADD reg16 , imm8 
		dw opcode_ADD_GenInstr_37
		db   6, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; ADD mem , imm16 
		dw opcode_ADD_GenInstr_38
		db   6, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADD mem , imm8 
		dw opcode_ADD_GenInstr_39
		db   6, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADD reg16 , imm8 
		dw opcode_ADD_GenInstr_40
		db   6, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADD mem , imm8 
		dw opcode_ADD_GenInstr_41
		db   6, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADD reg8 , imm8 
		dw opcode_ADD_GenInstr_42
		db   6, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; ADD reg16 , imm16 
		dw opcode_ADD_GenInstr_43
		db   6, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ADD reg16 , imm8 
		dw opcode_ADD_GenInstr_44




		db   7, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; AND reg8 , reg8 
		dw opcode_AND_GenInstr_45
		db   7, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; AND mem , reg8 
		dw opcode_AND_GenInstr_46
		db   7, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; AND reg16 , reg16 
		dw opcode_AND_GenInstr_47
		db   7, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; AND mem , reg16 
		dw opcode_AND_GenInstr_48
		db   7, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; AND reg8 , reg8 
		dw opcode_AND_GenInstr_49
		db   7, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; AND reg8 , mem 
		dw opcode_AND_GenInstr_50
		db   7, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; AND reg16 , reg16 
		dw opcode_AND_GenInstr_51
		db   7, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; AND reg16 , mem 
		dw opcode_AND_GenInstr_52
		db   7, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; AND reg8 , imm8 
		dw opcode_AND_GenInstr_53
		db   7, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; AND mem , imm8 
		dw opcode_AND_GenInstr_54
		db   7, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; AND reg16 , imm16 
		dw opcode_AND_GenInstr_55
		db   7, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; AND reg16 , imm8 
		dw opcode_AND_GenInstr_56
		db   7, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; AND mem , imm16 
		dw opcode_AND_GenInstr_57
		db   7, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; AND mem , imm8 
		dw opcode_AND_GenInstr_58
		db   7, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; AND reg16 , imm8 
		dw opcode_AND_GenInstr_59
		db   7, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; AND mem , imm8 
		dw opcode_AND_GenInstr_60
		db   7, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; AND reg8 , imm8 
		dw opcode_AND_GenInstr_61
		db   7, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; AND reg16 , imm16 
		dw opcode_AND_GenInstr_62
		db   7, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; AND reg16 , imm8 
		dw opcode_AND_GenInstr_63


		db   8, Token.TYPE_IMM16, 0, 0, 0, 0 ; CALL imm16 
		dw opcode_CALL_GenInstr_64
		db   8, Token.TYPE_IMM8, 0, 0, 0, 0 ; CALL imm8 
		dw opcode_CALL_GenInstr_65

		db   8, Token.TYPE_IMM16, Token.TYPE_DOIS_PONTOS, Token.TYPE_IMM16, 0, 0 ; CALL imm16 : imm16 
		dw opcode_CALL_GenInstr_66
		db   8, Token.TYPE_IMM8, Token.TYPE_DOIS_PONTOS, Token.TYPE_IMM8, 0, 0 ; CALL imm8 : imm8 
		dw opcode_CALL_GenInstr_67
		db   8, Token.TYPE_IMM16, Token.TYPE_DOIS_PONTOS, Token.TYPE_IMM8, 0, 0 ; CALL imm16 : imm8 
		dw opcode_CALL_GenInstr_68
		db   8, Token.TYPE_IMM8, Token.TYPE_DOIS_PONTOS, Token.TYPE_IMM16, 0, 0 ; CALL imm8 : imm16 
		dw opcode_CALL_GenInstr_69
		db   8, Token.TYPE_INSTR, Token.TYPE_MEM, 0, 0, 0 ; CALL FAR mem 
		dw opcode_CALL_GenInstr_70
		db   8, Token.TYPE_REG16, 0, 0, 0, 0 ; CALL reg16 
		dw opcode_CALL_GenInstr_71
		db   8, Token.TYPE_MEM, 0, 0, 0, 0 ; CALL mem 
		dw opcode_CALL_GenInstr_72



		db   9, 0, 0, 0, 0, 0 ; CBW 
		dw opcode_CBW_GenInstr_73
		db  10, 0, 0, 0, 0, 0 ; CWD 
		dw opcode_CWD_GenInstr_74

		db  11, 0, 0, 0, 0, 0 ; CLC 
		dw opcode_CLC_GenInstr_75 ; TESTED OK 08/02/2012 16:18

		db  12, 0, 0, 0, 0, 0 ; CLD 
		dw opcode_CLD_GenInstr_76
		db  13, 0, 0, 0, 0, 0 ; CLI 
		dw opcode_CLI_GenInstr_77
		db  14, 0, 0, 0, 0, 0 ; CMC 
		dw opcode_CMC_GenInstr_78





		db  15, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; CMP reg8 , reg8 
		dw opcode_CMP_GenInstr_79

		db  15, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; CMP mem , reg8 
		dw opcode_CMP_GenInstr_80

		db  15, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; CMP reg16 , reg16 
		dw opcode_CMP_GenInstr_81
		db  15, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; CMP mem , reg16 
		dw opcode_CMP_GenInstr_82
		db  15, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; CMP reg8 , reg8 
		dw opcode_CMP_GenInstr_83
		db  15, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; CMP reg8 , mem 
		dw opcode_CMP_GenInstr_84
		db  15, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; CMP reg16 , reg16 
		dw opcode_CMP_GenInstr_85
		db  15, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; CMP reg16 , mem 
		dw opcode_CMP_GenInstr_86
		db  15, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; CMP reg8 , imm8 
		dw opcode_CMP_GenInstr_87

		db  15, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; CMP mem , imm8 
		dw opcode_CMP_GenInstr_88

		db  15, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; CMP reg16 , imm16 
		dw opcode_CMP_GenInstr_89
		db  15, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; CMP reg16 , imm8 
		dw opcode_CMP_GenInstr_90
		db  15, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; CMP mem , imm16 
		dw opcode_CMP_GenInstr_91
		db  15, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; CMP mem , imm8 
		dw opcode_CMP_GenInstr_92
		db  15, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; CMP reg16 , imm8 
		dw opcode_CMP_GenInstr_93
		db  15, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; CMP mem , imm8 
		dw opcode_CMP_GenInstr_94
		db  15, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; CMP reg8 , imm8 
		dw opcode_CMP_GenInstr_95
		db  15, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; CMP reg16 , imm16 
		dw opcode_CMP_GenInstr_96
		db  15, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; CMP reg16 , imm8 
		dw opcode_CMP_GenInstr_97





		db  16, 0, 0, 0, 0, 0 ; CMPSB 
		dw opcode_CMPSB_GenInstr_98
		db  17, 0, 0, 0, 0, 0 ; CMPSW 
		dw opcode_CMPSW_GenInstr_99
		db  18, 0, 0, 0, 0, 0 ; DAA 
		dw opcode_DAA_GenInstr_100
		db  19, 0, 0, 0, 0, 0 ; DAS 
		dw opcode_DAS_GenInstr_101


		db  20, Token.TYPE_REG16, 0, 0, 0, 0 ; DEC reg16 
		dw opcode_DEC_GenInstr_102
		db  20, Token.TYPE_REG8, 0, 0, 0, 0 ; DEC reg8 
		dw opcode_DEC_GenInstr_103
		db  20, Token.TYPE_MEM, 0, 0, 0, 0 ; DEC mem 
		dw opcode_DEC_GenInstr_104
		db  20, Token.TYPE_REG16, 0, 0, 0, 0 ; DEC reg16 
		dw opcode_DEC_GenInstr_105
		db  20, Token.TYPE_MEM, 0, 0, 0, 0 ; DEC mem 
		dw opcode_DEC_GenInstr_106


		db  21, Token.TYPE_REG8, 0, 0, 0, 0 ; DIV reg8 
		dw opcode_DIV_GenInstr_107
		db  21, Token.TYPE_MEM, 0, 0, 0, 0 ; DIV mem 
		dw opcode_DIV_GenInstr_108
		db  21, Token.TYPE_REG16, 0, 0, 0, 0 ; DIV reg16 
		dw opcode_DIV_GenInstr_109
		db  21, Token.TYPE_MEM, 0, 0, 0, 0 ; DIV mem 
		dw opcode_DIV_GenInstr_110


		db  22, 0, 0, 0, 0, 0 ; HLT 
		dw opcode_HLT_GenInstr_111


		db  23, Token.TYPE_REG8, 0, 0, 0, 0 ; IDIV reg8 
		dw opcode_IDIV_GenInstr_112
		db  23, Token.TYPE_MEM, 0, 0, 0, 0 ; IDIV mem 
		dw opcode_IDIV_GenInstr_113
		db  23, Token.TYPE_REG16, 0, 0, 0, 0 ; IDIV reg16 
		dw opcode_IDIV_GenInstr_114
		db  23, Token.TYPE_MEM, 0, 0, 0, 0 ; IDIV mem 
		dw opcode_IDIV_GenInstr_115


		db  24, Token.TYPE_REG8, 0, 0, 0, 0 ; IMUL reg8 
		dw opcode_IMUL_GenInstr_116
		db  24, Token.TYPE_MEM, 0, 0, 0, 0 ; IMUL mem 
		dw opcode_IMUL_GenInstr_117
		db  24, Token.TYPE_REG16, 0, 0, 0, 0 ; IMUL reg16 
		dw opcode_IMUL_GenInstr_118
		db  24, Token.TYPE_MEM, 0, 0, 0, 0 ; IMUL mem 
		dw opcode_IMUL_GenInstr_119


		db  25, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; IN reg8 , imm8 
		dw opcode_IN_GenInstr_120
		db  25, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; IN reg16 , imm8 
		dw opcode_IN_GenInstr_121
		db  25, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; IN reg8 , reg16 
		dw opcode_IN_GenInstr_122
		db  25, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; IN reg16 , reg16 
		dw opcode_IN_GenInstr_123


		db  26, Token.TYPE_REG16, 0, 0, 0, 0 ; INC reg16 
		dw opcode_INC_GenInstr_124
		db  26, Token.TYPE_REG8, 0, 0, 0, 0 ; INC reg8 
		dw opcode_INC_GenInstr_125
		db  26, Token.TYPE_MEM, 0, 0, 0, 0 ; INC mem 
		dw opcode_INC_GenInstr_126
		db  26, Token.TYPE_REG16, 0, 0, 0, 0 ; INC reg16 
		dw opcode_INC_GenInstr_127
		db  26, Token.TYPE_MEM, 0, 0, 0, 0 ; INC mem 
		dw opcode_INC_GenInstr_128




		db  27, Token.TYPE_IMM8, 0, 0, 0, 0 ; INT imm8 
		dw opcode_INT_GenInstr_129
		db  28, 0, 0, 0, 0, 0 ; INT3 
		dw opcode_INT3_GenInstr_130
		db  29, 0, 0, 0, 0, 0 ; INTO 
		dw opcode_INTO_GenInstr_131
		db  30, 0, 0, 0, 0, 0 ; IRET 
		dw opcode_IRET_GenInstr_132
		db  31, 0, 0, 0, 0, 0 ; IRETW 
		dw opcode_IRETW_GenInstr_133
		db  32, Token.TYPE_IMM8, 0, 0, 0, 0 ; JCXZ imm8 
		dw opcode_JCXZ_GenInstr_134



		;db  33, Token.TYPE_IMM8, 0, 0, 0, 0 ; JMP imm8 
		;dw opcode_JMP_GenInstr_135

		db  33, Token.TYPE_IMM16, 0, 0, 0, 0 ; JMP imm16 
		dw opcode_JMP_GenInstr_136

		;db  33, Token.TYPE_IMM8, 0, 0, 0, 0 ; JMP imm8 
		;dw opcode_JMP_GenInstr_137

		db  33, Token.TYPE_RESERVED_SHORT, Token.TYPE_IMM16, 0, 0, 0 ; JMP SHORT label  
		dw opcode_JMP_GenInstr_138

		;db  33, Token.TYPE_IMM16, Token.TYPE_DOIS_PONTOS, Token.TYPE_IMM16, 0, 0 ; JMP imm16 : imm16 
		;dw opcode_JMP_GenInstr_139
		;db  33, Token.TYPE_IMM8, Token.TYPE_DOIS_PONTOS, Token.TYPE_IMM8, 0, 0 ; JMP imm8 : imm8 
		;dw opcode_JMP_GenInstr_140
		;db  33, Token.TYPE_IMM8, Token.TYPE_DOIS_PONTOS, Token.TYPE_IMM16, 0, 0 ; JMP imm8 : imm16 
		;dw opcode_JMP_GenInstr_141
		;db  33, Token.TYPE_IMM16, Token.TYPE_DOIS_PONTOS, Token.TYPE_IMM8, 0, 0 ; JMP imm16 : imm8 
		;dw opcode_JMP_GenInstr_142
		;db  33, Token.TYPE_INSTR, Token.TYPE_MEM, 0, 0, 0 ; JMP FAR mem 
		;dw opcode_JMP_GenInstr_143
		;db  33, Token.TYPE_REG16, 0, 0, 0, 0 ; JMP reg16 
		;dw opcode_JMP_GenInstr_144
		;db  33, Token.TYPE_MEM, 0, 0, 0, 0 ; JMP mem 
		;dw opcode_JMP_GenInstr_145





		db  34, Token.TYPE_IMM16, 0, 0, 0, 0 ; JO label
		dw opcode_JO_GenInstr_146
		db  35, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNO label
		dw opcode_JNO_GenInstr_147
		db  36, Token.TYPE_IMM16, 0, 0, 0, 0 ; JB label
		dw opcode_JB_GenInstr_148
		db  37, Token.TYPE_IMM16, 0, 0, 0, 0 ; JC label 
		dw opcode_JC_GenInstr_149
		db  38, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNAE label 
		dw opcode_JNAE_GenInstr_150
		db  39, Token.TYPE_IMM16, 0, 0, 0, 0 ; JAE label
		dw opcode_JAE_GenInstr_151
		db  40, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNB label
		dw opcode_JNB_GenInstr_152
		db  41, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNC label
		dw opcode_JNC_GenInstr_153
		db  42, Token.TYPE_IMM16, 0, 0, 0, 0 ; JE label
		dw opcode_JE_GenInstr_154
		db  43, Token.TYPE_IMM16, 0, 0, 0, 0 ; JZ label
		dw opcode_JZ_GenInstr_155
		db  44, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNE label
		dw opcode_JNE_GenInstr_156
		db  45, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNZ label
		dw opcode_JNZ_GenInstr_157
		db  46, Token.TYPE_IMM16, 0, 0, 0, 0 ; JBE label
		dw opcode_JBE_GenInstr_158
		db  47, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNA label
		dw opcode_JNA_GenInstr_159
		db  48, Token.TYPE_IMM16, 0, 0, 0, 0 ; JA label 
		dw opcode_JA_GenInstr_160
		db  49, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNBE label 
		dw opcode_JNBE_GenInstr_161
		db  50, Token.TYPE_IMM16, 0, 0, 0, 0 ; JS label 
		dw opcode_JS_GenInstr_162
		db  51, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNS label 
		dw opcode_JNS_GenInstr_163
		db  52, Token.TYPE_IMM16, 0, 0, 0, 0 ; JP label 
		dw opcode_JP_GenInstr_164
		db  53, Token.TYPE_IMM16, 0, 0, 0, 0 ; JPE label 
		dw opcode_JPE_GenInstr_165
		db  54, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNP label 
		dw opcode_JNP_GenInstr_166
		db  55, Token.TYPE_IMM16, 0, 0, 0, 0 ; JPO label 
		dw opcode_JPO_GenInstr_167
		db  56, Token.TYPE_IMM16, 0, 0, 0, 0 ; JL label 
		dw opcode_JL_GenInstr_168
		db  57, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNGE label 
		dw opcode_JNGE_GenInstr_169
		db  58, Token.TYPE_IMM16, 0, 0, 0, 0 ; JGE label 
		dw opcode_JGE_GenInstr_170
		db  59, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNL label 
		dw opcode_JNL_GenInstr_171
		db  60, Token.TYPE_IMM16, 0, 0, 0, 0 ; JLE label 
		dw opcode_JLE_GenInstr_172
		db  61, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNG label 
		dw opcode_JNG_GenInstr_173
		db  62, Token.TYPE_IMM16, 0, 0, 0, 0 ; JG label 
		dw opcode_JG_GenInstr_174
		db  63, Token.TYPE_IMM16, 0, 0, 0, 0 ; JNLE label 
		dw opcode_JNLE_GenInstr_175





		db  64, 0, 0, 0, 0, 0 ; LAHF 
		dw opcode_LAHF_GenInstr_176


		db  65, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; LDS reg16 , mem 
		dw opcode_LDS_GenInstr_177


		db  66, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; LES reg16 , mem 
		dw opcode_LES_GenInstr_178


		db  67, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; LEA reg16 , mem 
		dw opcode_LEA_GenInstr_179


		db  68, 0, 0, 0, 0, 0 ; LODSB 
		dw opcode_LODSB_GenInstr_180
		db  69, 0, 0, 0, 0, 0 ; LODSW 
		dw opcode_LODSW_GenInstr_181


		db  70, Token.TYPE_IMM16, 0, 0, 0, 0 ; LOOP imm8 // changed to LOOP imm16
		dw opcode_LOOP_GenInstr_182
		db  70, Token.TYPE_IMM8, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; LOOP imm8 , reg16 
		dw opcode_LOOP_GenInstr_183
		db  71, Token.TYPE_IMM8, 0, 0, 0, 0 ; LOOPE imm8 
		dw opcode_LOOPE_GenInstr_184
		db  71, Token.TYPE_IMM8, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; LOOPE imm8 , reg16 
		dw opcode_LOOPE_GenInstr_185
		db  72, Token.TYPE_IMM8, 0, 0, 0, 0 ; LOOPZ imm8 
		dw opcode_LOOPZ_GenInstr_186
		db  72, Token.TYPE_IMM8, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; LOOPZ imm8 , reg16 
		dw opcode_LOOPZ_GenInstr_187
		db  73, Token.TYPE_IMM8, 0, 0, 0, 0 ; LOOPNE imm8 
		dw opcode_LOOPNE_GenInstr_188
		db  73, Token.TYPE_IMM8, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; LOOPNE imm8 , reg16 
		dw opcode_LOOPNE_GenInstr_189
		db  74, Token.TYPE_IMM8, 0, 0, 0, 0 ; LOOPNZ imm8 
		dw opcode_LOOPNZ_GenInstr_190
		db  74, Token.TYPE_IMM8, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; LOOPNZ imm8 , reg16 
		dw opcode_LOOPNZ_GenInstr_191





		db  75, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; MOV reg8 , reg8 
		dw opcode_MOV_GenInstr_192
		db  75, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; MOV mem , reg8 
		dw opcode_MOV_GenInstr_193

		db  75, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; MOV reg16 , reg16 
		dw opcode_MOV_GenInstr_194
		db  75, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; MOV mem , reg16 
		dw opcode_MOV_GenInstr_195
		db  75, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; MOV reg8 , reg8 
		dw opcode_MOV_GenInstr_196
		db  75, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; MOV reg8 , mem 
		dw opcode_MOV_GenInstr_197
		db  75, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; MOV reg16 , reg16 
		dw opcode_MOV_GenInstr_198
		db  75, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; MOV reg16 , mem 
		dw opcode_MOV_GenInstr_199
		db  75, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; MOV reg8 , imm8 
		dw opcode_MOV_GenInstr_200
		db  75, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; MOV reg16 , imm16 
		dw opcode_MOV_GenInstr_201

		db  75, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; MOV reg16 , imm8 
		dw opcode_MOV_GenInstr_202

		db  75, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; MOV reg8 , imm8 
		dw opcode_MOV_GenInstr_203

		db  75, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; MOV BYTE mem , imm8 
		dw opcode_MOV_GenInstr_204

		db  75, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; MOV reg16 , imm16 
		dw opcode_MOV_GenInstr_205
		db  75, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; MOV reg16 , imm8 
		dw opcode_MOV_GenInstr_206

		db  75, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0 ; MOV WORD mem , imm16 
		dw opcode_MOV_GenInstr_207
		db  75, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; MOV WORD mem , imm8 
		dw opcode_MOV_GenInstr_208

		db  75, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_MEMOFFS8, 0, 0 ; MOV reg8 , memoffs8 
		dw opcode_MOV_GenInstr_209
		db  75, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEMOFFS16, 0, 0 ; MOV reg16 , memoffs16 
		dw opcode_MOV_GenInstr_210
		db  75, Token.TYPE_MEMOFFS8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; MOV memoffs8 , reg8 
		dw opcode_MOV_GenInstr_211
		db  75, Token.TYPE_MEMOFFS16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; MOV memoffs16 , reg16 
		dw opcode_MOV_GenInstr_212
		db  75, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REGSEG, 0, 0 ; MOV reg16 , segreg 
		dw opcode_MOV_GenInstr_213
		db  75, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REGSEG, 0, 0 ; MOV mem , segreg 
		dw opcode_MOV_GenInstr_214
		db  75, Token.TYPE_REGSEG, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; MOV segreg , reg16 
		dw opcode_MOV_GenInstr_215
		db  75, Token.TYPE_REGSEG, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; MOV segreg , mem 
		dw opcode_MOV_GenInstr_216


		db  76, 0, 0, 0, 0, 0 ; MOVSB 
		dw opcode_MOVSB_GenInstr_217
		db  77, 0, 0, 0, 0, 0 ; MOVSW 
		dw opcode_MOVSW_GenInstr_218


		db  78, Token.TYPE_REG8, 0, 0, 0, 0 ; MUL reg8 
		dw opcode_MUL_GenInstr_219
		db  78, Token.TYPE_MEM, 0, 0, 0, 0 ; MUL mem 
		dw opcode_MUL_GenInstr_220
		db  78, Token.TYPE_REG16, 0, 0, 0, 0 ; MUL reg16 
		dw opcode_MUL_GenInstr_221
		db  78, Token.TYPE_MEM, 0, 0, 0, 0 ; MUL mem 
		dw opcode_MUL_GenInstr_222


		db  79, Token.TYPE_REG8, 0, 0, 0, 0 ; NEG reg8 
		dw opcode_NEG_GenInstr_223
		db  79, Token.TYPE_MEM, 0, 0, 0, 0 ; NEG mem 
		dw opcode_NEG_GenInstr_224
		db  79, Token.TYPE_REG16, 0, 0, 0, 0 ; NEG reg16 
		dw opcode_NEG_GenInstr_225
		db  79, Token.TYPE_MEM, 0, 0, 0, 0 ; NEG mem 
		dw opcode_NEG_GenInstr_226


		db  80, Token.TYPE_REG8, 0, 0, 0, 0 ; NOT reg8 
		dw opcode_NOT_GenInstr_227
		db  80, Token.TYPE_MEM, 0, 0, 0, 0 ; NOT mem 
		dw opcode_NOT_GenInstr_228
		db  80, Token.TYPE_REG16, 0, 0, 0, 0 ; NOT reg16 
		dw opcode_NOT_GenInstr_229
		db  80, Token.TYPE_MEM, 0, 0, 0, 0 ; NOT mem 
		dw opcode_NOT_GenInstr_230

		db  81, 0, 0, 0, 0, 0 ; NOP 
		dw opcode_NOP_GenInstr_231

		db  82, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; OR reg8 , reg8 
		dw opcode_OR_GenInstr_232
		db  82, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; OR mem , reg8 
		dw opcode_OR_GenInstr_233
		db  82, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; OR reg16 , reg16 
		dw opcode_OR_GenInstr_234
		db  82, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; OR mem , reg16 
		dw opcode_OR_GenInstr_235
		db  82, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; OR reg8 , reg8 
		dw opcode_OR_GenInstr_236
		db  82, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; OR reg8 , mem 
		dw opcode_OR_GenInstr_237
		db  82, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; OR reg16 , reg16 
		dw opcode_OR_GenInstr_238
		db  82, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; OR reg16 , mem 
		dw opcode_OR_GenInstr_239
		db  82, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; OR reg8 , imm8 
		dw opcode_OR_GenInstr_240
		db  82, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; OR mem , imm8 
		dw opcode_OR_GenInstr_241
		db  82, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; OR reg16 , imm16 
		dw opcode_OR_GenInstr_242
		db  82, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; OR reg16 , imm8 
		dw opcode_OR_GenInstr_243
		db  82, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; OR mem , imm16 
		dw opcode_OR_GenInstr_244
		db  82, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; OR mem , imm8 
		dw opcode_OR_GenInstr_245
		db  82, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; OR reg16 , imm8 
		dw opcode_OR_GenInstr_246
		db  82, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; OR mem , imm8 
		dw opcode_OR_GenInstr_247
		db  82, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; OR reg8 , imm8 
		dw opcode_OR_GenInstr_248
		db  82, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; OR reg16 , imm16 
		dw opcode_OR_GenInstr_249
		db  82, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; OR reg16 , imm8 
		dw opcode_OR_GenInstr_250


		db  83, Token.TYPE_IMM8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; OUT imm8 , reg8 
		dw opcode_OUT_GenInstr_251
		db  83, Token.TYPE_IMM8, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; OUT imm8 , reg16 
		dw opcode_OUT_GenInstr_252
		db  83, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; OUT reg16 , reg8 
		dw opcode_OUT_GenInstr_253
		db  83, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; OUT reg16 , reg16 
		dw opcode_OUT_GenInstr_254


		db  84, Token.TYPE_REG16, 0, 0, 0, 0 ; POP reg16 
		dw opcode_POP_GenInstr_255
		db  84, Token.TYPE_REG16, 0, 0, 0, 0 ; POP reg16 
		dw opcode_POP_GenInstr_256
		db  84, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, 0, 0, 0 ; POP WORD [mem] 
		dw opcode_POP_GenInstr_257

		; handled by POP_HANDLER
		; db  84, Token.TYPE_REGSEG, 0, 0, 0, 0 ; POP DS
		; dw opcode_POP_GenInstr_258
		; db  84, Token.TYPE_REGSEG, 0, 0, 0, 0 ; POP ES
		; dw opcode_POP_GenInstr_259
		; db  84, Token.TYPE_REGSEG, 0, 0, 0, 0 ; POP SS
		; dw opcode_POP_GenInstr_260


		db  85, Token.TYPE_REG16, 0, 0, 0, 0 ; PUSH reg16 
		dw opcode_PUSH_GenInstr_261
		db  85, Token.TYPE_REG16, 0, 0, 0, 0 ; PUSH reg16 
		dw opcode_PUSH_GenInstr_262
		db  85, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, 0, 0, 0 ; PUSH WORD [mem] 
		dw opcode_PUSH_GenInstr_263

		; handled by POP_HANDLER
		; db  85, Token.TYPE_REGSEG, 0, 0, 0, 0 ; PUSH segreg 
		; dw opcode_PUSH_GenInstr_264
		; db  85, Token.TYPE_REGSEG, 0, 0, 0, 0 ; PUSH segreg 
		; dw opcode_PUSH_GenInstr_265
		; db  85, Token.TYPE_REGSEG, 0, 0, 0, 0 ; PUSH segreg 
		; dw opcode_PUSH_GenInstr_266
		; db  85, Token.TYPE_REGSEG, 0, 0, 0, 0 ; PUSH segreg 
		; dw opcode_PUSH_GenInstr_267



		db  86, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; RCL reg8 , 1 
		dw opcode_RCL_GenInstr_268
		db  86, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; RCL BYTE [mem] , 1 
		dw opcode_RCL_GenInstr_269
		db  86, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; RCL reg8 , CL 
		dw opcode_RCL_GenInstr_270
		db  86, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; RCL BYTE [mem] , CL
		dw opcode_RCL_GenInstr_271
		db  86, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; RCL reg16 , 1 
		dw opcode_RCL_GenInstr_272
		db  86, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; RCL WORD [mem] , 1 
		dw opcode_RCL_GenInstr_273
		db  86, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; RCL reg16 , CL
		dw opcode_RCL_GenInstr_274
		db  86, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; RCL WORD [mem] , CL
		dw opcode_RCL_GenInstr_275




		db  88, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; RCR reg8 , 1 
		dw opcode_RCR_GenInstr_276
		db  88, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; RCR BYTE [mem] , 1 
		dw opcode_RCR_GenInstr_277
		db  88, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; RCR reg8 , CL 
		dw opcode_RCR_GenInstr_278
		db  88, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; RCR BYTE [mem] , CL
		dw opcode_RCR_GenInstr_279
		db  88, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; RCR reg16 , 1 
		dw opcode_RCR_GenInstr_280
		db  88, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; RCR WORD [mem] , 1 
		dw opcode_RCR_GenInstr_281
		db  88, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; RCR reg16 , CL
		dw opcode_RCR_GenInstr_282
		db  88, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; RCR WORD [mem] , CL
		dw opcode_RCR_GenInstr_283



		db  89, 0, 0, 0, 0, 0 ; RET 
		dw opcode_RET_GenInstr_284
		db  89, Token.TYPE_IMM16, 0, 0, 0, 0 ; RET imm16 
		dw opcode_RET_GenInstr_285
		db  89, Token.TYPE_IMM8, 0, 0, 0, 0 ; RET imm8 
		dw opcode_RET_GenInstr_286


		db  90, 0, 0, 0, 0, 0 ; RETF 
		dw opcode_RETF_GenInstr_287
		db  90, Token.TYPE_IMM16, 0, 0, 0, 0 ; RETF imm16 
		dw opcode_RETF_GenInstr_288
		db  90, Token.TYPE_IMM8, 0, 0, 0, 0 ; RETF imm8 
		dw opcode_RETF_GenInstr_289


		db  91, 0, 0, 0, 0, 0 ; RETN 
		dw opcode_RETN_GenInstr_290
		db  91, Token.TYPE_IMM16, 0, 0, 0, 0 ; RETN imm16 
		dw opcode_RETN_GenInstr_291
		db  91, Token.TYPE_IMM8, 0, 0, 0, 0 ; RETN imm8 
		dw opcode_RETN_GenInstr_292



		db  92, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ROL reg8 , 1 
		dw opcode_ROL_GenInstr_293
		db  92, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; ROL BYTE [mem] , 1 
		dw opcode_ROL_GenInstr_294
		db  92, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; ROL reg8 , CL 
		dw opcode_ROL_GenInstr_295
		db  92, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; ROL BYTE [mem] , CL
		dw opcode_ROL_GenInstr_296
		db  92, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ROL reg16 , 1 
		dw opcode_ROL_GenInstr_297
		db  92, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; ROL WORD [mem] , 1 
		dw opcode_ROL_GenInstr_298
		db  92, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; ROL reg16 , CL
		dw opcode_ROL_GenInstr_299
		db  92, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; ROL WORD [mem] , CL
		dw opcode_ROL_GenInstr_300






		db  93, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ROR reg8 , 1 
		dw opcode_ROR_GenInstr_301
		db  93, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; ROR BYTE [mem] , 1 
		dw opcode_ROR_GenInstr_302
		db  93, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; ROR reg8 , CL 
		dw opcode_ROR_GenInstr_303
		db  93, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; ROR BYTE [mem] , CL
		dw opcode_ROR_GenInstr_304
		db  93, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; ROR reg16 , 1 
		dw opcode_ROR_GenInstr_305
		db  93, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; ROR WORD [mem] , 1 
		dw opcode_ROR_GenInstr_306
		db  93, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; ROR reg16 , CL
		dw opcode_ROR_GenInstr_307
		db  93, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; ROR WORD [mem] , CL
		dw opcode_ROR_GenInstr_308



		db  94, 0, 0, 0, 0, 0 ; SAHF 
		dw opcode_SAHF_GenInstr_309




		db  95, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SAL reg8 , 1 
		dw opcode_SAL_GenInstr_310
		db  95, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; SAL BYTE [mem] , 1 
		dw opcode_SAL_GenInstr_311
		db  95, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SAL reg8 , CL 
		dw opcode_SAL_GenInstr_312
		db  95, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; SAL BYTE [mem] , CL
		dw opcode_SAL_GenInstr_313
		db  95, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SAL reg16 , 1 
		dw opcode_SAL_GenInstr_314
		db  95, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; SAL WORD [mem] , 1 
		dw opcode_SAL_GenInstr_315
		db  95, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SAL reg16 , CL
		dw opcode_SAL_GenInstr_316
		db  95, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; SAL WORD [mem] , CL
		dw opcode_SAL_GenInstr_317





		db  96, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SAR reg8 , 1 
		dw opcode_SAR_GenInstr_318
		db  96, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; SAR BYTE [mem] , 1 
		dw opcode_SAR_GenInstr_319
		db  96, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SAR reg8 , CL 
		dw opcode_SAR_GenInstr_320
		db  96, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; SAR BYTE [mem] , CL
		dw opcode_SAR_GenInstr_321
		db  96, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SAR reg16 , 1 
		dw opcode_SAR_GenInstr_322
		db  96, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; SAR WORD [mem] , 1 
		dw opcode_SAR_GenInstr_323
		db  96, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SAR reg16 , CL
		dw opcode_SAR_GenInstr_324
		db  96, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; SAR WORD [mem] , CL
		dw opcode_SAR_GenInstr_325



		db  97, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SBB reg8 , reg8 
		dw opcode_SBB_GenInstr_326
		db  97, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SBB mem , reg8 
		dw opcode_SBB_GenInstr_327
		db  97, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; SBB reg16 , reg16 
		dw opcode_SBB_GenInstr_328
		db  97, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; SBB mem , reg16 
		dw opcode_SBB_GenInstr_329
		db  97, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SBB reg8 , reg8 
		dw opcode_SBB_GenInstr_330
		db  97, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; SBB reg8 , mem 
		dw opcode_SBB_GenInstr_331
		db  97, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; SBB reg16 , reg16 
		dw opcode_SBB_GenInstr_332
		db  97, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; SBB reg16 , mem 
		dw opcode_SBB_GenInstr_333
		db  97, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SBB reg8 , imm8 
		dw opcode_SBB_GenInstr_334
		db  97, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SBB mem , imm8 
		dw opcode_SBB_GenInstr_335
		db  97, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; SBB reg16 , imm16 
		dw opcode_SBB_GenInstr_336
		db  97, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SBB reg16 , imm8 
		dw opcode_SBB_GenInstr_337
		db  97, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; SBB mem , imm16 
		dw opcode_SBB_GenInstr_338
		db  97, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SBB mem , imm8 
		dw opcode_SBB_GenInstr_339
		db  97, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SBB reg16 , imm8 
		dw opcode_SBB_GenInstr_340
		db  97, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SBB mem , imm8 
		dw opcode_SBB_GenInstr_341
		db  97, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SBB reg8 , imm8 
		dw opcode_SBB_GenInstr_342
		db  97, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; SBB reg16 , imm16 
		dw opcode_SBB_GenInstr_343
		db  97, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SBB reg16 , imm8 
		dw opcode_SBB_GenInstr_344


		db  98, 0, 0, 0, 0, 0 ; SCASB 
		dw opcode_SCASB_GenInstr_345
		db  99, 0, 0, 0, 0, 0 ; SCASW 
		dw opcode_SCASW_GenInstr_346





		db 100, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SHL reg8 , 1 
		dw opcode_SHL_GenInstr_347
		db 100, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; SHL BYTE [mem] , 1 
		dw opcode_SHL_GenInstr_348
		db 100, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SHL reg8 , CL 
		dw opcode_SHL_GenInstr_349
		db 100, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; SHL BYTE [mem] , CL
		dw opcode_SHL_GenInstr_350
		db 100, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SHL reg16 , 1 
		dw opcode_SHL_GenInstr_351
		db 100, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; SHL WORD [mem] , 1 
		dw opcode_SHL_GenInstr_352
		db 100, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SHL reg16 , CL
		dw opcode_SHL_GenInstr_353
		db 100, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; SHL WORD [mem] , CL
		dw opcode_SHL_GenInstr_354



		db 101, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SHR reg8 , 1 
		dw opcode_SHR_GenInstr_355
		db 101, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; SHR BYTE [mem] , 1 
		dw opcode_SHR_GenInstr_356
		db 101, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SHR reg8 , CL 
		dw opcode_SHR_GenInstr_357
		db 101, Token.TYPE_RESERVED_BYTE, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; SHR BYTE [mem] , CL
		dw opcode_SHR_GenInstr_358
		db 101, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SHR reg16 , 1 
		dw opcode_SHR_GenInstr_359
		db 101, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0 ; SHR WORD [mem] , 1 
		dw opcode_SHR_GenInstr_360
		db 101, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SHR reg16 , CL
		dw opcode_SHR_GenInstr_361
		db 101, Token.TYPE_RESERVED_WORD, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0 ; SHR WORD [mem] , CL
		dw opcode_SHR_GenInstr_362




		db 102, 0, 0, 0, 0, 0 ; STC 
		dw opcode_STC_GenInstr_363 ; TESTED OK 08/02/2012 16:10

		db 103, 0, 0, 0, 0, 0 ; STD 
		dw opcode_STD_GenInstr_364
		db 104, 0, 0, 0, 0, 0 ; STI 
		dw opcode_STI_GenInstr_365
		db 105, 0, 0, 0, 0, 0 ; STOSB 
		dw opcode_STOSB_GenInstr_366
		db 106, 0, 0, 0, 0, 0 ; STOSW 
		dw opcode_STOSW_GenInstr_367
		db 107, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SUB reg8 , reg8 
		dw opcode_SUB_GenInstr_368
		db 107, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SUB mem , reg8 
		dw opcode_SUB_GenInstr_369
		db 107, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; SUB reg16 , reg16 
		dw opcode_SUB_GenInstr_370
		db 107, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; SUB mem , reg16 
		dw opcode_SUB_GenInstr_371
		db 107, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; SUB reg8 , reg8 
		dw opcode_SUB_GenInstr_372
		db 107, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; SUB reg8 , mem 
		dw opcode_SUB_GenInstr_373
		db 107, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; SUB reg16 , reg16 
		dw opcode_SUB_GenInstr_374
		db 107, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; SUB reg16 , mem 
		dw opcode_SUB_GenInstr_375
		db 107, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SUB reg8 , imm8 
		dw opcode_SUB_GenInstr_376
		db 107, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SUB mem , imm8 
		dw opcode_SUB_GenInstr_377
		db 107, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; SUB reg16 , imm16 
		dw opcode_SUB_GenInstr_378
		db 107, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SUB reg16 , imm8 
		dw opcode_SUB_GenInstr_379
		db 107, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; SUB mem , imm16 
		dw opcode_SUB_GenInstr_380
		db 107, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SUB mem , imm8 
		dw opcode_SUB_GenInstr_381
		db 107, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SUB reg16 , imm8 
		dw opcode_SUB_GenInstr_382
		db 107, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SUB mem , imm8 
		dw opcode_SUB_GenInstr_383
		db 107, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SUB reg8 , imm8 
		dw opcode_SUB_GenInstr_384
		db 107, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; SUB reg16 , imm16 
		dw opcode_SUB_GenInstr_385
		db 107, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; SUB reg16 , imm8 
		dw opcode_SUB_GenInstr_386
		db 108, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; TEST reg8 , reg8 
		dw opcode_TEST_GenInstr_387
		db 108, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; TEST mem , reg8 
		dw opcode_TEST_GenInstr_388
		db 108, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; TEST reg16 , reg16 
		dw opcode_TEST_GenInstr_389
		db 108, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; TEST mem , reg16 
		dw opcode_TEST_GenInstr_390
		db 108, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; TEST reg8 , imm8 
		dw opcode_TEST_GenInstr_391
		db 108, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; TEST mem , imm8 
		dw opcode_TEST_GenInstr_392
		db 108, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; TEST reg16 , imm16 
		dw opcode_TEST_GenInstr_393
		db 108, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; TEST reg16 , imm8 
		dw opcode_TEST_GenInstr_394
		db 108, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; TEST mem , imm16 
		dw opcode_TEST_GenInstr_395
		db 108, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; TEST mem , imm8 
		dw opcode_TEST_GenInstr_396
		db 108, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; TEST reg8 , imm8 
		dw opcode_TEST_GenInstr_397
		db 108, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; TEST reg16 , imm16 
		dw opcode_TEST_GenInstr_398
		db 108, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; TEST reg16 , imm8 
		dw opcode_TEST_GenInstr_399
		db 109, 0, 0, 0, 0, 0 ; WAIT 
		dw opcode_WAIT_GenInstr_400
		db 110, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; XCHG reg8 , reg8 
		dw opcode_XCHG_GenInstr_401
		db 110, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; XCHG reg8 , mem 
		dw opcode_XCHG_GenInstr_402
		db 110, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; XCHG reg16 , reg8 
		dw opcode_XCHG_GenInstr_403
		db 110, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; XCHG reg16 , mem 
		dw opcode_XCHG_GenInstr_404
		db 110, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; XCHG reg8 , reg8 
		dw opcode_XCHG_GenInstr_405
		db 110, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; XCHG mem , reg8 
		dw opcode_XCHG_GenInstr_406
		db 110, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; XCHG reg16 , reg16 
		dw opcode_XCHG_GenInstr_407
		db 110, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; XCHG mem , reg16 
		dw opcode_XCHG_GenInstr_408
		db 110, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; XCHG reg16 , reg16 
		dw opcode_XCHG_GenInstr_409
		db 110, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; XCHG reg16 , reg16 
		dw opcode_XCHG_GenInstr_410
		db 111, 0, 0, 0, 0, 0 ; XLATB 
		dw opcode_XLATB_GenInstr_411
		db 112, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; XOR reg8 , reg8 
		dw opcode_XOR_GenInstr_412
		db 112, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; XOR mem , reg8 
		dw opcode_XOR_GenInstr_413
		db 112, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; XOR reg16 , reg16 
		dw opcode_XOR_GenInstr_414
		db 112, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; XOR mem , reg16 
		dw opcode_XOR_GenInstr_415
		db 112, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_REG8, 0, 0 ; XOR reg8 , reg8 
		dw opcode_XOR_GenInstr_416
		db 112, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; XOR reg8 , mem 
		dw opcode_XOR_GenInstr_417
		db 112, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_REG16, 0, 0 ; XOR reg16 , reg16 
		dw opcode_XOR_GenInstr_418
		db 112, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_MEM, 0, 0 ; XOR reg16 , mem 
		dw opcode_XOR_GenInstr_419
		db 112, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; XOR reg8 , imm8 
		dw opcode_XOR_GenInstr_420
		db 112, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; XOR mem , imm8 
		dw opcode_XOR_GenInstr_421
		db 112, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; XOR reg16 , imm16 
		dw opcode_XOR_GenInstr_422
		db 112, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; XOR reg16 , imm8 
		dw opcode_XOR_GenInstr_423
		db 112, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; XOR mem , imm16 
		dw opcode_XOR_GenInstr_424
		db 112, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; XOR mem , imm8 
		dw opcode_XOR_GenInstr_425
		db 112, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; XOR reg16 , imm8 
		dw opcode_XOR_GenInstr_426
		db 112, Token.TYPE_MEM, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; XOR mem , imm8 
		dw opcode_XOR_GenInstr_427
		db 112, Token.TYPE_REG8, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; XOR reg8 , imm8 
		dw opcode_XOR_GenInstr_428
		db 112, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM16, 0, 0 ; XOR reg16 , imm16 
		dw opcode_XOR_GenInstr_429
		db 112, Token.TYPE_REG16, Token.TYPE_VIRGULA, Token.TYPE_IMM8, 0, 0 ; XOR reg16 , imm8 
		dw opcode_XOR_GenInstr_430
		db 113, Token.TYPE_IMM8, 0, 0, 0, 0 ; DB imm8 
		dw opcode_DB_GenInstr_431
		db 114, Token.TYPE_IMM8, 0, 0, 0, 0 ; DW imm8 
		dw opcode_DW_GenInstr_432
		db 114, Token.TYPE_IMM16, 0, 0, 0, 0 ; DW imm16 
		dw opcode_DW_GenInstr_433
		db 114, Token.TYPE_IMM8, 0, 0, 0, 0 ; DW imm8 
		dw opcode_DW_GenInstr_434
		db 115, Token.TYPE_IMM8, 0, 0, 0, 0 ; ORG imm8 
		dw opcode_ORG_GenInstr_435

		db 115, Token.TYPE_IMM16, 0, 0, 0, 0 ; ORG imm16 
		dw opcode_ORG_GenInstr_436
		db 115, Token.TYPE_IMM8, 0, 0, 0, 0 ; ORG imm8 
		dw opcode_ORG_GenInstr_437

		db 116, Token.TYPE_IMM8, 0, 0, 0, 0 ; BITS imm8 
		dw opcode_BITS_GenInstr_438
		db 116, Token.TYPE_IMM16, 0, 0, 0, 0 ; BITS imm16 
		dw opcode_BITS_GenInstr_439
		db 116, Token.TYPE_IMM8, 0, 0, 0, 0 ; BITS imm8 
		dw opcode_BITS_GenInstr_440

		db 0



; --- Opcode generation instructions

        opcode_AAA_GenInstr_1 db 2, OG.WB, 037h ; w37 ; TESTED OK 08/02/2012
        opcode_AAS_GenInstr_2 db 2, OG.WB, 03Fh ; w3F ; TESTED OK 08/02/2012
        opcode_AAD_GenInstr_3 db 4, OG.WB, 0D5h, OG.WB, 00Ah ; wD5 w0A ; TESTED OK 08/02/2012

        opcode_AAD_GenInstr_4 db 3, OG.WB, 0D5h, OG.IB ; wD5 ib ; ---> NOT TESTED YET

        opcode_AAM_GenInstr_5 db 4, OG.WB, 0D4h, OG.WB, 00Ah ; wD4 w0A ; TESTED OK 08/02/2012

        opcode_AAM_GenInstr_6 db 3, OG.WB, 0D4h, OG.IB ; wD4 ib ; ---> NOT TESTED YET

        opcode_ADC_GenInstr_7 db 3, OG.WB, 010h, OG.RM1 ; w10 /r1 ; TESTED OK 08/02/2012 16:34

        opcode_ADC_GenInstr_8 db 3, OG.WB, 010h, OG.RM1 ; w10 /r1 ; TESTED OK 08/02/2012 17:18

        opcode_ADC_GenInstr_9 db 3, OG.WB, 011h, OG.RM1 ; o16 w11 /r1
        opcode_ADC_GenInstr_10 db 3, OG.WB, 011h, OG.RM1 ; o16 w11 /r1
        opcode_ADC_GenInstr_11 db 3, OG.WB, 012h, OG.RM2 ; w12 /r2
        opcode_ADC_GenInstr_12 db 3, OG.WB, 012h, OG.RM2 ; w12 /r2
        opcode_ADC_GenInstr_13 db 3, OG.WB, 013h, OG.RM2 ; o16 w13 /r2
        opcode_ADC_GenInstr_14 db 3, OG.WB, 013h, OG.RM2 ; o16 w13 /r2
        opcode_ADC_GenInstr_15 db 4, OG.WB, 080h, OG.B2, OG.IB ; w80 /2 ib
        opcode_ADC_GenInstr_16 db 4, OG.WB, 080h, OG.B2, OG.IB ; w80 /2 ib
        opcode_ADC_GenInstr_17 db 4, OG.WB, 081h, OG.B2, OG.IW ; o16 w81 /2 iw
        opcode_ADC_GenInstr_18 db 4, OG.WB, 081h, OG.B2, OG.IW ; o16 w81 /2 iw
        opcode_ADC_GenInstr_19 db 4, OG.WB, 081h, OG.B2, OG.IW ; o16 w81 /2 iw
        opcode_ADC_GenInstr_20 db 4, OG.WB, 081h, OG.B2, OG.IW ; o16 w81 /2 iw
        opcode_ADC_GenInstr_21 db 4, OG.WB, 083h, OG.B2, OG.IB ; o16 w83 /2 ib
        opcode_ADC_GenInstr_22 db 4, OG.WB, 083h, OG.B2, OG.IB ; o16 w83 /2 ib
;;;;;        opcode_ADC_GenInstr_23 db 3, OG.WB, 014h, OG.IB, *********************************** 
        opcode_ADC_GenInstr_23 db 0, 0  

;;;;;        opcode_ADC_GenInstr_24 db 3, OG.WB, 015h, OG.IW, *********************************** 
        opcode_ADC_GenInstr_24 db 0, 0  

;;;;;        opcode_ADC_GenInstr_25 db 3, OG.WB, 015h, OG.IW, *********************************** 
        opcode_ADC_GenInstr_25 db 0, 0  

        opcode_ADD_GenInstr_26 db 3, OG.WB, 000h, OG.RM1 ; w00 /r1
        opcode_ADD_GenInstr_27 db 3, OG.WB, 000h, OG.RM1 ; w00 /r1
        opcode_ADD_GenInstr_28 db 3, OG.WB, 001h, OG.RM1 ; o16 w01 /r1
        opcode_ADD_GenInstr_29 db 3, OG.WB, 001h, OG.RM1 ; o16 w01 /r1
        opcode_ADD_GenInstr_30 db 3, OG.WB, 002h, OG.RM2 ; w02 /r2
        opcode_ADD_GenInstr_31 db 3, OG.WB, 002h, OG.RM2 ; w02 /r2
        opcode_ADD_GenInstr_32 db 3, OG.WB, 003h, OG.RM2 ; o16 w03 /r2
        opcode_ADD_GenInstr_33 db 3, OG.WB, 003h, OG.RM2 ; o16 w03 /r2
        opcode_ADD_GenInstr_34 db 4, OG.WB, 080h, OG.B0, OG.IB ; w80 /0 ib
        opcode_ADD_GenInstr_35 db 4, OG.WB, 080h, OG.B0, OG.IB ; w80 /0 ib

        opcode_ADD_GenInstr_36 db 4, OG.WB, 081h, OG.B0, OG.IW ; o16 w81 /0 iw
        opcode_ADD_GenInstr_37 db 4, OG.WB, 081h, OG.B0, OG.GBIW ; o16 w81 /0 gbiw OK

        opcode_ADD_GenInstr_38 db 4, OG.WB, 081h, OG.B0, OG.IW ; o16 w81 /0 iw
        opcode_ADD_GenInstr_39 db 4, OG.WB, 081h, OG.B0, OG.IW ; o16 w81 /0 iw
        opcode_ADD_GenInstr_40 db 4, OG.WB, 083h, OG.B0, OG.IB ; o16 w83 /0 ib
        opcode_ADD_GenInstr_41 db 4, OG.WB, 083h, OG.B0, OG.IB ; o16 w83 /0 ib
;;;;;        opcode_ADD_GenInstr_42 db 3, OG.WB, 004h, OG.IB, *********************************** 
        opcode_ADD_GenInstr_42 db 0, 0  

;;;;;        opcode_ADD_GenInstr_43 db 3, OG.WB, 005h, OG.IW, *********************************** 
        opcode_ADD_GenInstr_43 db 0, 0  

;;;;;        opcode_ADD_GenInstr_44 db 3, OG.WB, 005h, OG.IW, *********************************** 
        opcode_ADD_GenInstr_44 db 0, 0  

        opcode_AND_GenInstr_45 db 3, OG.WB, 020h, OG.RM1 ; w20 /r1
        opcode_AND_GenInstr_46 db 3, OG.WB, 020h, OG.RM1 ; w20 /r1
        opcode_AND_GenInstr_47 db 3, OG.WB, 021h, OG.RM1 ; o16 w21 /r1
        opcode_AND_GenInstr_48 db 3, OG.WB, 021h, OG.RM1 ; o16 w21 /r1
        opcode_AND_GenInstr_49 db 3, OG.WB, 022h, OG.RM2 ; w22 /r2
        opcode_AND_GenInstr_50 db 3, OG.WB, 022h, OG.RM2 ; w22 /r2
        opcode_AND_GenInstr_51 db 3, OG.WB, 023h, OG.RM2 ; o16 w23 /r2
        opcode_AND_GenInstr_52 db 3, OG.WB, 023h, OG.RM2 ; o16 w23 /r2
        opcode_AND_GenInstr_53 db 4, OG.WB, 080h, OG.B4, OG.IB ; w80 /4 ib
        opcode_AND_GenInstr_54 db 4, OG.WB, 080h, OG.B4, OG.IB ; w80 /4 ib
        opcode_AND_GenInstr_55 db 4, OG.WB, 081h, OG.B4, OG.IW ; o16 w81 /4 iw
        opcode_AND_GenInstr_56 db 4, OG.WB, 081h, OG.B4, OG.IW ; o16 w81 /4 iw
        opcode_AND_GenInstr_57 db 4, OG.WB, 081h, OG.B4, OG.IW ; o16 w81 /4 iw
        opcode_AND_GenInstr_58 db 4, OG.WB, 081h, OG.B4, OG.IW ; o16 w81 /4 iw
        opcode_AND_GenInstr_59 db 4, OG.WB, 083h, OG.B4, OG.IB ; o16 w83 /4 ib
        opcode_AND_GenInstr_60 db 4, OG.WB, 083h, OG.B4, OG.IB ; o16 w83 /4 ib
;;;;;        opcode_AND_GenInstr_61 db 3, OG.WB, 024h, OG.IB, *********************************** 
        opcode_AND_GenInstr_61 db 0, 0  

;;;;;        opcode_AND_GenInstr_62 db 3, OG.WB, 025h, OG.IW, *********************************** 
        opcode_AND_GenInstr_62 db 0, 0  

;;;;;        opcode_AND_GenInstr_63 db 3, OG.WB, 025h, OG.IW, *********************************** 
        opcode_AND_GenInstr_63 db 0, 0  

        opcode_CALL_GenInstr_64 db 3, OG.WB, 0E8h, OG.RW ; wE8 rw
        opcode_CALL_GenInstr_65 db 3, OG.WB, 0E8h, OG.GBRW ; wE8 gbrw

        opcode_CALL_GenInstr_66 db 4, OG.WB, 09Ah, OG.IW, OG.IW ; o16 w9A iw iw
        opcode_CALL_GenInstr_67 db 4, OG.WB, 09Ah, OG.IW, OG.IW ; o16 w9A iw iw
        opcode_CALL_GenInstr_68 db 4, OG.WB, 09Ah, OG.IW, OG.IW ; o16 w9A iw iw
        opcode_CALL_GenInstr_69 db 4, OG.WB, 09Ah, OG.IW, OG.IW ; o16 w9A iw iw
        opcode_CALL_GenInstr_70 db 3, OG.WB, 0FFh, OG.B3 ; o16 wFF /3
        opcode_CALL_GenInstr_71 db 3, OG.WB, 0FFh, OG.B2 ; o16 wFF /2
        opcode_CALL_GenInstr_72 db 3, OG.WB, 0FFh, OG.B2 ; o16 wFF /2
        opcode_CBW_GenInstr_73 db 2, OG.WB, 098h ; o16 w98
        opcode_CWD_GenInstr_74 db 2, OG.WB, 099h ; o16 w99

        opcode_CLC_GenInstr_75 db 2, OG.WB, 0F8h ; wF8 ; OK 08/02/2012 16:18

        opcode_CLD_GenInstr_76 db 2, OG.WB, 0FCh ; wFC
        opcode_CLI_GenInstr_77 db 2, OG.WB, 0FAh ; wFA
        opcode_CMC_GenInstr_78 db 2, OG.WB, 0F5h ; wF5
        opcode_CMP_GenInstr_79 db 3, OG.WB, 038h, OG.RM1 ; w38 /r1
        opcode_CMP_GenInstr_80 db 3, OG.WB, 038h, OG.RM1 ; w38 /r1
        opcode_CMP_GenInstr_81 db 3, OG.WB, 039h, OG.RM1 ; o16 w39 /r1
        opcode_CMP_GenInstr_82 db 3, OG.WB, 039h, OG.RM1 ; o16 w39 /r1
        opcode_CMP_GenInstr_83 db 3, OG.WB, 03Ah, OG.RM2 ; w3A /r2
        opcode_CMP_GenInstr_84 db 3, OG.WB, 03Ah, OG.RM2 ; w3A /r2
        opcode_CMP_GenInstr_85 db 3, OG.WB, 03Bh, OG.RM2 ; o16 w3B /r2
        opcode_CMP_GenInstr_86 db 3, OG.WB, 03Bh, OG.RM2 ; o16 w3B /r2

        opcode_CMP_GenInstr_87 db 4, OG.WB, 080h, OG.B7, OG.IB ; w80 /7 ib (OK)

        opcode_CMP_GenInstr_88 db 4, OG.WB, 080h, OG.B0, OG.IB ; w80 /0 ib
        opcode_CMP_GenInstr_89 db 4, OG.WB, 081h, OG.B7, OG.IW ; o16 w81 /7 iw
        opcode_CMP_GenInstr_90 db 4, OG.WB, 081h, OG.B7, OG.IW ; o16 w81 /7 iw
        opcode_CMP_GenInstr_91 db 4, OG.WB, 081h, OG.B7, OG.IW ; o16 w81 /7 iw
        opcode_CMP_GenInstr_92 db 4, OG.WB, 081h, OG.B7, OG.IW ; o16 w81 /7 iw
        opcode_CMP_GenInstr_93 db 4, OG.WB, 083h, OG.B7, OG.IB ; o16 w83 /7 ib
        opcode_CMP_GenInstr_94 db 4, OG.WB, 083h, OG.B7, OG.IB ; o16 w83 /7 ib
;;;;;        opcode_CMP_GenInstr_95 db 3, OG.WB, 03Ch, OG.IB, *********************************** 
        opcode_CMP_GenInstr_95 db 0, 0  

;;;;;        opcode_CMP_GenInstr_96 db 3, OG.WB, 03Dh, OG.IW, *********************************** 
        opcode_CMP_GenInstr_96 db 0, 0  

;;;;;        opcode_CMP_GenInstr_97 db 3, OG.WB, 03Dh, OG.IW, *********************************** 
        opcode_CMP_GenInstr_97 db 0, 0  

        opcode_CMPSB_GenInstr_98 db 2, OG.WB, 0A6h ; wA6
        opcode_CMPSW_GenInstr_99 db 2, OG.WB, 0A7h ; o16 wA7
        opcode_DAA_GenInstr_100 db 2, OG.WB, 027h ; w27
        opcode_DAS_GenInstr_101 db 2, OG.WB, 02Fh ; w2F
        opcode_DEC_GenInstr_102 db 2, OG.PR, 048h ; o16 48+r
        opcode_DEC_GenInstr_103 db 3, OG.WB, 0FEh, OG.B1 ; wFE /1
        opcode_DEC_GenInstr_104 db 3, OG.WB, 0FEh, OG.B1 ; wFE /1
        opcode_DEC_GenInstr_105 db 3, OG.WB, 0FFh, OG.B1 ; o16 wFF /1
        opcode_DEC_GenInstr_106 db 3, OG.WB, 0FFh, OG.B1 ; o16 wFF /1
        opcode_DIV_GenInstr_107 db 3, OG.WB, 0F6h, OG.B6 ; wF6 /6
        opcode_DIV_GenInstr_108 db 3, OG.WB, 0F6h, OG.B6 ; wF6 /6
        opcode_DIV_GenInstr_109 db 3, OG.WB, 0F7h, OG.B6 ; o16 wF7 /6
        opcode_DIV_GenInstr_110 db 3, OG.WB, 0F7h, OG.B6 ; o16 wF7 /6
        opcode_HLT_GenInstr_111 db 2, OG.WB, 0F4h ; wF4
        opcode_IDIV_GenInstr_112 db 3, OG.WB, 0F6h, OG.B7 ; wF6 /7
        opcode_IDIV_GenInstr_113 db 3, OG.WB, 0F6h, OG.B7 ; wF6 /7
        opcode_IDIV_GenInstr_114 db 3, OG.WB, 0F7h, OG.B7 ; o16 wF7 /7
        opcode_IDIV_GenInstr_115 db 3, OG.WB, 0F7h, OG.B7 ; o16 wF7 /7
        opcode_IMUL_GenInstr_116 db 3, OG.WB, 0F6h, OG.B5 ; wF6 /5
        opcode_IMUL_GenInstr_117 db 3, OG.WB, 0F6h, OG.B5 ; wF6 /5
        opcode_IMUL_GenInstr_118 db 3, OG.WB, 0F7h, OG.B5 ; o16 wF7 /5
        opcode_IMUL_GenInstr_119 db 3, OG.WB, 0F7h, OG.B5 ; o16 wF7 /5


        opcode_IN_GenInstr_120 db 3, OG.WB, 0E4h, OG.IB ; wE4 ib amb1AL
        opcode_IN_GenInstr_121 db 3, OG.WB, 0E5h, OG.IB ; wE5 ib amb1AX
        opcode_IN_GenInstr_122 db 2, OG.WB, 0ECh        ; wEC amb1AL
        opcode_IN_GenInstr_123 db 2, OG.WB, 0EDh        ; wED amb1AX


        opcode_INC_GenInstr_124 db 2, OG.PR, 040h ; o16 40+r
        opcode_INC_GenInstr_125 db 3, OG.WB, 0FEh, OG.B0 ; wFE /0
        opcode_INC_GenInstr_126 db 3, OG.WB, 0FEh, OG.B0 ; wFE /0
        opcode_INC_GenInstr_127 db 3, OG.WB, 0FFh, OG.B0 ; o16 wFF /0
        opcode_INC_GenInstr_128 db 3, OG.WB, 0FFh, OG.B0 ; o16 wFF /0
        opcode_INT_GenInstr_129 db 3, OG.WB, 0CDh, OG.IB ; wCD ib
        opcode_INT3_GenInstr_130 db 2, OG.WB, 0CCh ; wCC
        opcode_INTO_GenInstr_131 db 2, OG.WB, 0CEh ; wCE
        opcode_IRET_GenInstr_132 db 2, OG.WB, 0CFh ; wCF
        opcode_IRETW_GenInstr_133 db 2, OG.WB, 0CFh ; o16 wCF
        opcode_JCXZ_GenInstr_134 db 3, OG.WB, 0E3h, OG.RB ; o16 wE3 rb
        opcode_JMP_GenInstr_135 db 3, OG.WB, 0E9h, OG.RW ; wE9 rw
        opcode_JMP_GenInstr_136 db 3, OG.WB, 0E9h, OG.RW ; wE9 rw
        opcode_JMP_GenInstr_137 db 3, OG.WB, 0E9h, OG.RW ; wE9 rw

        opcode_JMP_GenInstr_138 db 3, OG.WB, 0EBh, OG.RB ; wEB rb

        opcode_JMP_GenInstr_139 db 4, OG.WB, 0EAh, OG.IW, OG.IW ; o16 wEA iw iw
        opcode_JMP_GenInstr_140 db 4, OG.WB, 0EAh, OG.IW, OG.IW ; o16 wEA iw iw
        opcode_JMP_GenInstr_141 db 4, OG.WB, 0EAh, OG.IW, OG.IW ; o16 wEA iw iw
        opcode_JMP_GenInstr_142 db 4, OG.WB, 0EAh, OG.IW, OG.IW ; o16 wEA iw iw
        opcode_JMP_GenInstr_143 db 3, OG.WB, 0FFh, OG.B5 ; o16 wFF /5
        opcode_JMP_GenInstr_144 db 3, OG.WB, 0FFh, OG.B4 ; o16 wFF /4
        opcode_JMP_GenInstr_145 db 3, OG.WB, 0FFh, OG.B4 ; o16 wFF /4


        opcode_JO_GenInstr_146 db 3, OG.WB, 070h, OG.RB ; w70 rb
        opcode_JNO_GenInstr_147 db 3, OG.WB, 071h, OG.RB ; w71 rb
        opcode_JB_GenInstr_148 db 3, OG.WB, 072h, OG.RB ; w72 rb
        opcode_JC_GenInstr_149 db 3, OG.WB, 072h, OG.RB ; w72 rb
        opcode_JNAE_GenInstr_150 db 3, OG.WB, 072h, OG.RB ; w72 rb
        opcode_JAE_GenInstr_151 db 3, OG.WB, 073h, OG.RB ; w73 rb
        opcode_JNB_GenInstr_152 db 3, OG.WB, 073h, OG.RB ; w73 rb
        opcode_JNC_GenInstr_153 db 3, OG.WB, 073h, OG.RB ; w73 rb
        opcode_JE_GenInstr_154 db 3, OG.WB, 074h, OG.RB ; w74 rb
        opcode_JZ_GenInstr_155 db 3, OG.WB, 074h, OG.RB ; w74 rb
        opcode_JNE_GenInstr_156 db 3, OG.WB, 075h, OG.RB ; w75 rb
        opcode_JNZ_GenInstr_157 db 3, OG.WB, 075h, OG.RB ; w75 rb
        opcode_JBE_GenInstr_158 db 3, OG.WB, 076h, OG.RB ; w76 rb
        opcode_JNA_GenInstr_159 db 3, OG.WB, 076h, OG.RB ; w76 rb
        opcode_JA_GenInstr_160 db 3, OG.WB, 077h, OG.RB ; w77 rb
        opcode_JNBE_GenInstr_161 db 3, OG.WB, 077h, OG.RB ; w77 rb
        opcode_JS_GenInstr_162 db 3, OG.WB, 078h, OG.RB ; w78 rb
        opcode_JNS_GenInstr_163 db 3, OG.WB, 079h, OG.RB ; w79 rb
        opcode_JP_GenInstr_164 db 3, OG.WB, 07Ah, OG.RB ; w7A rb
        opcode_JPE_GenInstr_165 db 3, OG.WB, 07Ah, OG.RB ; w7A rb
        opcode_JNP_GenInstr_166 db 3, OG.WB, 07Bh, OG.RB ; w7B rb
        opcode_JPO_GenInstr_167 db 3, OG.WB, 07Bh, OG.RB ; w7B rb
        opcode_JL_GenInstr_168 db 3, OG.WB, 07Ch, OG.RB ; w7C rb
        opcode_JNGE_GenInstr_169 db 3, OG.WB, 07Ch, OG.RB ; w7C rb
        opcode_JGE_GenInstr_170 db 3, OG.WB, 07Dh, OG.RB ; w7D rb
        opcode_JNL_GenInstr_171 db 3, OG.WB, 07Dh, OG.RB ; w7D rb
        opcode_JLE_GenInstr_172 db 3, OG.WB, 07Eh, OG.RB ; w7E rb
        opcode_JNG_GenInstr_173 db 3, OG.WB, 07Eh, OG.RB ; w7E rb
        opcode_JG_GenInstr_174 db 3, OG.WB, 07Fh, OG.RB ; w7F rb
        opcode_JNLE_GenInstr_175 db 3, OG.WB, 07Fh, OG.RB ; w7F rb


        opcode_LAHF_GenInstr_176 db 2, OG.WB, 09Fh ; w9F
        opcode_LDS_GenInstr_177 db 3, OG.WB, 0C5h, OG.BR ; o16 wC5 /r
        opcode_LES_GenInstr_178 db 3, OG.WB, 0C4h, OG.BR ; o16 wC4 /r
        opcode_LEA_GenInstr_179 db 3, OG.WB, 08Dh, OG.BR ; o16 w8D /r
        opcode_LODSB_GenInstr_180 db 2, OG.WB, 0ACh ; wAC
        opcode_LODSW_GenInstr_181 db 2, OG.WB, 0ADh ; o16 wAD
        opcode_LOOP_GenInstr_182 db 3, OG.WB, 0E2h, OG.RB ; wE2 rb
;;;;;        opcode_LOOP_GenInstr_183 db 3, OG.WB, 0E2h, OG.RB, *********************************** 
        opcode_LOOP_GenInstr_183 db 0, 0  

        opcode_LOOPE_GenInstr_184 db 3, OG.WB, 0E1h, OG.RB ; wE1 rb
;;;;;        opcode_LOOPE_GenInstr_185 db 3, OG.WB, 0E1h, OG.RB, *********************************** 
        opcode_LOOPE_GenInstr_185 db 0, 0  

        opcode_LOOPZ_GenInstr_186 db 3, OG.WB, 0E1h, OG.RB ; wE1 rb
;;;;;        opcode_LOOPZ_GenInstr_187 db 3, OG.WB, 0E1h, OG.RB, *********************************** 
        opcode_LOOPZ_GenInstr_187 db 0, 0  

        opcode_LOOPNE_GenInstr_188 db 3, OG.WB, 0E0h, OG.RB ; wE0 rb
;;;;;        opcode_LOOPNE_GenInstr_189 db 3, OG.WB, 0E0h, OG.RB, *********************************** 
        opcode_LOOPNE_GenInstr_189 db 0, 0  

        opcode_LOOPNZ_GenInstr_190 db 3, OG.WB, 0E0h, OG.RB ; wE0 rb
;;;;;        opcode_LOOPNZ_GenInstr_191 db 3, OG.WB, 0E0h, OG.RB, *********************************** 
        opcode_LOOPNZ_GenInstr_191 db 0, 0  

        opcode_MOV_GenInstr_192 db 3, OG.WB, 088h, OG.RM1 ; w88 /r1
        opcode_MOV_GenInstr_193 db 3, OG.WB, 088h, OG.RM1 ; w88 /r1
        opcode_MOV_GenInstr_194 db 3, OG.WB, 089h, OG.RM1 ; o16 w89 /r1
        opcode_MOV_GenInstr_195 db 3, OG.WB, 089h, OG.RM1 ; o16 w89 /r1
        opcode_MOV_GenInstr_196 db 3, OG.WB, 08Ah, OG.RM2 ; w8A /r2
        opcode_MOV_GenInstr_197 db 3, OG.WB, 08Ah, OG.RM2 ; w8A /r2
        opcode_MOV_GenInstr_198 db 3, OG.WB, 08Bh, OG.RM2 ; o16 w8B /r2
        opcode_MOV_GenInstr_199 db 3, OG.WB, 08Bh, OG.RM2 ; o16 w8B /r2
        opcode_MOV_GenInstr_200 db 3, OG.PR, 0B0h, OG.IB ; B0+r ib
        opcode_MOV_GenInstr_201 db 3, OG.PR, 0B8h, OG.IW ; o16 B8+r iw

        opcode_MOV_GenInstr_202 db 3, OG.PR, 0B8h, OG.GBIW ; o16 B8+r gbiw ; OK

        opcode_MOV_GenInstr_203 db 4, OG.WB, 0C6h, OG.B0, OG.IB ; wC6 /0 ib
        opcode_MOV_GenInstr_204 db 4, OG.WB, 0C6h, OG.B0, OG.IB ; wC6 /0 ib
        opcode_MOV_GenInstr_205 db 4, OG.WB, 0C7h, OG.B0, OG.IW ; o16 wC7 /0 iw
        opcode_MOV_GenInstr_206 db 4, OG.WB, 0C7h, OG.B0, OG.IW ; o16 wC7 /0 iw
        opcode_MOV_GenInstr_207 db 4, OG.WB, 0C7h, OG.B0, OG.IW ; o16 wC7 /0 iw
        opcode_MOV_GenInstr_208 db 4, OG.WB, 0C7h, OG.B0, OG.GBIW ; o16 wC7 /0 gbiw

;;;;;        opcode_MOV_GenInstr_209 db 3, OG.WB, 0A0h, OG.OW, *********************************** 
        opcode_MOV_GenInstr_209 db 0, 0  

;;;;;        opcode_MOV_GenInstr_210 db 3, OG.WB, 0A1h, OG.OW, *********************************** 
        opcode_MOV_GenInstr_210 db 0, 0  

;;;;;        opcode_MOV_GenInstr_211 db 3, OG.WB, 0A2h, OG.OW, *********************************** 
        opcode_MOV_GenInstr_211 db 0, 0  

;;;;;        opcode_MOV_GenInstr_212 db 3, OG.WB, 0A3h, OG.OW, *********************************** 
        opcode_MOV_GenInstr_212 db 0, 0  

        opcode_MOV_GenInstr_213 db 3, OG.WB, 08Ch, OG.RM1 ; o16 w8C /r1
        opcode_MOV_GenInstr_214 db 3, OG.WB, 08Ch, OG.RM1 ; o16 w8C /r1
        opcode_MOV_GenInstr_215 db 3, OG.WB, 08Eh, OG.RM2 ; o16 w8E /r2
        opcode_MOV_GenInstr_216 db 3, OG.WB, 08Eh, OG.RM2 ; o16 w8E /r2
        opcode_MOVSB_GenInstr_217 db 2, OG.WB, 0A4h ; wA4
        opcode_MOVSW_GenInstr_218 db 2, OG.WB, 0A5h ; o16 wA5
        opcode_MUL_GenInstr_219 db 3, OG.WB, 0F6h, OG.B4 ; wF6 /4
        opcode_MUL_GenInstr_220 db 3, OG.WB, 0F6h, OG.B4 ; wF6 /4
        opcode_MUL_GenInstr_221 db 3, OG.WB, 0F7h, OG.B4 ; o16 wF7 /4
        opcode_MUL_GenInstr_222 db 3, OG.WB, 0F7h, OG.B4 ; o16 wF7 /4
        opcode_NEG_GenInstr_223 db 3, OG.WB, 0F6h, OG.B3 ; wF6 /3
        opcode_NEG_GenInstr_224 db 3, OG.WB, 0F6h, OG.B3 ; wF6 /3
        opcode_NEG_GenInstr_225 db 3, OG.WB, 0F7h, OG.B3 ; o16 wF7 /3
        opcode_NEG_GenInstr_226 db 3, OG.WB, 0F7h, OG.B3 ; o16 wF7 /3
        opcode_NOT_GenInstr_227 db 3, OG.WB, 0F6h, OG.B2 ; wF6 /2
        opcode_NOT_GenInstr_228 db 3, OG.WB, 0F6h, OG.B2 ; wF6 /2
        opcode_NOT_GenInstr_229 db 3, OG.WB, 0F7h, OG.B2 ; o16 wF7 /2
        opcode_NOT_GenInstr_230 db 3, OG.WB, 0F7h, OG.B2 ; o16 wF7 /2
        opcode_NOP_GenInstr_231 db 2, OG.WB, 090h ; w90
        opcode_OR_GenInstr_232 db 3, OG.WB, 008h, OG.RM1 ; w08 /r1
        opcode_OR_GenInstr_233 db 3, OG.WB, 008h, OG.RM1 ; w08 /r1
        opcode_OR_GenInstr_234 db 3, OG.WB, 009h, OG.RM1 ; o16 w09 /r1
        opcode_OR_GenInstr_235 db 3, OG.WB, 009h, OG.RM1 ; o16 w09 /r1
        opcode_OR_GenInstr_236 db 3, OG.WB, 00Ah, OG.RM2 ; w0A /r2
        opcode_OR_GenInstr_237 db 3, OG.WB, 00Ah, OG.RM2 ; w0A /r2
        opcode_OR_GenInstr_238 db 3, OG.WB, 00Bh, OG.RM2 ; o16 w0B /r2
        opcode_OR_GenInstr_239 db 3, OG.WB, 00Bh, OG.RM2 ; o16 w0B /r2
        opcode_OR_GenInstr_240 db 4, OG.WB, 080h, OG.B1, OG.IB ; w80 /1 ib
        opcode_OR_GenInstr_241 db 4, OG.WB, 080h, OG.B1, OG.IB ; w80 /1 ib
        opcode_OR_GenInstr_242 db 4, OG.WB, 081h, OG.B1, OG.IW ; o16 w81 /1 iw
        opcode_OR_GenInstr_243 db 4, OG.WB, 081h, OG.B1, OG.IW ; o16 w81 /1 iw
        opcode_OR_GenInstr_244 db 4, OG.WB, 081h, OG.B1, OG.IW ; o16 w81 /1 iw
        opcode_OR_GenInstr_245 db 4, OG.WB, 081h, OG.B1, OG.IW ; o16 w81 /1 iw
        opcode_OR_GenInstr_246 db 4, OG.WB, 083h, OG.B1, OG.IB ; o16 w83 /1 ib
        opcode_OR_GenInstr_247 db 4, OG.WB, 083h, OG.B1, OG.IB ; o16 w83 /1 ib
;;;;;        opcode_OR_GenInstr_248 db 3, OG.WB, 00Ch, OG.IB, *********************************** 
        opcode_OR_GenInstr_248 db 0, 0  

;;;;;        opcode_OR_GenInstr_249 db 3, OG.WB, 00Dh, OG.IW, *********************************** 
        opcode_OR_GenInstr_249 db 0, 0  

;;;;;        opcode_OR_GenInstr_250 db 3, OG.WB, 00Dh, OG.IW, *********************************** 
        opcode_OR_GenInstr_250 db 0, 0  



        opcode_OUT_GenInstr_251 db 3, OG.WB, 0E6h, OG.IB ; wE6 ib
        opcode_OUT_GenInstr_252 db 3, OG.WB, 0E7h, OG.IB ; wE7 ib
        opcode_OUT_GenInstr_253 db 2, OG.WB, 0EEh,       ; wEE
        opcode_OUT_GenInstr_254 db 2, OG.WB, 0EFh,       ; wEF



        opcode_POP_GenInstr_255 db 2, OG.PR, 058h ; o16 58+r
        opcode_POP_GenInstr_256 db 3, OG.WB, 08Fh, OG.B0 ; o16 w8F /0
        opcode_POP_GenInstr_257 db 3, OG.WB, 08Fh, OG.B0 ; o16 w8F /0

        opcode_POP_GenInstr_258 db 2, OG.WB, 01Fh ; 1F amb1DS
        opcode_POP_GenInstr_259 db 2, OG.WB, 007h ; 07 amb1ES
        opcode_POP_GenInstr_260 db 2, OG.WB, 017h ; 17 amb1SS

        opcode_PUSH_GenInstr_261 db 2, OG.PR, 050h ; o16 50+r
        opcode_PUSH_GenInstr_262 db 3, OG.WB, 0FFh, OG.B6 ; o16 wFF /6
        opcode_PUSH_GenInstr_263 db 3, OG.WB, 0FFh, OG.B6 ; o16 wFF /6

        opcode_PUSH_GenInstr_264 db 2, OG.WB, 00Eh ; w0E amb1CS
        opcode_PUSH_GenInstr_265 db 2, OG.WB, 01Eh ; w1E amb1DS
        opcode_PUSH_GenInstr_266 db 2, OG.WB, 006h ; w06 amb1ES
        opcode_PUSH_GenInstr_267 db 2, OG.WB, 016h ; w16 amb1SS




        opcode_RCL_GenInstr_268 db 4, OG.WB, 0D0h, OG.B2, OG.A2MBIMM1 ; wD0 /2 a2mbimm1
        opcode_RCL_GenInstr_269 db 4, OG.WB, 0D0h, OG.B2, OG.A2MBIMM1 ; wD0 /2 a2mbimm1
        opcode_RCL_GenInstr_270 db 4, OG.WB, 0D2h, OG.B2, OG.A2MBCL   ; wD2 /2 a2mbCL
        opcode_RCL_GenInstr_271 db 4, OG.WB, 0D2h, OG.B2, OG.A2MBCL   ; wD2 /2 a2mbCL
        opcode_RCL_GenInstr_272 db 4, OG.WB, 0D1h, OG.B2, OG.A2MBIMM1 ; wD1 /2 a2mbimm1
        opcode_RCL_GenInstr_273 db 4, OG.WB, 0D1h, OG.B2, OG.A2MBIMM1 ; wD1 /2 a2mbimm1
        opcode_RCL_GenInstr_274 db 4, OG.WB, 0D3h, OG.B2, OG.A2MBCL   ; wD3 /2 a2mbCL
        opcode_RCL_GenInstr_275 db 4, OG.WB, 0D3h, OG.B2, OG.A2MBCL   ; wD3 /2 a2mbCL



        opcode_RCR_GenInstr_276 db 3, OG.WB, 0D0h, OG.B3, OG.A2MBIMM1 ; wD0 /3 a2mbimm1
        opcode_RCR_GenInstr_277 db 3, OG.WB, 0D0h, OG.B3, OG.A2MBIMM1 ; wD0 /3 a2mbimm1
        opcode_RCR_GenInstr_278 db 3, OG.WB, 0D2h, OG.B3, OG.A2MBCL   ; wD2 /3 a2mbCL
        opcode_RCR_GenInstr_279 db 3, OG.WB, 0D2h, OG.B3, OG.A2MBCL   ; wD2 /3 a2mbCL
        opcode_RCR_GenInstr_280 db 3, OG.WB, 0D1h, OG.B3, OG.A2MBIMM1 ; wD1 /3 a2mbimm1
        opcode_RCR_GenInstr_281 db 3, OG.WB, 0D1h, OG.B3, OG.A2MBIMM1 ; wD1 /3 a2mbimm1
        opcode_RCR_GenInstr_282 db 3, OG.WB, 0D3h, OG.B3, OG.A2MBCL   ; wD3 /3 a2mbCL
        opcode_RCR_GenInstr_283 db 3, OG.WB, 0D3h, OG.B3, OG.A2MBCL   ; wD3 /3 a2mbCL



        opcode_RET_GenInstr_284 db 2, OG.WB, 0C3h ; wC3
        opcode_RET_GenInstr_285 db 3, OG.WB, 0C2h, OG.IW ; wC2 iw
        opcode_RET_GenInstr_286 db 3, OG.WB, 0C2h, OG.IW ; wC2 iw
        opcode_RETF_GenInstr_287 db 2, OG.WB, 0CBh ; wCB
        opcode_RETF_GenInstr_288 db 3, OG.WB, 0CAh, OG.IW ; wCA iw
        opcode_RETF_GenInstr_289 db 3, OG.WB, 0CAh, OG.IW ; wCA iw
        opcode_RETN_GenInstr_290 db 2, OG.WB, 0C3h ; wC3
        opcode_RETN_GenInstr_291 db 3, OG.WB, 0C2h, OG.IW ; wC2 iw
        opcode_RETN_GenInstr_292 db 3, OG.WB, 0C2h, OG.IW ; wC2 iw



        opcode_ROL_GenInstr_293 db 3, OG.WB, 0D0h, OG.B0, OG.A2MBIMM1 ; wD0 /0 a2mbimm1
        opcode_ROL_GenInstr_294 db 3, OG.WB, 0D0h, OG.B0, OG.A2MBIMM1 ; wD0 /0 a2mbimm1
        opcode_ROL_GenInstr_295 db 3, OG.WB, 0D2h, OG.B0, OG.A2MBCL   ; wD2 /0 a2mbCL
        opcode_ROL_GenInstr_296 db 3, OG.WB, 0D2h, OG.B0, OG.A2MBCL   ; wD2 /0 a2mbCL
        opcode_ROL_GenInstr_297 db 3, OG.WB, 0D1h, OG.B0, OG.A2MBIMM1 ; wD1 /0 a2mbimm1
        opcode_ROL_GenInstr_298 db 3, OG.WB, 0D1h, OG.B0, OG.A2MBIMM1 ; wD1 /0 a2mbimm1
        opcode_ROL_GenInstr_299 db 3, OG.WB, 0D3h, OG.B0, OG.A2MBCL   ; wD3 /0 a2mbCL
        opcode_ROL_GenInstr_300 db 3, OG.WB, 0D3h, OG.B0, OG.A2MBCL   ; wD3 /0 a2mbCL




        opcode_ROR_GenInstr_301 db 3, OG.WB, 0D0h, OG.B1, OG.A2MBIMM1 ; wD0 /1 a2mbimm1
        opcode_ROR_GenInstr_302 db 3, OG.WB, 0D0h, OG.B1, OG.A2MBIMM1 ; wD0 /1 a2mbimm1
        opcode_ROR_GenInstr_303 db 3, OG.WB, 0D2h, OG.B1, OG.A2MBCL   ; wD2 /1 a2mbCL
        opcode_ROR_GenInstr_304 db 3, OG.WB, 0D2h, OG.B1, OG.A2MBCL   ; wD2 /1 a2mbCL
        opcode_ROR_GenInstr_305 db 3, OG.WB, 0D1h, OG.B1, OG.A2MBIMM1 ; wD1 /1 a2mbimm1
        opcode_ROR_GenInstr_306 db 3, OG.WB, 0D1h, OG.B1, OG.A2MBIMM1 ; wD1 /1 a2mbimm1
        opcode_ROR_GenInstr_307 db 3, OG.WB, 0D3h, OG.B1, OG.A2MBCL   ; wD3 /1 a2mbCL
        opcode_ROR_GenInstr_308 db 3, OG.WB, 0D3h, OG.B1, OG.A2MBCL   ; wD3 /1 a2mbCL




        opcode_SAHF_GenInstr_309 db 2, OG.WB, 09Eh ; w9E




        opcode_SAL_GenInstr_310 db 3, OG.WB, 0D0h, OG.B4, OG.A2MBIMM1 ; wD0 /4 a2mbimm1
        opcode_SAL_GenInstr_311 db 3, OG.WB, 0D0h, OG.B4, OG.A2MBIMM1 ; wD0 /4 a2mbimm1
        opcode_SAL_GenInstr_312 db 3, OG.WB, 0D2h, OG.B4, OG.A2MBCL   ; wD2 /4 a2mbCL
        opcode_SAL_GenInstr_313 db 3, OG.WB, 0D2h, OG.B4, OG.A2MBCL   ; wD2 /4 a2mbCL
        opcode_SAL_GenInstr_314 db 3, OG.WB, 0D1h, OG.B4, OG.A2MBIMM1 ; wD1 /4 a2mbimm1
        opcode_SAL_GenInstr_315 db 3, OG.WB, 0D1h, OG.B4, OG.A2MBIMM1 ; wD1 /4 a2mbimm1
        opcode_SAL_GenInstr_316 db 3, OG.WB, 0D3h, OG.B4, OG.A2MBCL   ; wD3 /4 a2mbCL
        opcode_SAL_GenInstr_317 db 3, OG.WB, 0D3h, OG.B4, OG.A2MBCL   ; wD3 /4 a2mbCL




        opcode_SAR_GenInstr_318 db 3, OG.WB, 0D0h, OG.B7, OG.A2MBIMM1 ; wD0 /7 a2mbimm1
        opcode_SAR_GenInstr_319 db 3, OG.WB, 0D0h, OG.B7, OG.A2MBIMM1 ; wD0 /7 a2mbimm1
        opcode_SAR_GenInstr_320 db 3, OG.WB, 0D2h, OG.B7, OG.A2MBCL   ; wD2 /7 a2mbCL
        opcode_SAR_GenInstr_321 db 3, OG.WB, 0D2h, OG.B7, OG.A2MBCL   ; wD2 /7 a2mbCL
        opcode_SAR_GenInstr_322 db 3, OG.WB, 0D1h, OG.B7, OG.A2MBIMM1 ; wD1 /7 a2mbimm1
        opcode_SAR_GenInstr_323 db 3, OG.WB, 0D1h, OG.B7, OG.A2MBIMM1 ; wD1 /7 a2mbimm1
        opcode_SAR_GenInstr_324 db 3, OG.WB, 0D3h, OG.B7, OG.A2MBCL   ; wD3 /7 a2mbCL
        opcode_SAR_GenInstr_325 db 3, OG.WB, 0D3h, OG.B7, OG.A2MBCL   ; wD3 /7 a2mbCL





        opcode_SBB_GenInstr_326 db 3, OG.WB, 018h, OG.RM1 ; w18 /r1
        opcode_SBB_GenInstr_327 db 3, OG.WB, 018h, OG.RM1 ; w18 /r1
        opcode_SBB_GenInstr_328 db 3, OG.WB, 019h, OG.RM1 ; o16 w19 /r1
        opcode_SBB_GenInstr_329 db 3, OG.WB, 019h, OG.RM1 ; o16 w19 /r1
        opcode_SBB_GenInstr_330 db 3, OG.WB, 01Ah, OG.RM2 ; w1A /r2
        opcode_SBB_GenInstr_331 db 3, OG.WB, 01Ah, OG.RM2 ; w1A /r2
        opcode_SBB_GenInstr_332 db 3, OG.WB, 01Bh, OG.RM2 ; o16 w1B /r2
        opcode_SBB_GenInstr_333 db 3, OG.WB, 01Bh, OG.RM2 ; o16 w1B /r2
        opcode_SBB_GenInstr_334 db 4, OG.WB, 080h, OG.B3, OG.IB ; w80 /3 ib
        opcode_SBB_GenInstr_335 db 4, OG.WB, 080h, OG.B3, OG.IB ; w80 /3 ib
        opcode_SBB_GenInstr_336 db 4, OG.WB, 081h, OG.B3, OG.IW ; o16 w81 /3 iw
        opcode_SBB_GenInstr_337 db 4, OG.WB, 081h, OG.B3, OG.IW ; o16 w81 /3 iw
        opcode_SBB_GenInstr_338 db 4, OG.WB, 081h, OG.B3, OG.IW ; o16 w81 /3 iw
        opcode_SBB_GenInstr_339 db 4, OG.WB, 081h, OG.B3, OG.IW ; o16 w81 /3 iw
        opcode_SBB_GenInstr_340 db 4, OG.WB, 083h, OG.B3, OG.IB ; o16 w83 /3 ib
        opcode_SBB_GenInstr_341 db 4, OG.WB, 083h, OG.B3, OG.IB ; o16 w83 /3 ib
;;;;;        opcode_SBB_GenInstr_342 db 3, OG.WB, 01Ch, OG.IB, *********************************** 
        opcode_SBB_GenInstr_342 db 0, 0  

;;;;;        opcode_SBB_GenInstr_343 db 3, OG.WB, 01Dh, OG.IW, *********************************** 
        opcode_SBB_GenInstr_343 db 0, 0  

;;;;;        opcode_SBB_GenInstr_344 db 3, OG.WB, 01Dh, OG.IW, *********************************** 
        opcode_SBB_GenInstr_344 db 0, 0  

        opcode_SCASB_GenInstr_345 db 2, OG.WB, 0AEh ; wAE
        opcode_SCASW_GenInstr_346 db 2, OG.WB, 0AFh ; o16 wAF



        opcode_SHL_GenInstr_347 db 3, OG.WB, 0D0h, OG.B4, OG.A2MBIMM1 ; wD0 /4 a2mbimm1
        opcode_SHL_GenInstr_348 db 3, OG.WB, 0D0h, OG.B4, OG.A2MBIMM1 ; wD0 /4 a2mbimm1
        opcode_SHL_GenInstr_349 db 3, OG.WB, 0D2h, OG.B4, OG.A2MBCL   ; wD2 /4 a2mbCL
        opcode_SHL_GenInstr_350 db 3, OG.WB, 0D2h, OG.B4, OG.A2MBCL   ; wD2 /4 a2mbCL
        opcode_SHL_GenInstr_351 db 3, OG.WB, 0D1h, OG.B4, OG.A2MBIMM1 ; wD1 /4 a2mbimm1
        opcode_SHL_GenInstr_352 db 3, OG.WB, 0D1h, OG.B4, OG.A2MBIMM1 ; wD1 /4 a2mbimm1
        opcode_SHL_GenInstr_353 db 3, OG.WB, 0D3h, OG.B4, OG.A2MBCL   ; wD3 /4 a2mbCL
        opcode_SHL_GenInstr_354 db 3, OG.WB, 0D3h, OG.B4, OG.A2MBCL   ; wD3 /4 a2mbCL



        opcode_SHR_GenInstr_355 db 3, OG.WB, 0D0h, OG.B5, OG.A2MBIMM1 ; wD0 /5 a2mbimm1
        opcode_SHR_GenInstr_356 db 3, OG.WB, 0D0h, OG.B5, OG.A2MBIMM1 ; wD0 /5 a2mbimm1
        opcode_SHR_GenInstr_357 db 3, OG.WB, 0D2h, OG.B5, OG.A2MBCL   ; wD2 /5 a2mbCL
        opcode_SHR_GenInstr_358 db 3, OG.WB, 0D2h, OG.B5, OG.A2MBCL   ; wD2 /5 a2mbCL
        opcode_SHR_GenInstr_359 db 3, OG.WB, 0D1h, OG.B5, OG.A2MBIMM1 ; wD1 /5 a2mbimm1
        opcode_SHR_GenInstr_360 db 3, OG.WB, 0D1h, OG.B5, OG.A2MBIMM1 ; wD1 /5 a2mbimm1
        opcode_SHR_GenInstr_361 db 3, OG.WB, 0D3h, OG.B5, OG.A2MBCL   ; wD3 /5 a2mbCL
        opcode_SHR_GenInstr_362 db 3, OG.WB, 0D3h, OG.B5, OG.A2MBCL   ; wD3 /5 a2mbCL



        opcode_STC_GenInstr_363 db 2, OG.WB, 0F9h ; wF9 ; -- OK 16/02/2012 16:10

        opcode_STD_GenInstr_364 db 2, OG.WB, 0FDh ; wFD
        opcode_STI_GenInstr_365 db 2, OG.WB, 0FBh ; wFB
        opcode_STOSB_GenInstr_366 db 2, OG.WB, 0AAh ; wAA
        opcode_STOSW_GenInstr_367 db 2, OG.WB, 0ABh ; o16 wAB
        opcode_SUB_GenInstr_368 db 3, OG.WB, 028h, OG.RM1 ; w28 /r1
        opcode_SUB_GenInstr_369 db 3, OG.WB, 028h, OG.RM1 ; w28 /r1
        opcode_SUB_GenInstr_370 db 3, OG.WB, 029h, OG.RM1 ; o16 w29 /r1
        opcode_SUB_GenInstr_371 db 3, OG.WB, 029h, OG.RM1 ; o16 w29 /r1
        opcode_SUB_GenInstr_372 db 3, OG.WB, 02Ah, OG.RM2 ; w2A /r2
        opcode_SUB_GenInstr_373 db 3, OG.WB, 02Ah, OG.RM2 ; w2A /r2
        opcode_SUB_GenInstr_374 db 3, OG.WB, 02Bh, OG.RM2 ; o16 w2B /r2
        opcode_SUB_GenInstr_375 db 3, OG.WB, 02Bh, OG.RM2 ; o16 w2B /r2
        opcode_SUB_GenInstr_376 db 4, OG.WB, 080h, OG.B5, OG.IB ; w80 /5 ib
        opcode_SUB_GenInstr_377 db 4, OG.WB, 080h, OG.B5, OG.IB ; w80 /5 ib
        opcode_SUB_GenInstr_378 db 4, OG.WB, 081h, OG.B5, OG.IW ; o16 w81 /5 iw
        opcode_SUB_GenInstr_379 db 4, OG.WB, 081h, OG.B5, OG.IW ; o16 w81 /5 iw
        opcode_SUB_GenInstr_380 db 4, OG.WB, 081h, OG.B5, OG.IW ; o16 w81 /5 iw
        opcode_SUB_GenInstr_381 db 4, OG.WB, 081h, OG.B5, OG.IW ; o16 w81 /5 iw
        opcode_SUB_GenInstr_382 db 4, OG.WB, 083h, OG.B5, OG.IB ; o16 w83 /5 ib
        opcode_SUB_GenInstr_383 db 4, OG.WB, 083h, OG.B5, OG.IB ; o16 w83 /5 ib
;;;;;        opcode_SUB_GenInstr_384 db 3, OG.WB, 02Ch, OG.IB, *********************************** 
        opcode_SUB_GenInstr_384 db 0, 0  

;;;;;        opcode_SUB_GenInstr_385 db 3, OG.WB, 02Dh, OG.IW, *********************************** 
        opcode_SUB_GenInstr_385 db 0, 0  

;;;;;        opcode_SUB_GenInstr_386 db 3, OG.WB, 02Dh, OG.IW, *********************************** 
        opcode_SUB_GenInstr_386 db 0, 0  

        opcode_TEST_GenInstr_387 db 3, OG.WB, 084h, OG.RM1 ; w84 /r1
        opcode_TEST_GenInstr_388 db 3, OG.WB, 084h, OG.RM1 ; w84 /r1
        opcode_TEST_GenInstr_389 db 3, OG.WB, 085h, OG.RM1 ; o16 w85 /r1
        opcode_TEST_GenInstr_390 db 3, OG.WB, 085h, OG.RM1 ; o16 w85 /r1
        opcode_TEST_GenInstr_391 db 4, OG.WB, 0F6h, OG.B7, OG.IB ; wF6 /7 ib
        opcode_TEST_GenInstr_392 db 4, OG.WB, 0F6h, OG.B7, OG.IB ; wF6 /7 ib
        opcode_TEST_GenInstr_393 db 4, OG.WB, 0F7h, OG.B7, OG.IW ; o16 wF7 /7 iw
        opcode_TEST_GenInstr_394 db 4, OG.WB, 0F7h, OG.B7, OG.IW ; o16 wF7 /7 iw
        opcode_TEST_GenInstr_395 db 4, OG.WB, 0F7h, OG.B7, OG.IW ; o16 wF7 /7 iw
        opcode_TEST_GenInstr_396 db 4, OG.WB, 0F7h, OG.B7, OG.IW ; o16 wF7 /7 iw
;;;;;        opcode_TEST_GenInstr_397 db 3, OG.WB, 0A8h, OG.IB, *********************************** 
        opcode_TEST_GenInstr_397 db 0, 0  

;;;;;        opcode_TEST_GenInstr_398 db 3, OG.WB, 0A9h, OG.IW, *********************************** 
        opcode_TEST_GenInstr_398 db 0, 0  

;;;;;        opcode_TEST_GenInstr_399 db 3, OG.WB, 0A9h, OG.IW, *********************************** 
        opcode_TEST_GenInstr_399 db 0, 0  

        opcode_WAIT_GenInstr_400 db 2, OG.WB, 09Bh ; w9B
        opcode_XCHG_GenInstr_401 db 3, OG.WB, 086h, OG.RM2 ; w86 /r2
        opcode_XCHG_GenInstr_402 db 3, OG.WB, 086h, OG.RM2 ; w86 /r2
        opcode_XCHG_GenInstr_403 db 3, OG.WB, 087h, OG.RM2 ; o16 w87 /r2
        opcode_XCHG_GenInstr_404 db 3, OG.WB, 087h, OG.RM2 ; o16 w87 /r2
        opcode_XCHG_GenInstr_405 db 3, OG.WB, 086h, OG.RM1 ; w86 /r1
        opcode_XCHG_GenInstr_406 db 3, OG.WB, 086h, OG.RM1 ; w86 /r1
        opcode_XCHG_GenInstr_407 db 3, OG.WB, 087h, OG.RM1 ; o16 w87 /r1
        opcode_XCHG_GenInstr_408 db 3, OG.WB, 087h, OG.RM1 ; o16 w87 /r1
;;;;;        opcode_XCHG_GenInstr_409 db 2, OG.PR, 090h, *********************************** 
        opcode_XCHG_GenInstr_409 db 0, 0  

;;;;;        opcode_XCHG_GenInstr_410 db 2, OG.PR, 090h, *********************************** 
        opcode_XCHG_GenInstr_410 db 0, 0  

        opcode_XLATB_GenInstr_411 db 2, OG.WB, 0D7h ; wD7
        opcode_XOR_GenInstr_412 db 3, OG.WB, 030h, OG.RM1 ; w30 /r1
        opcode_XOR_GenInstr_413 db 3, OG.WB, 030h, OG.RM1 ; w30 /r1
        opcode_XOR_GenInstr_414 db 3, OG.WB, 031h, OG.RM1 ; o16 w31 /r1
        opcode_XOR_GenInstr_415 db 3, OG.WB, 031h, OG.RM1 ; o16 w31 /r1
        opcode_XOR_GenInstr_416 db 3, OG.WB, 032h, OG.RM2 ; w32 /r2
        opcode_XOR_GenInstr_417 db 3, OG.WB, 032h, OG.RM2 ; w32 /r2
        opcode_XOR_GenInstr_418 db 3, OG.WB, 033h, OG.RM2 ; o16 w33 /r2
        opcode_XOR_GenInstr_419 db 3, OG.WB, 033h, OG.RM2 ; o16 w33 /r2
        opcode_XOR_GenInstr_420 db 4, OG.WB, 080h, OG.B6, OG.IB ; w80 /6 ib
        opcode_XOR_GenInstr_421 db 4, OG.WB, 080h, OG.B6, OG.IB ; w80 /6 ib
        opcode_XOR_GenInstr_422 db 4, OG.WB, 081h, OG.B6, OG.IW ; o16 w81 /6 iw
        opcode_XOR_GenInstr_423 db 4, OG.WB, 081h, OG.B6, OG.IW ; o16 w81 /6 iw
        opcode_XOR_GenInstr_424 db 4, OG.WB, 081h, OG.B6, OG.IW ; o16 w81 /6 iw
        opcode_XOR_GenInstr_425 db 4, OG.WB, 081h, OG.B6, OG.IW ; o16 w81 /6 iw
        opcode_XOR_GenInstr_426 db 4, OG.WB, 083h, OG.B6, OG.IB ; o16 w83 /6 ib
        opcode_XOR_GenInstr_427 db 4, OG.WB, 083h, OG.B6, OG.IB ; o16 w83 /6 ib
;;;;;        opcode_XOR_GenInstr_428 db 3, OG.WB, 034h, OG.IB, *********************************** 
        opcode_XOR_GenInstr_428 db 0, 0  

;;;;;        opcode_XOR_GenInstr_429 db 3, OG.WB, 035h, OG.IW, *********************************** 
        opcode_XOR_GenInstr_429 db 0, 0  

;;;;;        opcode_XOR_GenInstr_430 db 3, OG.WB, 035h, OG.IW, *********************************** 
        opcode_XOR_GenInstr_430 db 0, 0  

        opcode_DB_GenInstr_431 db 1, OG.IB ; ib
        opcode_DW_GenInstr_432 db 1, OG.IW ; iw
        opcode_DW_GenInstr_433 db 1, OG.IW ; iw
        opcode_DW_GenInstr_434 db 1, OG.IW ; iw
        opcode_ORG_GenInstr_435 db 0, 0  ; org
        opcode_ORG_GenInstr_436 db 0, 0  ; \n
        opcode_ORG_GenInstr_437 db 0, 0  ; \n
        opcode_BITS_GenInstr_438 db 0, 0  ; \n
        opcode_BITS_GenInstr_439 db 0, 0  ; \n
        opcode_BITS_GenInstr_440 db 0, 0  ; \n


