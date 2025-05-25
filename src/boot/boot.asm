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

;xchg bx, bx; bochs 魔术断点
    mov cl, 1
    mov edi, 0x1000
    mov ebx, 0

    call read_disk

jmp $

%include "src/boot/print.asm"
%include "src/boot/disk.asm"

times 510 - ($ - $$) db 0

db 0x55, 0xaa