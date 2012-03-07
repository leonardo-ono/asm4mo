   org 32768
   bits 16

   mov ax, 1
   mov bx, 1
   cmp ax, bx

   JO label
   JNO label
   JB label
   JC label
   JNAE label
   JAE label
   JNB label
   JNC label

   JNE label
   JNZ label
   JBE label
   JNA label
   JA label
   JNBE label
   JS label
   JNS label
   JP label
   JPE label
   JNP label
   JPO label
   JL label
   JNGE label
   JGE label
   JNL label
   JLE label
   JNG label
   JG label
   JNLE label

label:
   mov ah, 0eh
   mov al, 41h
   int 10h

   ret


