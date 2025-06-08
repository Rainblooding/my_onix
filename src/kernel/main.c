#include <onix/onix.h>
#include <onix/console.h>
#include <onix/printk.h>



void kernel_init()
{
    console_clear();
    int cnt = 30;
    while (cnt--)
    {
        printk("hello onix %#010x\n", cnt);
    }
}