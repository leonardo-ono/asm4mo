; AAA 1 - ASCII ADJUST AFTER ADDITION
; Assembler for MikeOS (https://github.com/leonardo-ono/asm4mo)
; Example written by Leonardo Ono (ono.leo@gmail.com)
; 08/02/2012 13:46

mov ah, 0
mov al, '7'
mov bl, '6'

add al, bl

aaa ; 7 + 6 = 13 -> ah=01h al=03h

mov bl, al
mov al, ah 

mov dx, 78h
call dx ; os_print_2hex

mov al, bl
call dx ; os_print_2hex

mov dx, 0fh
call dx ; os_print_newline

ret

