; CMP r/m8,reg8                 ; 38 /r                [8086]
; CMP r/m16,reg16               ; 39 /r                [8086]
; CMP reg8,r/m8                 ; 3A /r                [8086]
; CMP reg16,r/m16               ; 3B /r                [8086]
; CMP r/m8,imm8                 ; 80 /0 ib             [8086]
; CMP r/m16,imm16               ; 81 /7 iw             [8086]
; CMP r/m16,imm8                ; 83 /7 ib             [8086]
; CMP AL,imm8                   ; 3C ib                [8086]
; CMP AX,imm16                  ; 3D iw                [8086]

; CMP r/m16,reg16               ; 39 /r                [8086]

;------------------------

cmp ax, ax
cmp ax, bx
cmp ax, cx
cmp ax, dx
cmp ax, si
cmp ax, di
cmp ax, bp

;------------------------

cmp bx, ax
cmp bx, bx
cmp bx, cx
cmp bx, dx
cmp bx, si
cmp bx, di
cmp bx, bp

;------------------------

cmp cx, ax
cmp cx, bx
cmp cx, cx
cmp cx, dx
cmp cx, si
cmp cx, di
cmp cx, bp

;------------------------

cmp dx, ax
cmp dx, bx
cmp dx, cx
cmp dx, dx
cmp dx, si
cmp dx, di
cmp dx, bp

;------------------------

cmp [bx], ax
cmp [bx], bx
cmp [bx], cx
cmp [bx], dx
cmp [bx], si
cmp [bx], di
cmp [bx], bp


