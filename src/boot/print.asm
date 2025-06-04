CHAR_0      equ     0x30 ; 字符‘0’的16进制编码
CHAR_A      equ     0x41 ; 字符‘A’的编码
CHAR_ENTER  equ     13   ; 回车
NEWLINE     equ     10   ; 换行
HEX_PREFIX:
    db "0x", 0
HEX_NUMBER:
    db '0', '0', '0', '0', 0

; 打印字符
; str
print:
    push bp
    mov bp, sp
    push ax
    push si

    mov si, [bp + 4]
    mov ah, 0x0e
.print_next:
    mov al, [si]
    cmp al, 0
    jz .print_done
    int 0x10
    inc si
    jmp .print_next

.print_done:
    pop si
    pop ax
    mov sp, bp
    pop bp
    ret


print_hex_prefix:
    push bp
    mov bp, sp
    push word HEX_PREFIX
    call print
    add sp, 2
    mov sp, bp
    pop bp
    ret

; 打印16进制数字
print_hex:
    push bp
    mov bp, sp
    push ax
    push bx
    push cx
    push dx
    push si

    mov byte [HEX_NUMBER], '0'
    mov byte [HEX_NUMBER + 1], '0'
    mov byte [HEX_NUMBER + 2], '0'
    mov byte [HEX_NUMBER + 3], '0'
    mov si,  HEX_NUMBER + 3

    mov ax, [bp + 4]

    ; 将除数放入bx中 将cx置为0 用于记录数字位数或循环次数
    mov bx, 16
    xor cx, cx

; 分解数字位数 不停的除16 直到ax中的数字为0
.print_hex_next:
    xor dx, dx
    div bx
    cmp dl, 10 ; 根据余数转换为相应的字符
    jl .print_hex_base
    jmp .print_hex_ext

; 0~9
.print_hex_base:
    or dl, CHAR_0
    jmp .print_hex_push

; a~f
.print_hex_ext:
    sub dl, 10
    or dl, CHAR_A

; 将字符压入栈中
.print_hex_push:
    mov [si], dl
    dec  si
    cmp ax, 0
    jne .print_hex_next


; 结束
.print_hex_done:
    push word HEX_NUMBER
    call print
    add  sp, 2
    pop si
    pop dx
    pop cx
    pop bx
    pop ax

    mov sp, bp
    pop bp
    ret


; 打印回车换行
println:
    push bp
    mov bp, sp
    push ax

    mov ah, 0x0e
    mov al, NEWLINE
    int 0x10
    mov al, CHAR_ENTER
    int 0x10

    pop ax
    mov sp, bp
    pop bp
    ret
    

