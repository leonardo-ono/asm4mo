   jmp short start

   msgEqual db "Equal ...", 0
   msgNotEqual db "Not equal ...", 0

start:
   mov ax, 123
   mov bx, 123
   cmp ax, bx

   jz equal
   ; je equal

   jmp short notEqual


equal:
   mov si, msgEqual
   call 3h ; os_print_string
   call 15 ; os_print_newline
   jmp short end

notEqual:
   mov si, msgNotEqual
   call 3h ; os_print_string
   call 15 ; os_print_newline
   jmp short end

end:
   ret
