; CLC 1 - CLEAR CARRY FLAG
; Assembler for MikeOS (https://github.com/leonardo-ono/asm4mo)
; Example written by Leonardo Ono (ono.leo@gmail.com)
; 08/02/2012 16:16

   clc ; clear carry flag
   ; stc ; set carry flag

   jc cset

cnotset:
   mov si, c_flag_notset
   jmp print

cset:
   mov si, c_flag_set
   jmp print

print:
   mov dx, 3h
   call dx ; os_print_string

   mov dx, 0fh
   call dx ; os_print_newline

end:
   ret

c_flag_set     db "Carry flag is set.", 0
c_flag_notset  db "Carry flag is not set.", 0

