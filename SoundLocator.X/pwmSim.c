#include <plib.h>
#include "pwmSim.h"
#include "led.h"
#include <stdbool.h>

uint16_t pwm_led_cmpCnt[PWM_LED_NUM] = {0};
bool pwm_led_lock[PWM_LED_NUM] = {0};

int pwm_cnt = 0;

void pwm_led_run(void)
{
    ConfigIntTimer4(T4_INT_ON | T4_INT_PRIOR_5 | T4_INT_SUB_PRIOR_0);
    OpenTimer4(T4_ON | T4_PS_1_1 | T4_SOURCE_INT,10000);  //1000000: 2/32 ms
}

void pwm_led_stop(void)
{
    ConfigIntTimer4(T4_INT_OFF);
    CloseTimer4();
    int i = 0;
    for(i = 0 ; i < PWM_LED_NUM; i++)
    {
        if(pwm_led_lock[i])
        {
            led_set(i, ON);
        }   
        else
        {
            led_set(i, OFF);
        }
    } 
}

//float light : 0~1
void pwm_led_reConfig(int ledIndex, float light)
{
    int i = 0;
    int cnt = (uint16_t)(light*(float)(PWM_PERIOD_CNT-1));
    if(light < 0) return;
    pwm_led_cmpCnt[ledIndex] = MIN(PWM_PERIOD_CNT-1, pwm_led_cmpCnt[ledIndex] + cnt);
    pwm_led_lock[ledIndex] = false;
}

void pwm_led_reConfig_batch(float *light)
{
    int i = 0;
    for( ; i < PWM_LED_NUM; i++)
    {
        pwm_led_reConfig(i, light[i]);
    }
}

void pwm_led_lock_en(int ledIndex)
{
    pwm_led_lock[ledIndex] = true;
}

/*
* low [0,pwm_map[i].cmpCnt)
* high [pwm_map[i].cmpCnt, PWM_PERIOD_CNT)
*/
void __ISR(_TIMER_4_VECTOR, ipl5) pwm_runTick_handler(void)
{
    static int subCnt = 0;
    int i = 0;
    
    pwm_cnt = (++pwm_cnt)%PWM_PERIOD_CNT;
    
    for(i = 0 ; i < PWM_LED_NUM; i++)
    {
        if(pwm_cnt == pwm_led_cmpCnt[i])
            led_set(i, OFF);
        else if(pwm_cnt == 0)
            led_set(i, ON);
        
    }

    //渐灭效果
    if(subCnt == 49)
    {
        for(i = 0 ; i < PWM_LED_NUM; i++)
        {
            if(pwm_led_lock[i] == false)
            {
                if(pwm_led_cmpCnt[i] > 0) pwm_led_cmpCnt[i] = pwm_led_cmpCnt[i]-1;
            }   
        }
    }
    subCnt = ++subCnt%50;
    INTClearFlag(INT_T4);//中断标志清零
}

