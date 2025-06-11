#include <onix/onix.h>
#include <onix/console.h>
#include <onix/printk.h>
#include <onix/assert.h>



void kernel_init()
{
    console_clear();

    assert(3 > 5);
}