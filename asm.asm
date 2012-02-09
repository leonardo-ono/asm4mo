;===============================================================================
; class Main
;===============================================================================
	bits	16
	org	32768

	; entry point
	; ----------------------------------------------------------------------
	jmp	Main.main

	; private properties
	; ----------------------------------------------------------------------
	Main.fileName			db "________.___", 0
	Main.defaultOutputFileName	db "OUT.BIN", 0
	Main.isFirstToken		db 1
	Main.line			dw 0

	; public methods
	; ----------------------------------------------------------------------

		Main.main: ; (entry point)
		; --------------------------------------------------------------

				mov	al, ' '
				cmp	si, 0			; Were we passed a filename?
				; je	.no_param_passed
				call	os_string_tokenize	; If so, get it from params

				; call	os_print_string
				; call	os_print_newline
				; ret

				push	di
				mov	di, Main.fileName	; Save file for later usage
				call	os_string_copy

				pop	di
				cmp	di, 0
				je	.noSecondArgument

				mov	si, di			; second parameter=output file
				call	os_string_tokenize		
				mov	di, Main.outputFileName		
				call	os_string_copy

				jmp	.startAssembler

			.noSecondArgument:

				mov	si, Main.defaultOutputFileName
				mov	di, Main.outputFileName
				call	os_string_copy

			.startAssembler:
				; ------------------------------------- ; Copied from edit.asm

				mov	si, Main.fileName
				call	parser.setFileName
				call	parser.start
				cmp	ah, 1
				je	.fileLoaded
			.fileNotFound:
				mov	si, Main.msg.error.fileNotFound
				call	os_print_string
				call	os_print_newline
				ret
			.fileLoaded:
				mov	si, Main.msg.assembling
				call	os_print_string
				mov	si, Main.fileName
				call	os_print_string
				mov	si, Main.msg.assembling2
				call	os_print_string
				call	os_print_newline
			.newLine:

				; call	os_wait_for_key ; <----------------- wait for key to process next line

				inc	word [Main.line]
				mov	byte [Main.isFirstToken], 1
				call	Mem.clear
				call	OpcodeGenerator.clearIbIwRMStacks
				call	InstructionHandler.clear
				call	os_print_newline
				call	Main.printLineNumber

			.getNextToken:
				call	parser.getNextToken
			.isEndOfFile:
				cmp	ah, 0 ; parser.endofline is bugged ?
				je	.end
				mov	al, [token.value]
				cmp	al, 0
				je	.end
			.notIsEndOfLine:

			.verifyFirstToken:
				; first token of line can be label or instruction
				mov	al, [Main.isFirstToken]
				cmp	al, 1
				jne	.notIsFirstToken
				call	Main.handleFirstToken
				cmp	ah, 2 ; endofline
				je	Main.main.newLine
				cmp	ah, 1
				je	.firstTokenOk
			.expectedLabelOrInstr:
				mov	si, Main.msg.error.expectedLabelOrInstr
				call	os_print_string
				call	os_print_newline
				ret

			.firstTokenOk:
			.notIsFirstToken:

				; second token must be a instruction
				mov	al, [Main.isFirstToken]
				cmp	al, 2
				jne	.mustNotBeInstruction
				call	Main.handleInstruction
				cmp	ah, 2 ; endofline
				je	Main.main.newLine
				cmp	ah, 1
				jne	.notIsInstruction
			.isInstruction:

				; call the official instruction handler
				; if ah=0 there isn't implementation, so will use default handler
				mov	ah, 0

				call	[token.handler] 

				cmp	ah, 1
				je	.newLine
				cmp	ah, 2 ; error !
				je	.errorInstructionHandler

				jmp	.getNextToken

			.errorInstructionHandler:
				mov	si, Main.msg.error.instructionHandler
				call	os_print_string
				call	os_print_newline
				ret

			.notIsInstruction:
				mov	si, Main.msg.error.expectedInstr
				call	os_print_string
				call	os_print_newline
				ret

			.mustNotBeInstruction:
				; is end of line ?
				mov	al, [token.type]
				cmp	al, Token.TYPE_END_OF_LINE
				je	.searchInstruction

				call	Main.printTokenInfo ; <-- debug: show token info
				
			;-------------------------------------------------------
				call	Main.handleReg
				cmp	ah, 1
				jne	.notIsReg
			.isReg:
				jmp	.getNextToken
			.notIsReg:

			;-------------------------------------------------------
				call	Main.handleImm
				cmp	ah, 1
				jne	.notIsImm
			.isImm:
				jmp	.getNextToken
			.notIsImm:

			;-------------------------------------------------------
				call	Main.handleMem
				cmp	ah, 1
				jne	.notIsMem
			.isMem:
				jmp	.getNextToken
			.notIsMem:

			;-------------------------------------------------------
				call	Main.handleLabel
				cmp	ah, 1
				jne	.notIsLabel
			.isLabel:
				jmp	.getNextToken
			.notIsLabel:

			;-------------------------------------------------------
				call	Main.handleSingleQuotes
				cmp	ah, 1
				jne	.notIsSingleQuotes
			.isSingleQuotes:
				jmp	.getNextToken
			.notIsSingleQuotes:


			;-------------------------------------------------------
				mov	al, [token.type]
				call	InstructionHandler.addTokenType
				jmp	.getNextToken
			.searchInstruction:
				mov	si, Main.msg.serchingInstruction
				call	os_print_string
				call	os_print_newline
				call	InstructionHandler.classify
				cmp	ah, 1
				jne	.notIsValidInstruction
			.isValidInstruction:
				mov	si, Main.msg.serchingInstruction.isValid
				call	os_print_string
				call	os_print_newline
				jmp	short .generateOpcode
			.notIsValidInstruction:
				mov	si, Main.msg.serchingInstruction.notIsValid
				call	os_print_string
				call	os_print_newline
				ret
			.generateOpcode:
				mov	si, Main.msg.generationOpcode
				call	os_print_string
				call	os_print_newline

				mov	ax, [InstructionHandler.opcodeGenerationPointer]
				call	OpcodeGenerator.execute
				cmp	bl, 0
				je	.notImplementedYet
				jmp	.newLine
			.notImplementedYet:
				mov	si, Main.msg.error.notImplementedYet
				call	os_print_string
				call	os_print_newline
			.end:
			.success:
				call	os_print_newline
				;call	LabelHandler.list
				mov	ax, [OpcodeGenerator.startMemoryAddress]
				call	LabelResolver.resolve
				call	os_print_newline
				; call	LabelResolver.list

				call	Main.writeFile

				call	os_print_newline
				mov	si, Main.msg.assemblerSucess
				call	os_print_string
				call	os_print_newline

				ret
			Main.msg.here				db "---> here <---", 0
			Main.msg.assembling			db "Assembling ", 0
			Main.msg.assembling2			db " ...", 0
			Main.msg.invokingHandler		db "Invoking handler ...",0
			Main.msg.serchingInstruction		db "   --> Verifying arguments ...", 0
			Main.msg.serchingInstruction.isValid	db "       Arguments ok.", 0
			Main.msg.serchingInstruction.notIsValid	db "       Invalid arguments !", 0
			Main.msg.generationOpcode		db "       Generating opcode ...", 0

			Main.msg.error.fileNotFound		db "File not found !", 0
			Main.msg.error.notImplementedYet	db "Instruction not implemented yet !", 0
			Main.msg.error.expectedLabelOrInstr	db "Expected label or instruction !", 0
			Main.msg.error.expectedInstr		db "Expected instruction !", 0
			Main.msg.error.instructionHandler	db "Error instruction handler !", 0
			Main.msg.assemblerSucess		db "Success.", 0


	; private methods
	; ----------------------------------------------------------------------
		
		Main.handleFirstToken:
		; --------------------------------------------------------------
		; ah = 0 - Expected label or instruction ! (end of line is ignored)
		;      1 - ok
		;      2 - end of line
				mov	al, [token.type]
				cmp	al, Token.TYPE_END_OF_LINE
				je	.endOfLine
				cmp	al, Token.TYPE_LABEL
				je	.isLabel
				jmp	.notIsLabel
			.isLabel:
				mov	si, Main.msg.handlingLabel
				call	os_print_string
				mov	si, token.value
				call	os_print_string
				call	os_print_newline

				; ax = pointer to label string
				; dx = offset
				mov	ax, token.value
				mov	dx, [OpcodeGenerator.index]
				call	LabelHandler.add

				call	parser.getNextToken
				mov	al, [token.type]
				cmp	al, Token.TYPE_DOIS_PONTOS
				jne	.notIsDoisPontos
			.isDoisPontos:
				call	parser.getNextToken
				mov	ah, 1
				jmp	short .ret
			.notIsLabel:
				; at least is instruction ?
				cmp	al, Token.TYPE_INSTR
				je	.isInstruction
			.notIsInstruction:
				mov	ah, 0
				jmp	short .ret
			.isInstruction:
			.notIsDoisPontos:
				mov	ah, 1
				jmp	short .ret
			.ret:
				mov	byte [Main.isFirstToken], 2
				ret
			.endOfLine:
				mov	ah, 2
				ret
			Main.msg.handlingLabel	db "   Handling label: ", 0
	

		Main.handleInstruction:
		; --------------------------------------------------------------
		; ah = 0 - not is instruction
		;      1 - success
		;      2 - end of line
				mov	al, [token.type]
				cmp	al, Token.TYPE_END_OF_LINE
				je	.endOfLine
				cmp	al, Token.TYPE_INSTR
				je	.isInstruction
			.notIsInstruction:
				mov	ah, 0
				ret
			.isInstruction:
				mov	si, Main.msg.handlingInstruction
				call	os_print_string
				mov	si, token.value
				call	os_print_string
				call	os_print_newline
				mov	al, [token.id]
				call	InstructionHandler.addTokenType	; if instruction, token type = id
				mov	byte [Main.isFirstToken], 3
				mov	ah, 1
				ret
			.endOfLine:
				mov	ah, 2
				ret
			Main.msg.handlingInstruction	db "   Handling instruction: ", 0


		Main.handleReg:
		; --------------------------------------------------------------
		; ah = 0 - not is reg
		;      1 - success
				mov	al, [token.type]
				cmp	al, Token.TYPE_REG8
				je	.isReg
				cmp	al, Token.TYPE_REG16		
				je	.isReg
				cmp	al, Token.TYPE_REGSEG
				je	.isReg
			.notIsReg:
				mov	ah, 0
				ret
			.isReg:
				mov	al, [token.type]
				call	InstructionHandler.addTokenType

				;   al = Id
				;   ah = Type
				;   cl = RegIndex
				;   ch = R/M
				;   dl = has extra imm8
				;   dh = extra imm8
				;   bh = has extra imm16
				;   si = extra imm16
				mov	al, [token.id]
				mov	ah, [token.type]
				mov	cl, [token.handler] ; for registers handler contains index  
				mov	ch, [token.handler+1] ; for registers handler+1 contains r/m
				mov	dl, 0
				mov	dh, 0
				mov	bh, 0
				mov	si, 0
				call	OpcodeGenerator.pushRM
				mov	ah, 1
				ret


		Main.handleImm:
		; --------------------------------------------------------------
		; ah = 0 - not is imm
		;      1 - success
				mov	al, [token.type]
				cmp	al, Token.TYPE_IMM8
				je	.isImm
				cmp	al, Token.TYPE_IMM16
				je	.isImm
			.notIsImm:
				mov	ah, 0
				ret
			.isImm:

			;---------------------------------------------
			.verifyisTokenImm8Or16:
				cmp	al, Token.TYPE_IMM8
				je	.addImm8ToMem
				cmp	al, Token.TYPE_IMM16
				je	.addImm16ToMem
				jmp	short .notIsImm
			.addImm8ToMem:
				mov	si, Main.msg.addImm8ToMem
				call	os_print_string
				call	os_print_newline
				mov	ax, [token.iw]
				call	OpcodeGenerator.pushIb
				jmp	short .success
			.addImm16ToMem:
				mov	si, Main.msg.addImm16ToMem
				call	os_print_string
				call	os_print_newline
				mov	ax, [token.iw]
				mov	cl, 0 ; not is label
				call	OpcodeGenerator.pushIw
				jmp	short .success
			.success:
				mov	al, [token.type]
				call	InstructionHandler.addTokenType
				mov	ah, 1
				ret


		Main.handleMem:
		; --------------------------------------------------------------
		; ah = 0 - not is mem
		;      1 - is mem
		;      2 - mem is not valid !
				mov	al, [token.value]
				cmp	al, '['
				je	.isMem
			.notIsMem:
				mov	ah, 0
				ret
			.isMem:
				mov	si, Main.msg.handlingMem
				call	os_print_string
				call	os_print_newline
			.nextToken:
				call	parser.getNextToken

			.printToken:
				mov	si, Main.msg.3spaces
				call	os_print_string
				call	Main.printTokenInfo

				mov	al, [token.value]
				cmp	al, ']'
				je	.end

			;---------------------------------------------
			.verifyIsTokenLabel:
				mov	al, [token.type]
				cmp	al, Token.TYPE_LABEL
				jne	.notIsLabel
			.isLabel:
				mov	si, .verifyIsTokenLabel.msg.handlingLabel
				call	os_print_string
				call	os_print_newline

				mov	ax, 0
				mov	word [Mem.iw], ax
				mov	word [Mem.iwIsLabel], 1
				mov	byte [Mem.isIbOrIw], 2

				mov	ax, 0
				mov	cl, 1 ; is label
				mov	dx, token.value ; label name
				call	OpcodeGenerator.pushIw

				mov	al, Token.ID_IMM16
				call	Mem.addTokenId

				jmp	.nextToken
			.verifyIsTokenLabel.msg.handlingLabel	db "   Handling label: ", 0

			.notIsLabel:
			.verifyIsTokenLabel.end:
			;---------------------------------------------

				mov	al, [token.id]
				call	Mem.addTokenId

			;---------------------------------------------
			.verifyisTokenImm8Or16:
				cmp	al, Token.ID_IMM8
				je	.addImm8ToMem
				cmp	al, Token.ID_IMM16
				je	.addImm16ToMem
				jmp	short .endOfVerifyisTokenImm8Or16
			.addImm8ToMem:
				mov	si, Main.msg.addImm8ToMem
				call	os_print_string
				call	os_print_newline
				mov	ax, [token.iw]
				mov	byte [Mem.ib], al
				mov	byte [Mem.isIbOrIw], 1
				jmp	short .endOfVerifyisTokenImm8Or16
			.addImm16ToMem:
				mov	si, Main.msg.addImm16ToMem
				call	os_print_string
				call	os_print_newline
				mov	ax, [token.iw]
				mov	word [Mem.iw], ax
				mov	word [Mem.iwIsLabel], 0
				mov	byte [Mem.isIbOrIw], 2

				jmp	short .endOfVerifyisTokenImm8Or16
			;---------------------------------------------
			.endOfVerifyisTokenImm8Or16:
				jmp	.nextToken
			.end:
				call	Mem.classify
				cmp	ah, 1
				je	.memIsValid
			.memIsNotValid:
				mov	si, Main.msg.mem.classify.notIsValid
				call	os_print_string
				call	os_print_newline
				mov	ah, 2
				ret
			.memIsValid:
				mov	si, Main.msg.mem.classify.isValid
				call	os_print_string
				call	os_print_newline

				;   al = Id
				;   ah = Type
				;   cl = RegIndex
				;   ch = R/M
				;   dl = has extra imm8
				;   dh = extra imm8
				;   bh = has extra imm16
				;   si = extra imm16
				;   di = extra imm16 is label ? 1=true 0=false
				mov	al, [token.id]
				mov	ah, [token.type]
				mov	cl, 0
				mov	ch, [Mem.rm]
				mov	dl, 0
				mov	dh, 0
				mov	bh, 0
				mov	si, 0
				mov	di, [Mem.iwIsLabel]
			.verifyHasExtraByteOrWord:
				mov	bl, [Mem.isIbOrIw]
				cmp	bl, 1
				je	.hasExtraByte
				cmp	bl, 2
				je	.hasExtraWord
				cmp	bl, 3          ; must write word, but the original imm value is byte (imm8)
				je	.hasExtraWordGetByte
				jmp	short .notHasExtraByteOrWord
			.hasExtraByte:
				mov	dl, 1
				mov	dh, [Mem.ib]
				jmp	short .pushRM
			.hasExtraWord:
				mov	bh, 1
				mov	si, [Mem.iw]
				jmp	short .pushRM
			.hasExtraWordGetByte: ; for mov [1], cl
				mov	bh, 1
				mov	dh, 0
				mov	dl, [Mem.ib]
				mov	si, dx
				mov	dx, 0
				jmp	short .pushRM
			.notHasExtraByteOrWord:
			.pushRM:
				call	OpcodeGenerator.pushRM
				mov	al, Token.TYPE_MEM
				call	InstructionHandler.addTokenType
				mov	ah, 1
				ret
			Main.msg.3spaces			db "   ",0
			Main.msg.handlingMem			db "   Handling mem: ",0
			Main.msg.mem.classify.isValid		db "   Mem format ok ! ", 0
			Main.msg.mem.classify.notIsValid	db "   Mem format not valid ! ", 0
			Main.msg.addImm8ToMem			db "   Adding imm8 to mem.", 0
			Main.msg.addImm16ToMem			db "   Adding imm16 to mem.", 0


		Main.handleLabel:
		; --------------------------------------------------------------
		; ah = 0 - not is label
		;      1 - success
				mov	al, [token.type]
				cmp	al, Token.TYPE_LABEL
				je	.isLabel
			.notIsLabel:
				mov	ah, 0
				ret
			.isLabel:
				mov	si, Main.msg.handlingLabel2
				call	os_print_string
				mov	si, token.value
				call	os_print_string
				call	os_print_newline

				; // TODO label token value needs to save too ?
				mov	ax, 0
				mov	cl, 1 ; is label
				mov	dx, token.value ; label name
				call	OpcodeGenerator.pushIw

				mov	al, Token.TYPE_IMM16
				call	InstructionHandler.addTokenType

				mov	ah, 1
				ret
			Main.msg.handlingLabel2	db "   Handling label: ", 0


		Main.handleSingleQuotes:
		; --------------------------------------------------------------
		; ah = 0 - not is single quotes
		;      1 - success
				mov	al, [token.type]
				cmp	al, Token.TYPE_STRING
				je	.isString
			.notIsSingleQuotes:
				mov	ah, 0
				ret
			.isString:
				mov	ax, token.value
				call	os_string_length
				cmp	ax, 3
				jg	.notIsSingleQuotes
			.isSingleQuotes:
				mov	si, Main.msg.handlingSingleQuotes
				call	os_print_string
				mov	si, token.value
				call	os_print_string
				call	os_print_newline

				mov	al, [token.value+1] ; byte value
				call	OpcodeGenerator.pushIb

				mov	al, Token.TYPE_IMM8
				call	InstructionHandler.addTokenType	
				mov	byte [Main.isFirstToken], 3
				mov	ah, 1
				ret
			Main.msg.handlingSingleQuotes	db "   Handling single quotes: ", 0






		Main.printTokenInfo:
		; --------------------------------------------------------------
			.printTokenValue:
				mov	si, Main.msg.tokenEqual
				call	os_print_string
				mov	si, token.value
				call	os_print_string
			.printTokenType:
				mov	si, Main.msg.tokenTypeEqual
				call	os_print_string
				mov	ah, 0
				mov	al, [token.type]
				call	os_int_to_string
				mov	si, ax
				call	os_print_string
			.printTokenId:
				mov	si, Main.msg.tokenIdEqual
				call	os_print_string
				mov	ah, 0
				mov	al, [token.id]
				call	os_int_to_string
				mov	si, ax
				call	os_print_string
				call	os_print_newline
				ret
			Main.msg.tokenEqual		db "Token=", 0
			Main.msg.tokenTypeEqual		db " / type=", 0
			Main.msg.tokenIdEqual		db " / id=", 0


		Main.printLineNumber:
		; --------------------------------------------------------------
				mov	si, Main.msg.lineNumberEqual
				call	os_print_string
				mov	ah, 0
				mov	al, [Main.line]
				call	os_int_to_string
				mov	si, ax
				call	os_print_string
				call	os_print_newline
				ret
			Main.msg.lineNumberEqual	db "Line=", 0


		Main.writeFile:
		; --------------------------------------------------------------
			mov	si, Main.msg.writingFile1
			call	os_print_string
			mov	si, Main.outputFileName
			call	os_print_string
			mov	si, Main.msg.writingFile2
			call	os_print_string
			call	os_print_newline

			mov ax, Main.outputFileName
			mov bx, [OpcodeGenerator.startMemoryAddress]
			mov cx, [OpcodeGenerator.index]
			call os_remove_file
			call os_write_file
			ret
		Main.outputFileName	db "________.___", 0
		Main.msg.writingFile1	db "Writing ",0
		Main.msg.writingFile2	db " file ...", 0


	; includes
	; ----------------------------------------------------------------------
	%include	"include/mikedev.inc"
	%include	"include/OpcodeGenerator.inc"
	%include	"include/InstructionsHandler.inc"
	%include	"include/FileInputStream.inc"
	%include	"include/Token.inc"
	%include	"include/Character.inc"
	%include	"include/Imm.inc"
	%include	"include/Parser.inc"
	%include	"include/Keywords.inc"
	%include	"include/Mem.inc"
	%include	"include/LabelResolver.inc"
	%include	"include/LabelHandler.inc"

;===============================================================================
