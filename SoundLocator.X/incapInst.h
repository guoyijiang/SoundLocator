#ifndef __INCAP_INST_H__
#define __INCAP_INST_H__

#define MK0_BUFFER_FULL()  (IC2CONbits.ICOV)
#define MK1_BUFFER_FULL()  (IC3CONbits.ICOV)
#define MK2_BUFFER_FULL()  (IC4CONbits.ICOV)

void incap_timer_init(void);
void incap_timer_reset(void);
void incap_inst_init(void);
int incap_dumpData(ENUM_MK mk, uint32_t *buffer);

#endif 