prepare_protected_mode:
    xchg bx, bx; 断点

    cli; 关闭中断

    ; 打开 A20 线
    in al,  0x92
    or al, 0b10
    out 0x92, al

    lgdt [gdt_ptr]; 加载 gdt

    ; 启动保护模式
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; 用跳转来刷新缓存，启用保护模式
    jmp dword code_selector:protect_mode

[bits 32]
protect_mode:
    xchg bx, bx; 断点
    mov ax, data_selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax; 初始化段寄存器

    mov esp, 0x10000; 修改栈顶

    mov byte [0xb8000], 'P'

    
    mov edi, 0x10000; 读取的目标内存
    mov ecx, 10; 起始扇区
    mov bl, 200; 扇区数量

    xchg bx, bx; 断点
    call read_disk
    xchg bx, bx; 断点

    jmp dword code_selector:0x10000

    ud2

jmp $; 阻塞

code_selector equ (1 << 3)
data_selector equ (2 << 3)

memory_base equ 0 ; 内存开始的位置：基地址

; 内存界限 4G / 4K - 1
memory_limit equ ((1024 * 1024 * 1024 * 4) / (1024 * 4)) - 1
gdt_ptr:
    dw (gdt_end - gdt_base) - 1
    dd gdt_base
gdt_base:
    dd 0, 0 ;NULL描述符
gdt_code:
    dw memory_limit & 0xffff
    dw memory_base & 0xffff
    db (memory_base >> 16) & 0xff
    ;
    db 0b_1_00_1_1_0_1_0
    db 0b_1_1_0_0_0000 | (memory_limit >> 16) & 0xf
    db (memory_base >> 24) & 0xff
gdt_data:
    dw memory_limit & 0xffff
    dw memory_base & 0xffff
    db (memory_base >> 16) & 0xff
    db 0b_1_00_1_0_0_1_0
    db 0b_1_1_0_0_0000 | (memory_limit >> 16) & 0xf
    db (memory_base >> 24) & 0xff
gdt_end:
