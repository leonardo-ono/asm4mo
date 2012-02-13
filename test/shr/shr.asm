shr al, 1
shr ah, 1
shr bl, 1
shr bh, 1
shr cl, 1
shr ch, 1
shr dl, 1
shr dh, 1
shr byte [bx], 1
shr byte [bx+si], 1h
shr byte [bx+si+12h], 1b
shr byte [bx+si+4142h], 1

shr al, cl
shr ah, cl
shr bl, cl
shr bh, cl
shr cl, cl
shr ch, cl
shr dl, cl
shr dh, cl
shr byte [bx], cl
shr byte [bx+di], cl
shr byte [bx+di+12h], cl
shr byte [bx+di+4142h], cl

shr ax, 1
shr bx, 1
shr cx, 1
shr dx, 1
shr si, 1
shr di, 1
shr bp, 1

shr word [bx], 1
shr word [bx+si], 1h
shr word [bx+si+12h], 1b
shr word [bx+si+4142h], 1

shr ax, cl
shr bx, cl
shr cx, cl
shr dx, cl
shr si, cl
shr di, cl
shr bp, cl

shr word [bx], cl
shr word [bx+di], cl
shr word [bx+di+12h], cl
shr word [bx+si+12h], cl

 
