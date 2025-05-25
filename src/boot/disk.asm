read_disk:
    ; 设置读入扇区的数量
    mov dx, 0x1f2
    mov al, cl
    out dx, al

    inc dx; 0x1f3
    mov al, bl
    out dx, al ; 起始扇区的0~7

    inc dx; 0x1f4
    shr ebx, 8
    mov al, bl
    out dx, al ; 起始扇区8~15

    inc dx; 0x1f5
    shr ebx, 8
    mov al, bl
    out dx, al ; 起始扇区16~23

    inc dx; 0x1f6
    shr ebx, 8
    and bl, 0b1111 ; 只保留低4位
    mov al, 0b1110_0000
    or al, bl
    out dx, al

    inc dx; 0x1f7
    mov al, 0x20 ; 写盘
    out dx, al 

.read_disk_reads:
    push cx
    call .read_disk_waits ; 等待数据准备完毕
    call .read_disk_read ; 读取一个扇区
    pop cx
    loop .read_disk_read

    ret

.read_disk_waits:
    mov dx, 0x1f7

.read_disk_waits_check:
    in al, dx
    jmp $+2 ; nop 直接跳转到下一行
    jmp $+2
    jmp $+2
    and al, 0b1000_1000
    cmp al, 0b0000_1000
    jnz .read_disk_waits_check

    ret

.read_disk_read:
    mov dx, 0x1f0 
    mov cx, 256 ;一个扇区256个字

.read_disk_readw:
    in ax, dx
    jmp $+2 ; nop 直接跳转到下一行
    jmp $+2
    jmp $+2
    mov [edi], ax
    add edi, 2 
    loop .read_disk_readw
