; AAM 1 - ASCII ADJUST AFTER MULTIPLICATION
; Assembler for MikeOS (https://github.com/leonardo-ono/asm4mo)
; Example written by Leonardo Ono (ono.leo@gmail.com)
; 08/02/2012 15:35

; Reference: http://www.scs.carleton.ca/sivarama/asm_book_web
;            /Instructor_copies/ch11_bcd.pdf (see page 11)

mov al, 3    ; multiplier in unpacked BCD form
mov bl, 9    ; multiplicand in unpacked BCD form

mul bl       ; result 001bh is in AX

aam          ; ah=02 al=07h

mov bl, al

mov al, ah
mov dx, 78h
call dx ; os_print_2hex

mov al, bl
call dx ; os_print_2hex

mov dx, 0fh
call dx ; os_print_newline

ret

