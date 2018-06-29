/*
*simple pwm
*/

#ifndef __PWM_SIM_H__
#define	__PWM_SIM_H__

#include "data.h"
#include <stdbool.h>
#define PWM_LED_NUM 8
#define PWM_PERIOD_CNT 32

void pwm_led_run(void);
void pwm_led_stop(void);

void pwm_led_reConfig(int ledIndex, float light);
void pwm_led_reConfig_batch(float *light);

#endif	/* __PROCESS_H__ */

