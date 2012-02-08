; ADC mem, reg8 - ADD WITH CARRY
; Assembler for MikeOS (https://github.com/leonardo-ono/asm4mo)
; Example written by Leonardo Ono (ono.leo@gmail.com)
; 08/02/2012 17:05

; clc ; clear carry flag
stc ; set carry flag

mov dl, 35

adc [value], dl ; if carry flag is set     -> [value] = [value] + bl + 1
                ; if carry flag is not set -> [value] = [value] + bl + 0

mov al, [value]
mov ah, 0eh
int 10h    ; print al

; -----------------------------------
; clc
stc

mov dl, 34
mov bx, 2
adc [bx + value], dl ; [bx + value] = 34
                     ; dl = 34
                     ; 34 + 34 = 68 = D (+1 if carry flag is set)

mov al, [value + bx]
mov ah, 0eh
int 10h    ; print al 

; -----------------------------------
; clc
stc

mov dl, 30
mov bx, 4
mov si, 3
adc [value + bx + si], dl ; [value + bx + si] = 39
                          ; dl = 30
                          ; 30 + 39 = 69 = E (+1 if carry flag is set)

mov al, [bx + value + si]
mov ah, 0eh
int 10h    ; print al

ret

value db 32, 33, 34, 35, 36, 37, 38, 39, 40

