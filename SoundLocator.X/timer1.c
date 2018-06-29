#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <peripheral/timer.h>
#include "timer1.h"
#include "protocol.h"

#ifndef SYSTEM_PERIPHERAL_CLOCK
#define SYSTEM_PERIPHERAL_CLOCK F_PB_CLK
#endif

/*自动计算timer1重装载值*/
#define CLOCK_DIVIDER TIMER_PRESCALER_1
#define PR1_SETTING (SYSTEM_PERIPHERAL_CLOCK/1000/1)

#if (PR1_SETTING > 0xFFFF)
#undef CLOCK_DIVIDER
#undef PR1_SETTING
#define CLOCK_DIVIDER TIMER_PRESCALER_8
#define PR1_SETTING (SYSTEM_PERIPHERAL_CLOCK/1000/8)
#endif

#if (PR1_SETTING > 0xFFFF)
#undef CLOCK_DIVIDER
#undef PR1_SETTING
#define CLOCK_DIVIDER TIMER_PRESCALER_64
#define PR1_SETTING (SYSTEM_PERIPHERAL_CLOCK/1000/64)
#endif

#if (PR1_SETTING > 0xFFFF)
#undef CLOCK_DIVIDER
#undef PR1_SETTING
#define CLOCK_DIVIDER TIMER_PRESCALER_256
#define PR1_SETTING (SYSTEM_PERIPHERAL_CLOCK/1000/256)
#endif


/* Compiler checks and configuration *******************************/
#ifndef TIMER_MAX_1MS_CLIENTS
    #define TIMER_MAX_1MS_CLIENTS 10
#endif

/* Definitions *****************************************************/
#define STOP_TIMER_IN_IDLE_MODE     0x2000
#define TIMER_SOURCE_INTERNAL       0x0000
#define TIMER_SOURCE_EXTERNAL       0x0002
#define TIMER_ON                    0x8000
#define GATED_TIME_DISABLED         0x0000
#define TIMER_16BIT_MODE            0x0000

#define TIMER_PRESCALER_1           0x0000
#define TIMER_PRESCALER_8           0x0010
#define TIMER_PRESCALER_64          0x0020
#define TIMER_PRESCALER_256         0x0030

#define TIMER_INTERRUPT_PRIORITY    0x0001
#define TIMER_INTERRUPT_PRIORITY_4  0x0004

#define TIMER_INT_PRIOR             T1_INT_PRIOR_2
#define TIMER_INT_SUB_PRIOR         T1_INT_SUB_PRIOR_0
/* Type Definitions ************************************************/
typedef struct
{
    TICK_HANDLER handle;
    uint32_t rate;
    uint32_t count;
    bool enable;
} TICK_REQUEST;

/* Variables *******************************************************/
static TICK_REQUEST requests[TIMER_MAX_1MS_CLIENTS];
static bool configured = false;

/*********************************************************************
* Function: void TIMER_CancelTick(TICK_HANDLER handle)
*
* Overview: Cancels a tick request.
*
* PreCondition: None
*
* Input:  handle - the function that was handling the tick request
*
* Output: None
*
********************************************************************/
void TIMER_CancelTick(TICK_HANDLER handle)
{
    uint8_t i;

    for(i = 0; i < TIMER_MAX_1MS_CLIENTS; i++)
    {
        if(requests[i].handle == handle)
        {
            requests[i].handle = NULL;
        }
    }
}

/*********************************************************************
* Function: void TIMER_ResetCount(TICK_HANDLER handle)
*
* Overview: Reset a tick request.
*
* PreCondition: delta t = 1ms
*
* Input:  handle - the function that was handling the tick request
*
* Output: None
*
********************************************************************/
void TIMER_ResetCount(TICK_HANDLER handle)
{
    uint8_t i;

    for(i = 0; i < TIMER_MAX_1MS_CLIENTS; i++)
    {
        if(requests[i].handle == handle)
        {
            requests[i].count = 0;
        }
    }
}

/*********************************************************************
 * Function: bool TIMER_RequestTick(TICK_HANDLER handle, uint32_t rate)
 *
 * Overview: Requests to receive a periodic event.
 *
 * PreCondition: None
 *
 * Input:  handle - the function that will be called when the time event occurs
 *         rate - the number of ticks per event.
 *
 * Output: bool - true if successful, false if unsuccessful
 *
 ********************************************************************/
bool TIMER_RequestTick ( TICK_HANDLER handle , uint32_t rate )
{
    uint8_t i;
	
    if(configured == false)
    {
        return false;
    }

    for(i = 0; i < TIMER_MAX_1MS_CLIENTS; i++)
    {
        if(requests[i].handle == NULL)
        {
            requests[i].handle = handle;
            requests[i].rate = rate;
            requests[i].count = 0;
            requests[i].enable = false;
            return true;
        }
    }

    return false;
}


void TIMER_Start(TICK_HANDLER handle)
{
    uint8_t i;
    for(i = 0; i < TIMER_MAX_1MS_CLIENTS; i++)
    {
        if(requests[i].handle == handle)
        {
           requests[i].enable = true;
        }
    }
}
void TIMER_Stop(TICK_HANDLER handle)
{
    uint8_t i;
    for(i = 0; i < TIMER_MAX_1MS_CLIENTS; i++)
    {
        if(requests[i].handle == handle)
        {
           requests[i].enable = false;
        }
    }
}

/*********************************************************************
 * Function: bool TIMER_SetConfiguration(TIMER_CONFIGURATIONS configuration)
 *
 * Overview: Initializes the timer.
 *
 * PreCondition: None
 *
 * Input:  None
 *
 * Output: bool - true if successful, false if unsuccessful
 *
 ********************************************************************/
bool TIMER_SetConfiguration ( TIMER_CONFIGURATIONS configuration )
{
    switch(configuration)
    {
        case TIMER_CONFIGURATION_1MS:
            memset(requests, 0, sizeof(requests));
            
            ConfigIntTimer1(T1_INT_ON | TIMER_INT_PRIOR | TIMER_INT_SUB_PRIOR);
            OpenTimer1(TIMER_ON | CLOCK_DIVIDER | TIMER_SOURCE_INTERNAL,PR1_SETTING);  //分频设置为1

            configured = true;
            return true;

        case TIMER_CONFIGURATION_OFF:
            DisableIntT1;
            INTClearFlag(INT_T1);//中断标志清零
            OpenTimer1(T1_OFF,PR1_SETTING);
            configured = false;
            return true;
    }

    return false;
}

/****************************************************************************
  Function:
    void __attribute__((__interrupt__, auto_psv)) _T3Interrupt(void)

  Description:
    Timer ISR, used to update application state. If no transfers are pending
    new input request is scheduled.
  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/


void __ISR(_TIMER_1_VECTOR, ipl2) _Timer1Handler(void)
{
    //DisableIntT1;
    uint8_t i;
    for(i = 0; i < TIMER_MAX_1MS_CLIENTS; i++)
    {
        if(requests[i].handle != NULL)
        {
            if(requests[i].enable)
            {
                requests[i].count++;
            }
            
            if(requests[i].count == requests[i].rate)
            {
                requests[i].handle();
                requests[i].count = 0;
            }
        }
    }
    INTClearFlag(INT_T1);//中断标志清零
    //EnableIntT1;
}
