#ifndef __LED_H__
#define __LED_H__
#include <xc.h>
#include "protocol.h"

//#define led_state(level) LATBbits.LATB14 = level;

typedef enum
{
    OFF  = (unsigned)0x00,
    ON = (unsigned)0x01,
     
}ENUM_LED_LEVEL;

typedef struct {
    unsigned D3:1;
    unsigned :1;
    unsigned D4:1;
    unsigned :1;
    unsigned :1;
    unsigned D5:1;
    unsigned :1;
    unsigned D6:1;
    unsigned D7:1;
    unsigned D0:1;
    unsigned D1:1;
    unsigned D2:1;
    unsigned :1;
    unsigned :1;
    unsigned :1;
    unsigned :1;
} ST_LED_bits;

volatile ST_LED_bits LEDbits __asm__ ("LATB") __attribute__((section("sfrs"), address(0xBF886130)));

void led_init(void);

void led_set(u8 led, ENUM_LED_LEVEL sw);
void led_write(u8 s);

void led_state_init(void);
void led_state_setLevel(u16 level);

void led_flash_powerOn(void);
void led_flash_biu(int ledIndex);



#endif 
