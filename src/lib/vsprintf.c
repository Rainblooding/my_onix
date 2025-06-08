//
// Created by ch on 25-6-8.
//

#include <onix/stdarg.h>
#include <onix/string.h>

#define ZEROPAD 1  // 填充零
#define SIGN 2     // unsigned/signed long
#define PLUS 4     // 显示加
#define SPACE 8    // 如是加，则置空格
#define LEFT 16    // 左调整
#define SPECIAL 32 // 0x
#define SMALL 64   // 使用小写字母

#define is_digit(c) ((c) >= '0' && (c) <= '9')

static int skip_atoi(const char **s)
{
    int i = 0;
    while (is_digit(**s))
    {
        i = i * 10 + *((*s)++) - '0';
    }
    return i;
}

// 将整数转换为指定进制的字符串
// str - 输出字符串指针
// num - 整数
// base - 进制基数
// size - 字符串长度
// precision - 数字长度(精度)
// flags - 选项
static char *number(char *str, unsigned long num, int base, int size, int precision, int flags)
{
    char c, sign, tmp[36];
    const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i, index;
    char *ptr =  str;

    if (flags & SMALL) digits = "0123456789abcdefghijklmnopqrstuvwxyz";

    if (flags & LEFT) flags &= ~ZEROPAD;

    if (base < 2 || base > 36) return 0;
    c = (flags & ZEROPAD) ? '0' : ' ';
    if (flags & SIGN && num < 0)
    {
        sign = '-';
        num = - num;
    }
    else
    {
        sign = flags & PLUS ? '+' : ((flags & SPACE) ? ' ' : 0);
    }
    if (sign) size--;

    if (flags & SPECIAL)
    {
        if (base == 16) size -= 2;
        else if (base == 8) size--;
    }

    i = 0;

    if (num == 0) tmp[i++] = '0';
    else
        while(num != 0)
        {
            index = num % base;
            num /= base;
            tmp[i++] = digits[index];
        }

    if (i > precision) precision = i;

    size -= precision;

    if(!(flags & (ZEROPAD + LEFT)))
        while (size-- > 0)
            *str++ = ' ';
    if (sign) *str++ = sign;
    if (flags & SPECIAL)
    {
        if (base == 8) *str++ = '0';
        else if (base == 16)
        {
            *str++ = '0';
            *str++ = digits[33];
        }
    }


    while (!(flags & LEFT) && size-- > 0) *str++ = c;
    while (i < precision--) *str++ = '0';
    while (i-- > 0) *str++ = tmp[i];
    while (size-- > 0) *str++ = ' ';

    return str;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
    int len;
    int i;
    // 用于存放转换过程中的字符串
    char *str;
    char *s;
    int *ip;

    int flags;
    int field_width; // 输出字段宽度
    int precision;   // min 整数数字个数；max 字符串中字符个数
    int qualifier;   // 'h', 'l' 或 'L' 用于整数字段

    for(str = buf; *fmt; ++fmt)
    {
        if (*fmt != '%')
        {
            *str++ = *fmt;
            continue;
        }

        // 下面取得格式指示字符串中的标志域，并将标志常量放入 flags 变量中
        flags = 0;
    repeat:
        // 掉过第一个 %
        ++fmt;
        switch (*fmt)
        {
            case '-': flags |= LEFT; goto repeat;
            case '+': flags |= PLUS; goto repeat;
            case ' ': flags |= SPACE; goto repeat;
            case '#': flags |= SPECIAL; goto repeat;
            case '0': flags |= ZEROPAD; goto repeat;
        }

        // 取当前参数字段宽度域值，放入 field_width 变量中
        field_width = -1;

        // 如果宽度域中是数值则直接取其为宽度值
        if (is_digit(*fmt))
        {
            field_width = skip_atoi(&fmt);
        }
        // 如果宽度域中是字符 '*'，表示下一个参数指定宽度
        else if (*fmt == '*')
        {
            ++fmt;
            // 因此调用 va_arg 取宽度值
            field_width = va_arg(args, int);

            // 若此时宽度值小于 0，则该负数表示其带有标志域 '-' 标志（左对齐）
            if (field_width < 0)
            {
                // 因此还需在标志变量中添入该标志，并将字段宽度值取为其绝对值
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        // 取格式转换串的精度域，并放入 precision 变量中
        precision = -1;
        // 精度域开始的标志是'.' 其处理过程与上面宽度域的类似
        if (*fmt == '.')
        {
            ++fmt;
            if (is_digit(*fmt))
            {
                precision = skip_atoi(&fmt);
            }
            else if (*fmt == '*')
            {
                ++fmt;
                precision = va_arg(args, int);
            }
            if (precision < 0) precision = 0;
        }


        // 下面这段代码分析长度修饰符，并将其存入 qualifer 变量
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
        {
            qualifier = *fmt;
            ++fmt;
        }

        // 下面分析转换指示符
        switch (*fmt)
        {
            // 如果转换指示符是'c'，则表示对应参数应是字符
            case 'c':
                while (!(flags & LEFT) && (--field_width > 0)) *str++ = ' ';
                *str++ = (unsigned char) va_arg(args, int);
                // 如果宽度还大于零 表示左对齐
                while (--field_width > 0) *str++ = ' ';
                break;
            case 's':
                s = va_arg(args, char *);
                // 先获取参数字符串长度
                len = strlen(s);
                // 若其超过了精度域值, 则扩展进度域=字符串长度
                if (precision < 0) precision = len;
                else if (len > precision) len = precision;
                while (!(flags & LEFT) && (len < field_width--)) *str++ = ' ';
                for (i = 0; i < len; ++i) *str++ = *s++;
                while (len < field_width--) *str++ = ' ';
                break;
            case 'o':
                str = number(str, (unsigned long) va_arg(args, unsigned long), 8, field_width, precision, flags);
                break;
            case 'p':
                if (field_width  == -1)
                {
                    field_width = 8;
                    flags |= ZEROPAD;
                }
                str = number(str, (unsigned long) va_arg(args, void *), 16, field_width, precision, flags);
                break;
            case 'x':
                flags |= SMALL;
            case 'X':
                str = number(str, (unsigned long) va_arg(args, unsigned long), 16, field_width, precision, flags);
                break;
            case 'd':
            case 'i':
                flags != SIGN;
            case 'u':
                str = number(str, (unsigned long) va_arg(args, unsigned long), 16, field_width, precision, flags);
                break;
            case 'n':
                ip = va_arg(args, int *);
                *ip = (str - buf);
                break;
            default:
                // 若格式转换符不是 '%'，则表示格式字符串有错
                if (*fmt != '%')
                    // 直接将一个 '%' 写入输出串中
                        *str++ = '%';
                // 如果格式转换符的位置处还有字符，则也直接将该字符写入输出串中
                // 然后继续循环处理格式字符串
                if (*fmt)
                    *str++ = *fmt;
                else
                    // 否则表示已经处理到格式字符串的结尾处，则退出循环
                        --fmt;
                break;

        }
    }

    // 最后在转换好的字符串结尾处添上字符串结束标志
    *str = '\0';

    // 返回转换好的字符串长度值
    return str - buf;
}

int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int i = vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}