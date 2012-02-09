; CALL imm8 - CALL SUBROUTINE
; Assembler for MikeOS (https://github.com/leonardo-ono/asm4mo)
; Example written by Leonardo Ono (ono.leo@gmail.com)
; 09/02/2012 16:18

   mov si, msg
   call 3   ; os_print_string
   call 15  ; os_print_newline
   ret

msg db "Hello World !", 0   


