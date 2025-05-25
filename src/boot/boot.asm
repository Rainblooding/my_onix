[org 0x7c00]
jmp start

name:
    db 'CHEN', 0
 
start:
    mov ax, 3
    int 0x10

    mov si, name
    call print

    mov ax, name
    call print_hex

jmp $
%include "src/boot/print.asm"

times 510 - ($ - $$) db 0

db 0x55, 0xaa