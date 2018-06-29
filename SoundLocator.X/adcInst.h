#ifndef __ADCINST_H__
#define __ADCINST_H__
#include <stdint.h>
#define ADC_VREF 3.3f
typedef struct
{
    uint16_t v_avg;
    
}ST_ADC_RESULT;

void adc_init(void);
void adc2_init(void);
#endif