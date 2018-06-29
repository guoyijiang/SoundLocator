#ifndef __INT_EXT_H__
#define __INT_EXT_H__

void int_ext_init(void);

#define ENABLE_MK()     {CNENBbits.CNIEB15=1;CNENBbits.CNIEB1=1;CNENBbits.CNIEB3=1;INTClearFlag(INT_CNB);}
#define DISABLE_MK0()   {CNENBbits.CNIEB15=0;INTClearFlag(INT_CNB);}
#define DISABLE_MK1()   {CNENBbits.CNIEB1=0;INTClearFlag(INT_CNB);}
#define DISABLE_MK2()   {CNENBbits.CNIEB3=0;INTClearFlag(INT_CNB);}


#endif 
