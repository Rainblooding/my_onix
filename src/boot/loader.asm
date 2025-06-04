[org 0x1000]
dw 0x55aa

; 打印字符串
push word loading
call print

call detect_memory
; 打印字符串

push word loading
call print
add sp, 2

    xchg bx, bx
jmp prepare_protected_mode


jmp $

%include "src/boot/print.asm"
%include "src/boot/memory.asm"
%include "src/boot/gdt.asm"
%include "src/boot/disk.asm"

loading:
    db "Loading Onix...", 10, 13, 0; \n\r