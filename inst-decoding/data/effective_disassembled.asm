bits 16

mov al, [bx + si]
mov bx, [bp + di]
mov dx, [bp + 0]
mov ah, [bx + si + 4]
mov al, [bx + si + 4999]
mov [bx + di], cx
mov [bp + si], cl
mov [bp + 0], ch
