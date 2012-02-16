; CMP r/m8,reg8                 ; 38 /r                [8086]
; CMP r/m16,reg16               ; 39 /r                [8086]
; CMP reg8,r/m8                 ; 3A /r                [8086]
; CMP reg16,r/m16               ; 3B /r                [8086]
; CMP r/m8,imm8                 ; 80 /0 ib             [8086]
; CMP r/m16,imm16               ; 81 /7 iw             [8086]
; CMP r/m16,imm8                ; 83 /7 ib             [8086]
; CMP AL,imm8                   ; 3C ib                [8086]
; CMP AX,imm16                  ; 3D iw                [8086]

; CMP r/m8,reg8                 ; 38 /r                [8086]

;------------------------

cmp al, al
cmp al, ah

cmp al, bl
cmp al, bh

cmp al, cl
cmp al, ch

cmp al, dl
cmp al, dh

cmp ah, al
cmp ah, ah

cmp ah, bl
cmp ah, bh

cmp ah, cl
cmp ah, ch

cmp ah, dl
cmp ah, dh

;------------------------

cmp bl, al
cmp bl, ah

cmp bl, bl
cmp bl, bh

cmp bl, cl
cmp bl, ch

cmp bl, dl
cmp bl, dh

cmp bh, al
cmp bh, ah

cmp bh, bl
cmp bh, bh

cmp bh, cl
cmp bh, ch

cmp bh, dl
cmp bh, dh

;------------------------

cmp cl, al
cmp cl, ah

cmp cl, bl
cmp cl, bh

cmp cl, cl
cmp cl, ch

cmp cl, dl
cmp cl, dh

cmp ch, al
cmp ch, ah

cmp ch, bl
cmp ch, bh

cmp ch, cl
cmp ch, ch

cmp ch, dl
cmp ch, dh

;------------------------

cmp dl, al
cmp dl, ah

cmp dl, bl
cmp dl, bh

cmp dl, cl
cmp dl, ch

cmp dl, dl
cmp dl, dh

cmp dh, al
cmp dh, ah

cmp dh, bl
cmp dh, bh

cmp dh, cl
cmp dh, ch

cmp dh, dl
cmp dh, dh

;------------------------

cmp [bx], al
cmp [bx], ah

cmp [bx], bl
cmp [bx], bh

cmp [bx], cl
cmp [bx], ch

cmp [bx], dl
cmp [bx], dh


