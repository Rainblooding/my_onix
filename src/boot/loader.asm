[org 0x1000]
dw 0x55aa

; 打印字符串
push word loading
call print

    xchg bx, bx
call detect_memory
; 打印字符串
push word loading
call print



jmp $

%include "src/boot/print.asm"
%include "src/boot/disk.asm"
%include "src/boot/memory.asm"

loading:
    db "Loading Onix...", 10, 13, 0; \n\r