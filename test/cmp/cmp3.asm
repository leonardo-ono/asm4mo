; CMP r/m8,reg8                 ; 38 /r                [8086]
; CMP r/m16,reg16               ; 39 /r                [8086]
; CMP reg8,r/m8                 ; 3A /r                [8086]
; CMP reg16,r/m16               ; 3B /r                [8086]
; CMP r/m8,imm8                 ; 80 /0 ib             [8086]
; CMP r/m16,imm16               ; 81 /7 iw             [8086]
; CMP r/m16,imm8                ; 83 /7 ib             [8086]
; CMP AL,imm8                   ; 3C ib                [8086]
; CMP AX,imm16                  ; 3D iw                [8086]

; CMP reg8,r/m8                 ; 3A /r                [8086]

;------------------------

; CMP reg8,reg8 -> will generate opcode 38
; CMP reg8,mem8

cmp al, [bx]
cmp ah, [bx]
cmp bl, [bx]
cmp bh, [bx]
cmp cl, [bx]
cmp ch, [bx]
cmp dl, [bx]
cmp dh, [bx]


