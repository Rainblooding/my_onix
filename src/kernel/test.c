# 1 "main.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "main.c"
# 1 "../include/onix/onix.h" 1





void kernel_init();
# 2 "main.c" 2

int magic = 20250601;
char message[] = "hello onix!!!";
char buf[1024];

void kernel_init()
{
    char *video = (char *) 0xb8000;
    for(int i = 0; i < sizeof(message); i++)
    {
        video[i * 2] = message[i];
    }
}
