   jmp start
   msg db "Hello World !", 0
start:
   mov dx, ds 
   mov ax, 0b800h
   mov si, 0
   mov di, 0
nextChar:
   mov ds, dx
   mov cl, [msg+si]
   cmp cl, 0
   je end
   mov ds, ax
   mov [di], cl
   mov byte [di+1], 65
   add di, 2
   inc si
   jmp nextChar   
end:
   ret
