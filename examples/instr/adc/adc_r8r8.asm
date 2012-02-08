; ADC reg8, reg8 - ADD WITH CARRY
; Assembler for MikeOS (https://github.com/leonardo-ono/asm4mo)
; Example written by Leonardo Ono (ono.leo@gmail.com)
; 08/02/2012 16:34

   ; clc ; clear carry flag
   stc ; set carry flag

   mov al, 30
   mov bl, 35

   adc al, bl ; if carry flag is set     -> al = al + bl + 1
              ; if carry flag is not set -> al = al + bl + 0

   mov ah, 0eh
   int 10h    ; print al

   ret


