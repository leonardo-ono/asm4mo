; AAS 1 - ASCII ADJUST AFTER SUBTRACTION
; Assembler for MikeOS (https://github.com/leonardo-ono/asm4mo)
; Example written by Leonardo Ono (ono.leo@gmail.com)
; 08/02/2012 14:21

mov ah, 1
mov al, '2'

mov bl, '3'

sub al, bl

aas ; 12 - 3 = 9 -> ah=00h al=09h

mov bl, al
mov al, ah 

mov dx, 78h
call dx ; os_print_2hex

mov al, bl
call dx ; os_print_2hex

mov dx, 0fh
call dx ; os_print_newline

ret

