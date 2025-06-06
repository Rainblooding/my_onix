//
// Created by Administrator on 25-6-5.
//

#ifndef IO_H
#define IO_H

#include <onix/types.h>

extern u8 inb(u16 port);
extern u16 inw(u16 port);

extern void outb(u16 port, u8 val);
extern void outw(u16 port, u16 val);



#endif //IO_H
