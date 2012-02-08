; AAD 1 - ASCII ADJUST BEFORE DIVISION
; Assembler for MikeOS (https://github.com/leonardo-ono/asm4mo)
; Example written by Leonardo Ono (ono.leo@gmail.com)
; 08/02/2012 14:36

; Reference: http://www.scs.carleton.ca/sivarama/asm_book_web
;            /Instructor_copies/ch11_bcd.pdf (see page 14)

; Example: divide 27 by 5

mov ah, 02h
mov al, 07h ; dividend in unpacked BCD form

mov bl, 05h ; divisor in unpacked BCD form

aad          ; Ax=001bh

div bl       ; ah=02h al=05h

mov bl, al

mov al, ah
mov dx, 78h
call dx ; os_print_2hex

mov al, bl
call dx ; os_print_2hex

mov dx, 0fh
call dx ; os_print_newline

ret

