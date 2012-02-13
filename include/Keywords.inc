;===============================================================================
; class Keywords
;===============================================================================

	; includes -> needs "../../include/InstructionsHandler.inc"
	; ----------------------------------------------------------------------

	; public static constants
	; ----------------------------------------------------------------------
	Keywords.INSTR_LENGTH	equ 11

	; public static properties
	; ----------------------------------------------------------------------
	Keywords.list:

		;  Name                  ID              TYPE
		;--------------------------------------------

		; Assembly 8086 instructions
		db "AAA", 0, 0, 0, 0,     1, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "AAS", 0, 0, 0, 0,     2, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "AAD", 0, 0, 0, 0,     3, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "AAM", 0, 0, 0, 0,     4, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "ADC", 0, 0, 0, 0,     5, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "ADD", 0, 0, 0, 0,     6, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "AND", 0, 0, 0, 0,     7, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "CALL", 0, 0, 0,       8, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "CBW", 0, 0, 0, 0,     9, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "CWD", 0, 0, 0, 0,    10, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "CLC", 0, 0, 0, 0,    11, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "CLD", 0, 0, 0, 0,    12, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "CLI", 0, 0, 0, 0,    13, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "CMC", 0, 0, 0, 0,    14, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "CMP", 0, 0, 0, 0,    15, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "CMPSB", 0, 0,        16, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "CMPSW", 0, 0,        17, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "DAA", 0, 0, 0, 0,    18, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "DAS", 0, 0, 0, 0,    19, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "DEC", 0, 0, 0, 0,    20, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "DIV", 0, 0, 0, 0,    21, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "HLT", 0, 0, 0, 0,    22, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "IDIV", 0, 0, 0,      23, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "IMUL", 0, 0, 0,      24, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "IN", 0, 0, 0, 0, 0,  25, Token.TYPE_INSTR
		dw IN_handler
		db "INC", 0, 0, 0, 0,    26, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "INT", 0, 0, 0, 0,    27, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "INT3", 0, 0, 0,      28, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "INTO", 0, 0, 0,      29, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "IRET", 0, 0, 0,      30, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "IRETW", 0, 0,        31, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JCXZ", 0, 0, 0,      32, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JMP", 0, 0, 0, 0,    33, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JO", 0, 0, 0, 0, 0,  34, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNO", 0, 0, 0, 0,    35, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JB", 0, 0, 0, 0, 0,  36, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JC", 0, 0, 0, 0, 0,  37, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNAE", 0, 0, 0,      38, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JAE", 0, 0, 0, 0,    39, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNB", 0, 0, 0, 0,    40, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNC", 0, 0, 0, 0,    41, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JE", 0, 0, 0, 0, 0,  42, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JZ", 0, 0, 0, 0, 0,  43, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNE", 0, 0, 0, 0,    44, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNZ", 0, 0, 0, 0,    45, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JBE", 0, 0, 0, 0,    46, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNA", 0, 0, 0, 0,    47, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JA", 0, 0, 0, 0, 0,  48, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNBE", 0, 0, 0,      49, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JS", 0, 0, 0, 0, 0,  50, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNS", 0, 0, 0, 0,    51, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JP", 0, 0, 0, 0, 0,  52, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JPE", 0, 0, 0, 0,    53, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNP", 0, 0, 0, 0,    54, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JPO", 0, 0, 0, 0,    55, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JL", 0, 0, 0, 0, 0,  56, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNGE", 0, 0, 0,      57, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JGE", 0, 0, 0, 0,    58, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNL", 0, 0, 0, 0,    59, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JLE", 0, 0, 0, 0,    60, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNG", 0, 0, 0, 0,    61, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JG", 0, 0, 0, 0, 0,  62, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "JNLE", 0, 0, 0,      63, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "LAHF", 0, 0, 0,      64, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "LDS", 0, 0, 0, 0,    65, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "LES", 0, 0, 0, 0,    66, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "LEA", 0, 0, 0, 0,    67, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "LODSB", 0, 0,        68, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "LODSW", 0, 0,        69, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "LOOP", 0, 0, 0,      70, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "LOOPE", 0, 0,        71, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "LOOPZ", 0, 0,        72, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "LOOPNE", 0,          73, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "LOOPNZ", 0,          74, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "MOV", 0, 0, 0, 0,    75, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "MOVSB", 0, 0,        76, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "MOVSW", 0, 0,        77, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "MUL", 0, 0, 0, 0,    78, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "NEG", 0, 0, 0, 0,    79, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "NOT", 0, 0, 0, 0,    80, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "NOP", 0, 0, 0, 0,    81, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "OR", 0, 0, 0, 0, 0,  82, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "OUT", 0, 0, 0, 0,    83, Token.TYPE_INSTR
		dw OUT_handler
		db "POP", 0, 0, 0, 0,    84, Token.TYPE_INSTR
		dw POP_handler
		db "PUSH", 0, 0, 0,      85, Token.TYPE_INSTR
		dw PUSH_handler
		db "RCL", 0, 0, 0, 0,    86, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "RCR", 0, 0, 0, 0,    88, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "RET", 0, 0, 0, 0,    89, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "RETF", 0, 0, 0,      90, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "RETN", 0, 0, 0,      91, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "ROL", 0, 0, 0, 0,    92, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "ROR", 0, 0, 0, 0,    93, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "SAHF", 0, 0, 0,      94, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "SAL", 0, 0, 0, 0,    95, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "SAR", 0, 0, 0, 0,    96, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "SBB", 0, 0, 0, 0,    97, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "SCASB", 0, 0,        98, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "SCASW", 0, 0,        99, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "SHL", 0, 0, 0, 0,   100, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "SHR", 0, 0, 0, 0,   101, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "STC", 0, 0, 0, 0,   102, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "STD", 0, 0, 0, 0,   103, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "STI", 0, 0, 0, 0,   104, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "STOSB", 0, 0,       105, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "STOSW", 0, 0,       106, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "SUB", 0, 0, 0, 0,   107, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "TEST", 0, 0, 0,     108, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "WAIT", 0, 0, 0,     109, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "XCHG", 0, 0, 0,     110, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "XLATB", 0, 0,       111, Token.TYPE_INSTR
		dw DEFAULT_handler
		db "XOR", 0, 0, 0, 0,   112, Token.TYPE_INSTR
		dw DEFAULT_handler

		; BYTE and WORD reserved 
		db "BYTE", 0, 0, 0,     130, Token.TYPE_RESERVED_BYTE
		dw DEFAULT_handler
		db "WORD", 0, 0, 0,     131, Token.TYPE_RESERVED_WORD
		dw DEFAULT_handler

		; SHORT and FAR reserved 
		db "SHORT", 0, 0,       132, Token.TYPE_RESERVED_SHORT
		dw DEFAULT_handler
		db "FAR", 0, 0, 0, 0,   133, Token.TYPE_RESERVED_FAR
		dw DEFAULT_handler

		; Assembler instructions
		db "DB", 0, 0, 0, 0, 0, 113, Token.TYPE_INSTR
		dw DB_handler

		db "DW", 0, 0, 0, 0, 0, 114, Token.TYPE_INSTR
		dw DW_handler

		db "ORG", 0, 0, 0, 0,   115, Token.TYPE_INSTR
		dw ORG_handler

		db "BITS", 0, 0, 0,     116, Token.TYPE_INSTR
		dw DEFAULT_handler

		db "EQU", 0, 0, 0, 0,   117, Token.TYPE_INSTR
		dw DEFAULT_handler

		ID_AL equ 150
		ID_CL equ 151
		ID_AX equ 158
		ID_DX equ 160
		ID_ES equ 166 
		ID_CS equ 167
		ID_SS equ 168
		ID_DS equ 169

		;                                                        0 1 2 3 4 5 6 7
		;                                                       / 
		; Registers                                  index  __---___r/m
		db "AL", 0, 0, 0, 0, 0, ID_AL, Token.TYPE_REG8,    0, 11000000b
		db "CL", 0, 0, 0, 0, 0, ID_CL, Token.TYPE_REG8,    1, 11000001b
		db "DL", 0, 0, 0, 0, 0,   152, Token.TYPE_REG8,    2, 11000010b
		db "BL", 0, 0, 0, 0, 0,   153, Token.TYPE_REG8,    3, 11000011b
		db "AH", 0, 0, 0, 0, 0,   154, Token.TYPE_REG8,    4, 11000100b
		db "CH", 0, 0, 0, 0, 0,   155, Token.TYPE_REG8,    5, 11000101b
		db "DH", 0, 0, 0, 0, 0,   156, Token.TYPE_REG8,    6, 11000110b
		db "BH", 0, 0, 0, 0, 0,   157, Token.TYPE_REG8,    7, 11000111b
		db "AX", 0, 0, 0, 0, 0, ID_AX, Token.TYPE_REG16,   0, 11000000b
		db "CX", 0, 0, 0, 0, 0,   159, Token.TYPE_REG16,   1, 11000001b
		db "DX", 0, 0, 0, 0, 0, ID_DX, Token.TYPE_REG16,   2, 11000010b
		db "BX", 0, 0, 0, 0, 0,   161, Token.TYPE_REG16,   3, 11000011b
		db "SP", 0, 0, 0, 0, 0,   162, Token.TYPE_REG16,   4, 11000100b
		db "BP", 0, 0, 0, 0, 0,   163, Token.TYPE_REG16,   5, 11000101b
		db "SI", 0, 0, 0, 0, 0,   164, Token.TYPE_REG16,   6, 11000110b
		db "DI", 0, 0, 0, 0, 0,   165, Token.TYPE_REG16,   7, 11000111b
		db "ES", 0, 0, 0, 0, 0, ID_ES, Token.TYPE_REGSEG,  0, 00000000b
		db "CS", 0, 0, 0, 0, 0, ID_CS, Token.TYPE_REGSEG,  1, 00000000b
		db "SS", 0, 0, 0, 0, 0, ID_SS, Token.TYPE_REGSEG,  2, 00000000b
		db "DS", 0, 0, 0, 0, 0, ID_DS, Token.TYPE_REGSEG,  3, 00000000b

		db ":", 0, 0, 0, 0, 0, 0, 201, Token.TYPE_DOIS_PONTOS,    0, 0
		db ",", 0, 0, 0, 0, 0, 0, 202, Token.TYPE_VIRGULA,        0, 0
		db "+", 0, 0, 0, 0, 0, 0, 203, Token.TYPE_MAIS,           0, 0
		db "[", 0, 0, 0, 0, 0, 0, 204, Token.TYPE_ABRE_COLCHETE,  0, 0
		db "]", 0, 0, 0, 0, 0, 0, 205, Token.TYPE_FECHA_COLCHETE, 0, 0
		db '"', 0, 0, 0, 0, 0, 0, 206, Token.TYPE_ASPAS,          0, 0
		db "'", 0, 0, 0, 0, 0, 0, 207, Token.TYPE_APOSTROFE,      0, 0
		db 10,  0, 0, 0, 0, 0, 0, 208, Token.TYPE_END_OF_LINE,    0, 0
		db 13,  0, 0, 0, 0, 0, 0, 208, Token.TYPE_END_OF_LINE,    0, 0

					       Token.ID_IMM8	equ 220
					       Token.ID_IMM16	equ 221
					       Token.ID_MEM	equ 222
					       Token.ID_STRING	equ 223

		db 0 ; end of keywords list

;===============================================================================


