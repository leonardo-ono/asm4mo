shl al, 1
shl ah, 1
shl bl, 1
shl bh, 1
shl cl, 1
shl ch, 1
shl dl, 1
shl dh, 1
shl byte [bx], 1
shl byte [bx+si], 1h
shl byte [bx+si+12h], 1b
shl byte [bx+si+4142h], 1

shl al, cl
shl ah, cl
shl bl, cl
shl bh, cl
shl cl, cl
shl ch, cl
shl dl, cl
shl dh, cl
shl byte [bx], cl
shl byte [bx+di], cl
shl byte [bx+di+12h], cl
shl byte [bx+di+4142h], cl

shl ax, 1
shl bx, 1
shl cx, 1
shl dx, 1
shl si, 1
shl di, 1
shl bp, 1

shl word [bx], 1
shl word [bx+si], 1h
shl word [bx+si+12h], 1b
shl word [bx+si+4142h], 1

shl ax, cl
shl bx, cl
shl cx, cl
shl dx, cl
shl si, cl
shl di, cl
shl bp, cl

shl word [bx], cl
shl word [bx+di], cl
shl word [bx+di+12h], cl
shl word [bx+si+12h], cl

 
