//
// Created by Administrator on 25-6-6.
//
#include <onix/console.h>
#include <onix/io.h>
#include <onix/string.h>


#define CRT_ADDR_REG 0x3D4 // CRT(6845)索引寄存器
#define CRT_DATA_REG 0x3D5 // CRT(6845)数据寄存器

#define CRT_START_ADDR_H 0xC // 显示内存起始位置 - 高位
#define CRT_START_ADDR_L 0xD // 显示内存起始位置 - 低位
#define CRT_CURSOR_H 0xE     // 光标位置 - 高位
#define CRT_CURSOR_L 0xF     // 光标位置 - 低位


#define MEM_BASE 0xB8000              // 显卡内存起始位置
#define MEM_SIZE 0x4000               // 显卡内存大小
#define MEM_END (MEM_BASE + MEM_SIZE) // 显卡内存结束位置
#define WIDTH 80                      // 屏幕文本列数
#define HEIGHT 25                     // 屏幕文本行数
#define ROW_SIZE (WIDTH * 2)          // 每行字节数
#define SCR_SIZE (ROW_SIZE * HEIGHT)  // 屏幕字节数

#define ASCII_NUL 0x00
#define ASCII_ENQ 0x05
#define ASCII_BEL 0x07 // \a
#define ASCII_BS 0x08  // \b
#define ASCII_HT 0x09  // \t
#define ASCII_LF 0x0A  // \n
#define ASCII_VT 0x0B  // \v
#define ASCII_FF 0x0C  // \f
#define ASCII_CR 0x0D  // \r
#define ASCII_DEL 0x7F

static u8 attr = 7;        // 字符样式
static u16 erase = 0x0720; // 空格

// 光标
typedef struct pos_struct {
    u32 mem_position;
    u32 position;
    u32 x;
    u32 y;
} cursor;

// 控制台
typedef struct console_struct {
    u32 mem_position;
    u32 position;
    cursor cur;
} console;


// 全局控制台
static console global_console = {
    .mem_position = MEM_BASE,
    .position = 0,
    .cur = {
        .mem_position = MEM_BASE,
        .position = 0,
        .x = 0,
        .y = 0
    }
};

// 设置控制台位置
static void get_console_pos()
{
    outb(CRT_ADDR_REG, CRT_START_ADDR_H);
    global_console.position = inb(CRT_DATA_REG) << 8;
    outb(CRT_ADDR_REG, CRT_START_ADDR_L);
    global_console.position |= inb(CRT_DATA_REG);

    global_console.mem_position = MEM_BASE + (global_console.position << 1);
}

// 获取光标位置
static void get_cursor_pos() {
    cursor *cur = &global_console.cur;

    outb(CRT_ADDR_REG, CRT_CURSOR_H);
    cur->position = inb(CRT_DATA_REG) << 8;
    outb(CRT_ADDR_REG, CRT_CURSOR_L);
    cur->position |= inb(CRT_DATA_REG);

    u32 delta = cur->position - global_console.position;
    cur->x = delta % WIDTH;
    cur->y = delta / WIDTH;

    cur->mem_position = MEM_BASE + (cur->position << 1);
}

// 设置控制台位置
static void set_console_pos()
{
    outb(CRT_ADDR_REG, CRT_START_ADDR_H); // 开始位置高地址
    outb(CRT_DATA_REG, (global_console.position >> 8 & 0xff));
    outb(CRT_ADDR_REG, CRT_START_ADDR_L);
    outb(CRT_DATA_REG, (global_console.position & 0xff));
}

// 设置光标位置
static void set_cursor_pos()
{
    cursor *cur = &global_console.cur;

    outb(CRT_ADDR_REG, CRT_CURSOR_H); // 开始位置高地址
    outb(CRT_DATA_REG, (cur->position >> 8 & 0xff));
    outb(CRT_ADDR_REG, CRT_CURSOR_L);
    outb(CRT_DATA_REG, (cur->position & 0xff));
}

static void scroll_up()
{
    if (global_console.mem_position + SCR_SIZE + ROW_SIZE < MEM_END)
    {
        u16 *ptr = (u16 *) (global_console.mem_position + SCR_SIZE);
        for(size_t i = 0; i < WIDTH; i++)
        {
            *ptr++ = erase;
        }
        global_console.mem_position += ROW_SIZE;
        global_console.position += WIDTH;

        cursor *cur = &global_console.cur;
        cur->mem_position += ROW_SIZE;
        cur->position += WIDTH;
    }
    else
    {
        memcpy((void *) MEM_BASE, (void *) global_console.mem_position, SCR_SIZE);

        u32 offset = global_console.mem_position - MEM_BASE;
        global_console.mem_position = MEM_BASE;
        global_console.position = 0;

        cursor *cur = &global_console.cur;
        cur->mem_position = cur->mem_position - offset;
        cur->position = cur->position - offset / 2;

        u32 delta = cur->position - global_console.position;
        cur->x = delta % WIDTH;
        cur->y = delta / WIDTH;
    }
    set_console_pos();
    set_cursor_pos();
}

// 实现\b
static void command_bs()
{
    cursor *cur = &global_console.cur;
    if (cur->x)
    {
        cur->x -= 1;
        cur->position -= 1;
        u16 *ptr = (u16 *) cur->mem_position;
        ptr -= 2;
        *ptr = erase;
    }
}

// \n
static void command_lf()
{
    cursor *cur = &global_console.cur;
    if (cur->y + 1 < HEIGHT)
    {
        cur->y += 1;
        cur->position += WIDTH;
        cur->mem_position += ROW_SIZE;
        return;
    }
    scroll_up();
}

static void command_cr()
{
    cursor *cur = &global_console.cur;

    cur->position -= cur->x;
    cur->mem_position -= (cur->x << 1);
    cur->x = 0;

}

static void command_del()
{
    cursor *cur = &global_console.cur;
    if(cur->position)
    {
        cur->y -= (cur->x == 0 ? 0 : 1);
        cur->x = (cur->x + WIDTH - 1) % WIDTH;
        cur->position -= 1;
        cur->mem_position -= 2;
        *(u16 *) cur->mem_position = erase;
    }

}

void console_init()
{
    console_clear();
}

void console_clear()
{
    global_console.mem_position = MEM_BASE;
    global_console.position = 0;

    cursor *cur = &global_console.cur;
    cur->mem_position = MEM_BASE;
    cur->position = 0;
    cur->x = 0;
    cur->y = 0;

    set_console_pos();
    set_cursor_pos();

    u16 *ptr = (u16 *) MEM_BASE;
    while (ptr < MEM_END)
    {
        *ptr++ = erase;
    }
}

void console_write(char *buf, u32 count)
{
    char ch;
    cursor *cur = &global_console.cur;
    char **ptr = (char **) &cur->mem_position;
    while(count--)
    {
        ch = *buf++;
        switch(ch)
        {
            case ASCII_NUL: break;
            case ASCII_BEL: break; // todo \a
            case ASCII_BS: command_bs(); break;
            case ASCII_HT: break;
            case ASCII_LF: command_lf(); command_cr(); break;
            case ASCII_VT: break;
            case ASCII_FF: command_lf(); break;
            case ASCII_CR: command_cr(); break;
            case ASCII_DEL: command_del(); break;
            default:
                if (cur->x >= WIDTH)
                {
                    cur->x -= WIDTH;
                    cur->position -= WIDTH;
                    cur->mem_position -= ROW_SIZE;
                    command_lf();

                }
                *(*ptr)++ = ch;
                *(*ptr)++ = attr;
                cur->x += 1;
                cur->position += 1;
                break;
        }
    }

    set_cursor_pos();
    set_console_pos();
}

