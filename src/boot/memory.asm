base_addr:
    db "BaseAddr:", 0
length:
    db "Length:", 0
separator:
    db ",", 0
detecting:
    db "Detecting Memory Success...", 10, 13, 0; \n\r
ards_count:
    dw 0
ards_buffer:
times 512 db 0

detect_memory:
    xor ebx, ebx

    mov ax, 0
    mov es, ax
    mov edi, ards_buffer

    mov edx, 0x534d4150

.detect_memory_next:
    mov eax, 0xE820
    mov ecx, 20
    int 0x15

    jc .detect_memory_error

    add di, cx

    inc word [ards_count]

    cmp ebx, 0
    jnz  .detect_memory_next

    mov si, detecting
    call print

    ; 结构体数量
    mov cx, [ards_count]
    ; 结构体指针
    mov si, 0
.detect_memory_next_show:
    push cx
    mov eax, [ards_buffer + si]
    push ax
    shr eax, 16
    push ax
    mov eax, [ards_buffer + si + 4]
    push ax
    shr eax, 16
    push ax

    mov si, base_addr
    call print
    call print_hex_prefix
    pop ax
    call print_hex
    pop ax
    call print_hex
    pop ax
    call print_hex
    pop ax
    call print_hex
    mov si, separator
    call print

    mov ebx, [ards_buffer + si + 8]
    mov edx, [ards_buffer + si + 16]
    add si, 20
    xchg bx, bx
    pop cx
    loop .detect_memory_next_show
    ret

.detect_memory_error:
