; Stars 
; Demo for asm4mo (https://github.com/leonardo-ono/asm4mo/)
; Written by Leonardo Ono (ono.leo@gmail.com)
; 10/02/2011 22:34
; use: asm stars.asm stars.bin
 
   mov ah, 0
   mov al, 13h
   int 10h ; change to graphic mode

   mov cx, 100 ; number of stars

nextStar:
   push cx

   mov dx, 0

   mov ax, 1
   mov bx, 65535
   call 0b7h ; os_get_random

   add dx, cx

   mov ax, 1
   mov bx, 100
   call 0b7h ; os_get_random

   add dx, cx

   mov bx, dx
   call pset

   pop cx
   loop nextStar

end:
   call 12h ; os_wait_for_key

   mov ah, 0
   mov al, 3h
   int 10h ; return to text mode

   ret ; return to MikeOS

; bx = position
pset:
   mov dx, ds
   mov ax, 0a000h
   mov ds, ax
   mov [bx], bl
   mov ds, dx
   ret

