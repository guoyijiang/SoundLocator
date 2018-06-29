#include "led.h"
#include <stdbool.h>
#include <plib.h>
#include <math.h>
#include "pwmSim.h"
#include "log.h"

void led_init(void)
{
    TRISBbits.TRISB9  = 0;
    TRISBbits.TRISB10 = 0;
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB0  = 0;
    TRISBbits.TRISB2  = 0;
    TRISBbits.TRISB5  = 0;
    TRISBbits.TRISB7  = 0;
    TRISBbits.TRISB8  = 0;

    led_write(0x00);
    //TRISBbits.TRISB14 = 0;
    //led_state(OFF);
}

//light 0~1

void led_set(u8 led, ENUM_LED_LEVEL sw)
{
    switch(led)
    {
        case 0:{ LEDbits.D0 = sw; break; }
        case 1:{ LEDbits.D1 = sw; break; }
        case 2:{ LEDbits.D2 = sw; break; }
        case 3:{ LEDbits.D3 = sw; break; }
        case 4:{ LEDbits.D4 = sw; break; }
        case 5:{ LEDbits.D5 = sw; break; }
        case 6:{ LEDbits.D6 = sw; break; }
        case 7:{ LEDbits.D7 = sw; break; }
        default: 
            break;       
    } 
}

void led_write(u8 s)
{
    LEDbits.D0 = (bool)(s & (0x01 <<0));
    LEDbits.D1 = (bool)(s & (0x01 <<1));
    LEDbits.D2 = (bool)(s & (0x01 <<2));   
    LEDbits.D3 = (bool)(s & (0x01 <<3));
    LEDbits.D4 = (bool)(s & (0x01 <<4));   
    LEDbits.D5 = (bool)(s & (0x01 <<5));
    LEDbits.D6 = (bool)(s & (0x01 <<6));   
    LEDbits.D7 = (bool)(s & (0x01 <<7));
}


/*使用PWM控制ledstate，pwm源为timer3,但并没有占用*/
void led_state_init(void)
{   
    TRISBbits.TRISB14 = 0;
    PPSOutput(4, RPB14, OC3);
    
    OpenTimer3(T3_ON, 1025);//1025-1 = 1024
    OpenOC3( OC_ON | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_CONTINUE_PULSE | OC_CONTINUE_PULSE , 1024, 1025);
    
    //led_state_setLevel(1024);
}

/*控制ledstate的亮度，亮度等级为0~255*/
void led_state_setLevel(u16 level)
{
    //SetPulseOC3(level, 1024);
    SetDCOC3PWM(level);
}
        
u8 start_flash_arrya[32][8] = {
    {4, 0, 0, 0, 0, 0, 0, 0},
    {4, 0, 0, 0, 0, 0, 0, 0},
    {5, 0, 0, 0, 0, 0, 0, 0},
    {6, 0, 0, 0, 0, 0, 0, 0},
    {7, 0, 0, 0, 0, 0, 0, 0},          // 5

    {6, 1, 0, 0, 0, 0, 0, 0},
    {6, 3, 0, 0, 0, 0, 0, 0},
    {6, 5, 0, 0, 0, 0, 0, 0},
    {6, 7, 0, 0, 0, 0, 0, 0},
    {6, 6, 1, 0, 0, 0, 0, 0},          // 10

    {6, 6, 2, 0, 0, 0, 0, 0},
    {6, 6, 7, 0, 0, 0, 0, 0},
    {6, 6, 6, 4, 0, 0, 0, 0},
    {6, 6, 6, 6, 0, 0, 0, 0},
    {5, 6, 6, 6, 3, 0, 0, 0},          // 15

    {4, 6, 6, 6, 4, 0, 0, 0},
    {3, 5, 6, 6, 5, 0, 0, 0},
    {0, 4, 6, 6, 6, 3, 0, 0},
    {0, 3, 6, 6, 6, 4, 0, 0},
    {0, 1, 6, 6, 6, 5, 0, 0},          // 20

    {0, 0, 6, 6, 6, 6, 0, 0},
    {0, 0, 6, 6, 6, 6, 0, 0},
    {0, 0, 0, 6, 6, 6, 6, 0},
    {0, 0, 0, 6, 6, 6, 6, 0},
    {0, 0, 0, 3, 6, 6, 6, 6},          // 25

    {0, 0, 0, 1, 6, 6, 6, 6},
    {6, 0, 0, 0, 3, 6, 6, 6},
    {6, 0, 0, 0, 0, 6, 6, 6},
    {6, 0, 0, 0, 0, 3, 6, 6},
    {6, 0, 0, 0, 0, 0, 6, 6},          // 30

    {6, 0, 0, 0, 0, 0, 0, 6},
    {6, 0, 0, 0, 0, 0, 0, 0}
};
    

int indexXX = 6;

void led_flash_powerOn(void)
{
    int i = 0;
    float light[8] = {0.0};
    float temp[8] = {0.8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    while(1)
    {
        int j = 2000000;
        
        for( i = 0; i < 8; i++)
        {
            light[i] = temp[(i+indexXX)%8];
        }
        pwm_led_reConfig_batch(light);
        indexXX++;
        while(j--);
        if(indexXX > 24) break;
    }
    
}

void led_flash_biu(int ledIndex)
{
    int i = 0;
    float light[8] = {0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8};
    pwm_led_reConfig_batch(light);
    pwm_led_lock_en(ledIndex);
}


