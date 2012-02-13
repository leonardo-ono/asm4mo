; Sine Table
; Demo for asm4mo (https://github.com/leonardo-ono/asm4mo/)
; Written by Leonardo Ono (ono.leo@gmail.com)
; 13/02/2012 14:13
; Use: asm sin.asm sin.bin

   jmp short start

   varX  dw 0
   varX2 dw 0
   varY  dw 0
   varYS dw 0

start:
   mov al, 13h ; graphic mode
   call changeVideo

nextX:
   mov dx, 3
   mov ax, [varX]
   mul dx

   mov dx, [varX2]
   add ax, dx

   call sin ; cx=sin

   mov ax, cx
   mov cx, [varY]
   mul cx

   mov cx, 500
   mov dx, 500
   add cx, dx
   div cx ; mov cx, 1000 -> system crashes ...
   mov cx, ax

   mov ax, 100

   mov dx, 1
   cmp si, dx
   je msub

madd:
   add ax, cx
   jmp short cont

msub:
   sub ax, cx
   jmp short cont

cont:
   mov bx, [varX]
   mov cl, 15
   call pset

   mov ax, [varX]
   inc ax
   mov [varX], ax

   mov dx, 318 
   cmp bx, dx   ; cmp bx, 318 -> not working properly
   jle nextX    

   call 15h ; os_check_for_key
   cmp al, 27
   je end

   mov ax, 1
   call 24h ; os_pause

   ; call 12h; os_wait_or_key
   ; cmp al, 27
   ; je end

; next frame
   mov dx, [varX2]
   inc dx
   mov [varX2], dx
   mov word [varX], 0

   call clrscr

   mov si, [varYS]
   mov dx, 0
   cmp si, dx
   je varyadd

varysub:
   mov dx, [varY]
   dec dx
   mov [varY], dx
   mov bx, 0
   cmp dx, bx
   je varyzero
   jmp short varycont

varyadd:
   mov dx, [varY]
   inc dx
   mov [varY], dx
   mov bx, 60
   cmp dx, bx
   je vary60
   jmp short varycont

varyzero:
   mov word [varYS], 0
   jmp short varycont

vary60:
   mov word [varYS], 1
   jmp short varycont 

varycont:
   jmp nextX

end:  
   mov al, 3h ; text mode
   call changeVideo

   ret ; return to os



; al = mode
changeVideo:
   mov ah, 0
   int 10h
   ret

clrscr:
   push ax
   push bx
   push ds
   mov ax, 0a000h
   mov ds, ax
   mov bx, 0
csnp:
   mov byte [bx], 0
   cmp bx, 0ffffh
   je csnpend
   inc bx
   jmp short csnp
csnpend:         
   pop ds
   pop bx
   pop ax
   ret


; bx = x
; ax = y
; cl = color
pset:
   push ax
   push bx
   push cx
   push dx
   push ds
   mov dx, ax
   mov ax, 200
   sub ax, dx
   mov dx, 320
   mul dx
   add ax, bx
   mov bx, ax 
   mov dx, 0a000h
   mov ds, dx
   mov [bx], cl
   pop ds
   pop dx
   pop cx
   pop bx
   pop ax
   ret

; ax = angle (0-360)
; cx = sin * 100
; si = signal 0->+ 1->-
sin:
   push bx

   mov cx, 360
   div cx
   mov ax, dx ; ax = ax % 360 

   mov si, 0 ; signal

   mov dx, 180
   cmp ax, dx
   jle le180

g180:
   mov si, 1
   mov dx, 180
   sub ax, dx ; sub ax, 180 -> not working properly

le180:
   mov cx, 2
   mul cx
   mov bx, ax
   mov cx, [bx + sinTable]  
   jmp short sinRet

sinRet:
   pop bx
   ret
 
sinTable:
   ;  Deg  Sin
   dw 0    ; 0 
   dw 17   ; 1 
   dw 35   ; 2 
   dw 52   ; 3 
   dw 70   ; 4 
   dw 87   ; 5 
   dw 105  ; 6 
   dw 122  ; 7 
   dw 139  ; 8 
   dw 156  ; 9 
   dw 174  ; 10 
   dw 191  ; 11 
   dw 208  ; 12 
   dw 225  ; 13 
   dw 242  ; 14 
   dw 259  ; 15 
   dw 276  ; 16 
   dw 292  ; 17 
   dw 309  ; 18 
   dw 326  ; 19 
   dw 342  ; 20 
   dw 358  ; 21 
   dw 375  ; 22 
   dw 391  ; 23 
   dw 407  ; 24 
   dw 423  ; 25 
   dw 438  ; 26 
   dw 454  ; 27 
   dw 469  ; 28 
   dw 485  ; 29 
   dw 500  ; 30 
   dw 515  ; 31 
   dw 530  ; 32 
   dw 545  ; 33 
   dw 559  ; 34 
   dw 574  ; 35 
   dw 588  ; 36 
   dw 602  ; 37 
   dw 616  ; 38 
   dw 629  ; 39 
   dw 643  ; 40 
   dw 656  ; 41 
   dw 669  ; 42 
   dw 682  ; 43 
   dw 695  ; 44 
   dw 707  ; 45 
   dw 719  ; 46 
   dw 731  ; 47 
   dw 743  ; 48 
   dw 755  ; 49 
   dw 766  ; 50 
   dw 777  ; 51 
   dw 788  ; 52 
   dw 799  ; 53 
   dw 809  ; 54 
   dw 819  ; 55 
   dw 829  ; 56 
   dw 839  ; 57 
   dw 848  ; 58 
   dw 857  ; 59 
   dw 866  ; 60 
   dw 875  ; 61 
   dw 883  ; 62 
   dw 891  ; 63 
   dw 899  ; 64 
   dw 906  ; 65 
   dw 914  ; 66 
   dw 921  ; 67 
   dw 927  ; 68 
   dw 934  ; 69 
   dw 940  ; 70 
   dw 946  ; 71 
   dw 951  ; 72 
   dw 956  ; 73 
   dw 961  ; 74 
   dw 966  ; 75 
   dw 970  ; 76 
   dw 974  ; 77 
   dw 978  ; 78 
   dw 982  ; 79 
   dw 985  ; 80 
   dw 988  ; 81 
   dw 990  ; 82 
   dw 993  ; 83 
   dw 995  ; 84 
   dw 996  ; 85 
   dw 998  ; 86 
   dw 999  ; 87 
   dw 999  ; 88 
   dw 999  ; 89 
   dw 999  ; 90 
   dw 999  ; 91 
   dw 999  ; 92 
   dw 999  ; 93 
   dw 998  ; 94 
   dw 996  ; 95 
   dw 995  ; 96 
   dw 993  ; 97 
   dw 990  ; 98 
   dw 988  ; 99 
   dw 985  ; 100 
   dw 982  ; 101 
   dw 978  ; 102 
   dw 974  ; 103 
   dw 970  ; 104 
   dw 966  ; 105 
   dw 961  ; 106 
   dw 956  ; 107 
   dw 951  ; 108 
   dw 946  ; 109 
   dw 940  ; 110 
   dw 934  ; 111 
   dw 927  ; 112 
   dw 921  ; 113 
   dw 914  ; 114 
   dw 906  ; 115 
   dw 899  ; 116 
   dw 891  ; 117 
   dw 883  ; 118 
   dw 875  ; 119 
   dw 866  ; 120 
   dw 857  ; 121 
   dw 848  ; 122 
   dw 839  ; 123 
   dw 829  ; 124 
   dw 819  ; 125 
   dw 809  ; 126 
   dw 799  ; 127 
   dw 788  ; 128 
   dw 777  ; 129 
   dw 766  ; 130 
   dw 755  ; 131 
   dw 743  ; 132 
   dw 731  ; 133 
   dw 719  ; 134 
   dw 707  ; 135 
   dw 695  ; 136 
   dw 682  ; 137 
   dw 669  ; 138 
   dw 656  ; 139 
   dw 643  ; 140 
   dw 629  ; 141 
   dw 616  ; 142 
   dw 602  ; 143 
   dw 588  ; 144 
   dw 574  ; 145 
   dw 559  ; 146 
   dw 545  ; 147 
   dw 530  ; 148 
   dw 515  ; 149 
   dw 500  ; 150 
   dw 485  ; 151 
   dw 469  ; 152 
   dw 454  ; 153 
   dw 438  ; 154 
   dw 423  ; 155 
   dw 407  ; 156 
   dw 391  ; 157 
   dw 375  ; 158 
   dw 358  ; 159 
   dw 342  ; 160 
   dw 326  ; 161 
   dw 309  ; 162 
   dw 292  ; 163 
   dw 276  ; 164 
   dw 259  ; 165 
   dw 242  ; 166 
   dw 225  ; 167 
   dw 208  ; 168 
   dw 191  ; 169 
   dw 174  ; 170 
   dw 156  ; 171 
   dw 139  ; 172 
   dw 122  ; 173 
   dw 105  ; 174 
   dw 87   ; 175 
   dw 70   ; 176 
   dw 52   ; 177 
   dw 35   ; 178 
   dw 17   ; 179 
   dw 0    ; 180 

