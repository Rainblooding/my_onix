[org 0x1000]
dw 0x55aa

; 打印字符串
mov si, loading
call print


jmp $

%include "src/boot/print.asm"
%include "src/boot/disk.asm"

loading:
    db "Loading Onix...", 0; \n\r