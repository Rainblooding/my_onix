#include <onix/onix.h>
#include <onix/console.h>


char message[] = "hello onix!!!\n";
void kernel_init()
{
    console_init();

    while(true)
    {
        console_write(message, sizeof(message) - 1);
    }
}