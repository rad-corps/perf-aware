; ========================================================================
;
; (C) Copyright 2023 by Molly Rocket, Inc., All Rights Reserved.
;
; This software is provided 'as-is', without any express or implied
; warranty. In no event will the authors be held liable for any damages
; arising from the use of this software.
;
; Please see https://computerenhance.com for further information
;
; ========================================================================

; ========================================================================
; LISTING 39
; ========================================================================

bits 16

; immediate to memory
mov byte [bp + di], 16

; Register-to-register
mov si, bx
mov dh, al

; 8-bit immediate-to-register
;| 1 1 0 0 0 1 1 0 | 
;|     OPCODE    w |
mov cl, 12
mov ch, -12

; 16-bit immediate-to-register
mov cx, 12
mov cx, -12
mov dx, 3948
mov dx, -3948

; Source address calculation
; | 1 0 0 0 1 0 1 0 | 0 0 0 0 0 0 0 0
; |    OPCODE   D W | MOD REG   RM
mov al, [bx + si]

; | 1 0 0 0 1 0 1 1 | 0 0 0 1 1 0 1 1
; |    OPCODE   D W | MOD REG   RM
mov bx, [bp + di]

; | 1 0 0 0 1 0 1 1 | 0 0 0 1 0 1 1 0 # special case
; |    OPCODE   D W | MOD REG   RM
mov dx, [bp]

; Source address calculation plus 8-bit displacement
mov ah, [bx + si + 4]

; Source address calculation plus 16-bit displacement
mov al, [bx + si + 4999]

; Dest address calculation
mov [bx + di], cx
mov [bp + si], cl
mov [bp], ch



