; ------------------------------------------------------------------
; Line
; Demo for asm4mo (https://github.com/leonardo-ono/asm4mo/)
;
; Program to demonstrate an implementation of Bresenham's line algorithm
; Written by Troy Martin
; 24/02/2012 12:28
; ------------------------------------------------------------------

   ORG 32768

start:
   mov ah, 0        ; Switch to graphics mode
   mov al, 13h
   int 10h

   mov cx, 25       ; X1
   mov dx, 30       ; Y1
   mov si, 300      ; X2
   mov di, 150      ; Y2
   mov bl, 0Fh      ; Colour: white
   call draw_line

   mov cx, 310      ; X1
   mov dx, 140      ; Y1
   mov si, 287      ; X2
   mov di, 22       ; Y2
   mov bl, 04h      ; Colour: red
   call draw_line

   mov cx, 282      ; X1
   mov dx, 24       ; Y1
   mov si, 33       ; X2
   mov di, 122      ; Y2
   mov bl, 09h      ; Colour: blue
   call draw_line

   call 12h         ; os_wait_for_key

   mov ax, 3        ; Back to text mode
   mov bx, 0
   int 10h
   mov ax, 1003h    ; No blinking text!
   int 10h

   call 9h          ; os_clear_screen
   ret

; Simple wrapper for int 10h.
; IN: AX=X, CX=Y, BL=colour
; OUT: None, registers preserved
put_pixel:
   ; pusha
   push ax
   push cx
   push dx
   push bx
   push sp
   push bp
   push si
   push di

   mov dx, cx
   mov cx, ax
   mov ah, 0Ch
   mov al, bl
   xor bx, bx
   int 10h

   ; popa
   pop di
   pop si
   pop bp
   pop sp
   pop bx
   pop dx
   pop cx
   pop ax

   ret
   

; Implementation of Bresenham's line algorithm. Translated from an implement-
; ation in C (http://www.edepot.com/linebresenham.html)
; IN: CX=X1, DX=Y1, SI=X2, DI=Y2, BL=colour
; OUT: None, registers preserved
draw_line:
   ; pusha          ; Save parameters
   push ax
   push cx
   push dx
   push bx
   push sp
   push bp
   push si
   push di
   
   xor ax, ax       ; Clear variables
   mov di, v_x1
   mov cx, 11
   rep stosw
   
   ; popa           ; Restore and save parameters
   pop di
   pop si
   pop bp
   pop sp
   pop bx
   pop dx
   pop cx
   pop ax

   ; pusha
   push ax
   push cx
   push dx
   push bx
   push sp
   push bp
   push si
   push di
   
   mov [v_x1], cx   ; Save points
   mov [v_x], cx
   mov [v_y1], dx
   mov [v_y], dx
   mov [v_x2], si
   mov [v_y2], di
   
   mov [v_colour], bl ; Save the colour
   
   mov bx, [v_x2]
   mov ax, [v_x1]
   cmp bx, ax
   jl v_x1gtx2
   
   sub bx, ax
   mov [v_dx], bx
   mov ax, 1
   mov [v_incx], ax
   jmp v_test2
   
v_x1gtx2:
   sub ax, bx
   mov [v_dx], ax
   mov ax, 0ffffh ; -1
   mov [v_incx], ax
   
v_test2:
   mov bx, [v_y2]
   mov ax, [v_y1]
   cmp bx, ax
   jl v_y1gty2
   
   sub bx, ax
   mov [v_dy], bx
   mov ax, 1
   mov [v_incy], ax
   jmp v_test3
   
v_y1gty2:
   sub ax, bx
   mov [v_dy], ax
   mov ax, 0ffffh ; -1
   mov [v_incy], ax
   
v_test3:
   mov bx, [v_dx]
   mov ax, [v_dy]
   cmp bx, ax
   jl v_dygtdx
   
   mov ax, [v_dy]
   shl ax, 1
   mov [v_dy], ax
   
   mov bx, [v_dx]
   sub ax, bx
   mov [v_balance], ax
   
   shl bx, 1
   mov [v_dx], bx
   
v_xloop:
   mov ax, [v_x]
   mov bx, [v_x2]
   cmp ax, bx
   je v_done
   
   mov ax, [v_x]
   mov cx, [v_y]
   mov bl, [v_colour]
   call put_pixel
   
   xor si, si
   mov di, [v_balance]
   cmp di, si
   jl v_xloop1
   
   mov ax, [v_y]
   mov bx, [v_incy]
   add ax, bx
   mov [v_y], ax
   
   mov ax, [v_balance]
   mov bx, [v_dx]
   sub ax, bx
   mov [v_balance], ax
   
v_xloop1:
   mov ax, [v_balance]
   mov bx, [v_dy]
   add ax, bx
   mov [v_balance], ax
   
   mov ax, [v_x]
   mov bx, [v_incx]
   add ax, bx
   mov [v_x], ax
   
   jmp v_xloop
   
v_dygtdx:
   mov ax, [v_dx]
   shl ax, 1
   mov [v_dx], ax
   
   mov bx, [v_dy]
   sub ax, bx
   mov [v_balance], ax
   
   shl bx, 1
   mov [v_dy], bx
   
v_yloop:
   mov ax, [v_y]
   mov bx, [v_y2]
   cmp ax, bx
   je v_done
   
   mov ax, [v_x]
   mov cx, [v_y]
   mov bl, [v_colour]
   call put_pixel
   
   xor si, si
   mov di, [v_balance]
   cmp di, si
   jl v_yloop1
   
   mov ax, [v_x]
   mov bx, [v_incx]
   add ax, bx
   mov [v_x], ax
   
   mov ax, [v_balance]
   mov bx, [v_dy]
   sub ax, bx
   mov [v_balance], ax
   
v_yloop1:
   mov ax, [v_balance]
   mov bx, [v_dx]
   add ax, bx
   mov [v_balance], ax
   
   mov ax, [v_y]
   mov bx, [v_incy]
   add ax, bx
   mov [v_y], ax
   
   jmp v_yloop
   
v_done:
   mov ax, [v_x]
   mov cx, [v_y]
   mov bl, [v_colour]
   call put_pixel
   
   ; popa
   pop di
   pop si
   pop bp
   pop sp
   pop bx
   pop dx
   pop cx
   pop ax

   ret
      
   v_x1 dw 0
   v_y1 dw 0
   v_x2 dw 0
   v_y2 dw 0
   
   v_x dw 0
   v_y dw 0
   v_dx dw 0
   v_dy dw 0
   v_incx dw 0
   v_incy dw 0
   v_balance dw 0
   v_colour db 0
   v_pad db 0

; ------------------------------------------------------------------


