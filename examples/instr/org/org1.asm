; ORG 1 - BINARY FILE PROGRAM ORIGIN
; Assembler for MikeOS (https://github.com/leonardo-ono/asm4mo)
; Example written by Leonardo Ono (ono.leo@gmail.com)
; 09/02/2012 10:11

   org 4345h   ; 43h='C' 45h='E'

   msg db 90h  ; msg=4345h since it's the first thing

   mov bx, msg ; bx=4345h

   mov ah, 0eh
   mov al, bh  ; al=43h
   int 10h     ; print 'C'

   mov ah, 0eh
   mov al, bl  ; al=45h
   int 10h     ; print 'E'

   jmp short end

   mov ah, 0eh ; ignored because of jmp short
   mov al, 'X' ; ignored because of jmp short
   int 10h     ; ignored because of jmp short

end:
   ret

