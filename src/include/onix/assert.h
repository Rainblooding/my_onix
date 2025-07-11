//
// Created by Administrator on 25-6-9.
//

#ifndef ASSERT_H
#define ASSERT_H

void assertion_failure(char *exp, char *file, char *base, int line);

#define assert(exp)\
    if(exp)        \
        ;          \
    else           \
        assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)

void panic(const char *fmt, ...);

#endif //ASSERT_H
