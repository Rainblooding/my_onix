base_addr:
    db "BaseAddr:", 0
length:
    db "Length:", 0
type:
    db "Type:", 0
separator:
    db ",", 0
detecting:
    db "Detecting Memory Success...", 10, 13, 0; \n\r
ards_count:
    dw 0
ards_buffer:
times 256 db 0
ards_buffer_end:
    db "end", 0
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

    push word detecting
    call print
    add sp, 2

    ; 结构体数量
    mov cx, [ards_count]
    ; 结构体指针
    mov si, ards_buffer
.detect_memory_next_show:

    push word base_addr
    call print
    add sp, 2
    call print_hex_prefix

    mov eax, [si]
    push eax
    call .print_detect_memory
    add sp, 4
    mov eax, [si + 4]
    push eax
    call .print_detect_memory
    add sp, 4

    push word separator
    call print
    add sp, 2

    push word length
    call print
    add sp, 2
    mov eax, [si + 8]
    push eax
    call .print_detect_memory
    add sp, 4
    mov eax, [si + 12]
    push eax
    call .print_detect_memory
    add sp, 4
    push word separator
    call print
    add sp, 2

    push word type
    call print
    add sp, 2
    mov eax, [si + 16]
    push eax
    call .print_detect_memory
    add sp, 4

    add si, 20
    call println
    loop .detect_memory_next_show
    ret


.print_detect_memory:
    push bp
    mov bp, sp
    push eax


    mov eax, [bp + 4]
    push ax
    call print_hex
    add sp, 2
    shr eax, 16
    push ax
    call print_hex
    add sp, 2

    pop eax
    mov sp, bp
    pop bp
    ret

.detect_memory_error:
