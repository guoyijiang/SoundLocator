#include <xc.h>
#include "process.h"
#include <plib.h>
void int_ext_init(void)
{
    ANSELBbits.ANSB15 = 0;
    ANSELBbits.ANSB1 = 0;
    ANSELBbits.ANSB3 = 0;
    
    TRISBbits.TRISB15 = 1;
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB3 = 1;
    
    CNENBbits.CNIEB15 = 0;
    CNENBbits.CNIEB1 = 0;
    CNENBbits.CNIEB3 = 0;
    
    CNCONBbits.ON = 1;
    CNCONBbits.SIDL =0;

    INTSetVectorPriority( INT_CHANGE_NOTICE_VECTOR, INT_PRIORITY_LEVEL_5 );
    INTSetVectorSubPriority( INT_CHANGE_NOTICE_VECTOR, INT_SUB_PRIORITY_LEVEL_0 );   
    
    PORTRead(IOPORT_B);
    INTClearFlag(INT_CNB);
    INTEnable( INT_CNB, INT_ENABLED );
}


void __ISR(_CHANGE_NOTICE_VECTOR, ipl5) _ChangeNoticeHandler(void)
{
    
    if( CNSTATBbits.CNSTATB15 )
    {
        //TICK_PAUSE(MK0);
    }
    else if( CNSTATBbits.CNSTATB1)
    {
        //TICK_PAUSE(MK1);
    }
    else if( CNSTATBbits.CNSTATB3 )
    {
        //TICK_PAUSE(MK2);
    }
    
}