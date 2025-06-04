[org 0x7c00]
jmp start

name:
    db 'CHEN', 10, 13, 0
error_msg:
    db 'ERROR', 10, 13, 0
 
start:
    mov ax, 3
    int 0x10

    push word name
    call print
    push word name
    call print_hex
    call println

;xchg bx, bx; bochs 魔术断点
    
    mov edi, 0x1000; 读取的目标内存
    mov ecx, 2; 起始扇区
    mov bl, 6; 扇区数量

    call read_disk

    cmp word [0x1000], 0x55aa
    jnz error

    jmp 0:0x1002

error:
    push word error_msg
    call print

jmp $

%include "src/boot/print.asm"
%include "src/boot/disk.asm"

times 510 - ($ - $$) db 0

db 0x55, 0xaa