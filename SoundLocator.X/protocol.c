#include "protocol.h"
/*
 * 去掉字符串开头的空格
 *
 */
unsigned char* string_trimLeft(const unsigned char* string)
{
    const unsigned char* p = string;

    while(*p == ' ') p++;

    return (unsigned char*)p;
}
