#ifndef __LOG_H__
#define __LOG_H__

#include <plib.h>

#define DEBUG_EN 0

int print(const char* fmt, ...);

#if(DEBUG_EN) 
#define LOG_DEBUG(fmt, ...) print("[D][%s:%d %s]"fmt"\r\n",  __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  print("[I][%s:%d %s]"fmt"\r\n",  __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) print("[E][%s:%d %s]"fmt"\r\n",  __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_HEX(data, len)  log_hex(data, len);
#define LOG_BIN(data, len)  log_binary(data, len);
#else
#define LOG_DEBUG(fmt, ...)
#define LOG_INFO(fmt, ...)
#define LOG_ERROR(fmt, ...)
#define LOG_HEX(data, len)
#define LOG_BIN(data, len)
#endif




#endif
