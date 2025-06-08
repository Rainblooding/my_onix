#include <onix/onix.h>
#include <onix/console.h>
#include <onix/stdarg.h>


char message[] = "hello onix!!!\n";

void test_arg(int cnt, ...)
{
    va_list args;
    va_start(args, cnt);

    int i = 2;
    while(cnt--);
    {
        i = va_arg(args, int);
    }
	console_write(message, sizeof(message) - 1);

    va_end(args);
}

void kernel_init()
{
    console_init();
    test_arg(5, 1, 4, 6, 2, 1);
}