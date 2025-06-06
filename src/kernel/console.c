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


static u8 attr = 7;        // 字符样式
static u16 erase = 0x0720; // 空格

// 光标
typedef struct pos_struct {
    u32 position;
    u32 x;
    u32 y;
} cursor;

// 控制台
typedef struct console_struct {
    u32 position;
    cursor cur;
} console;


// 全局控制台
static console global_console = {
    .position = 0,
    .cur = {
        .position = 85,
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
}

// 获取光标位置
static void get_cur_pos() {
    cursor *cur = &global_console.cur;

    outb(CRT_ADDR_REG, CRT_CURSOR_H);
    cur->position = inb(CRT_DATA_REG) << 8;
    outb(CRT_ADDR_REG, CRT_CURSOR_L);
    cur->position |= inb(CRT_DATA_REG);

    u32 delta = cur->position - global_console.position;
    cur->x = delta % WIDTH;
    cur->y = delta / WIDTH;
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


void console_init()
{
    console_clear();
}

void console_clear()
{

    global_console.position  = 0;
    cursor *cur = &global_console.cur;
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
}

