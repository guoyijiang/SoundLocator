#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifndef TIMER1_H
#define TIMER1_H

#define TIMER_TICK_INTERVAL_MICRO_SECONDS 1000

/* Type Definitions ***********************************************/
typedef void (*TICK_HANDLER)(void);

typedef enum
{
    TIMER_CONFIGURATION_1MS,
    TIMER_CONFIGURATION_OFF
} TIMER_CONFIGURATIONS;


void TIMER_CancelTick(TICK_HANDLER handle);

void TIMER_ResetCount(TICK_HANDLER handle);

bool TIMER_RequestTick(TICK_HANDLER handle, uint32_t rate);

void TIMER_Start(TICK_HANDLER handle);
void TIMER_Stop(TICK_HANDLER handle);

bool TIMER_SetConfiguration(TIMER_CONFIGURATIONS configuration);

#endif	/* TIMER1_H */
