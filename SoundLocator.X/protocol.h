#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#define u8  unsigned char  
#define u16 unsigned short
#define u32 unsigned int

#define F_SYS_CLK (64000000)
#define F_PB_CLK  (16000000)

#define bittest(var, bit) ((var) & (1 <<(bit)))
#define bitset(var, bit) ((var) |= (1 << (bit)))
#define bitclr(var, bit) ((var) &= ~(1 << (bit)))

#define MAX(a,b) (a>b ? a:b)
#define MIN(a,b) (a>b ? b:a)
/*
typedef enum
{
    STATE_IDLE = 0,
    STATE_WAIT_STICK,
    STATE_SAMPLING,
    STATE_UPLOADING,
    STATE_CALC,
    STATE_CHANGE_PARAM,
    STATE_ERROR,
    
}ENUM_MAIN_STATE;


typedef struct
{
    ENUM_MAIN_STATE state;
    
}ST_SETTING;
*/
unsigned char* string_trimLeft(const unsigned char* string);

#endif
