#include <string.h>
#include <stdio.h>
#include "log.h"
#include "uart1.h"
#include "timer1.h"
#include "debug.h"
#include "protocol.h"

#define PBCLK		    F_PB_CLK
#define UART1_BAUDRATE	115200

#define UART1_INT_PRIOR       INT_PRIORITY_LEVEL_5
#define UART1_INT_SUB_PRIOR   INT_SUB_PRIORITY_LEVEL_0

#define UART1_BUFFER_LEN (1024*2)

#define UART1_TX_INT_ENABLE  INTEnable( INT_SOURCE_UART_TX( UART1 ), INT_ENABLED )
#define UART1_TX_INT_DISABLE INTEnable( INT_SOURCE_UART_TX( UART1 ), INT_DISABLED )
    
struct UART1_TX_EN
{
    uint8_t busyFlag;
    uint16_t remain;
    uint16_t pos;
    uint8_t buffer[UART1_BUFFER_LEN];
};

struct UART1_RX_EN 
{
    uint16_t length;
    uint8_t buffer[UART1_BUFFER_LEN];
};

struct UART1_TX_EN  uart_tx_en  = {0,0,0,{0}};
struct UART1_TX_EN  *uart1_tx   = &uart_tx_en;
struct UART1_RX_EN  uart_rx_en  = {0,{0}};
struct UART1_RX_EN  *uart1_rx   = &uart_rx_en;

int uart1_data_received_proc(const char *buf, uint16_t len);
void uart1_RX_timeout_proc( void );

/*使用UART1作为调试口*/
void uart1_init( void )
{
    /*重映射UART1端口，TX->RB4, RX->RA4*/
    TRISBbits.TRISB4 = 0;//TX
    TRISAbits.TRISA4 = 1;//RX
    PPSInput(3,U1RX, RPA4);
    PPSOutput(1,RPB4,U1TX);

	/* 配置和使能UART1, 关闭CTS, RTS数据流信号，只启用TX,RX */
	UARTConfigure( UART1, UART_ENABLE_PINS_TX_RX_ONLY );
    
	 /* 设置FIFO中断方式*/
	UARTSetFifoMode( UART1, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY );
    
	/* 设置为数据位8，无校验位，1位停止位 */
	UARTSetLineControl( UART1, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1 );
    
	/* 设置波特率 */
	UARTSetDataRate( UART1, PBCLK, UART1_BAUDRATE );

    /* 配置UART1接收中断 */
    INTEnable( INT_SOURCE_UART_RX( UART1 ), INT_ENABLED );
    //UART1_TX_INT_ENABLE;
        
    INTSetVectorPriority( INT_VECTOR_UART( UART1 ), UART1_INT_PRIOR );
    INTSetVectorSubPriority( INT_VECTOR_UART( UART1 ), UART1_INT_SUB_PRIOR );
    
    /* 使能UART模块，使能发送，使能接收 */
    UARTEnable( UART1, UART_ENABLE_FLAGS( UART_PERIPHERAL | UART_RX | UART_TX ) );  

    /*等待外设准备*/
    while(!UARTTransmitterIsReady(UART1));
    
    /*注册一个10ms超时回调*/
    TIMER_RequestTick(uart1_RX_timeout_proc, 10);

    //发送空字节
    //INTClearFlag( INT_SOURCE_UART_TX( UART1 ) );
    //UARTSendDataByte(UART1, 0x00);
}

void uart1_RX_timeout_proc( void )
{

    debug_proc(uart1_rx->buffer, uart1_rx->length);
    
    uart1_rx->length = 0;
    memset(uart1_rx->buffer, 0, UART1_BUFFER_LEN);
    TIMER_Stop(uart1_RX_timeout_proc);
}

void uart1_TX_interrupt_proc( void )
{
    /*首已经发送完了*/
    uart1_tx->pos++;
    uart1_tx->remain--;
    if(uart1_tx->remain > 0)
    {
        UARTSendDataByte(UART1, uart1_tx->buffer[uart1_tx->pos]);
    }
    else
    {
        UART1_TX_INT_DISABLE;//关闭发送中断
        uart1_tx->busyFlag = 0;
        //while ( !UARTTransmissionHasCompleted( UART1 ) );
    }
    INTClearFlag( INT_SOURCE_UART_TX( UART1 ) );
}

void uart1_RX_interrupt_proc( void )
{
    //while ( INTGetFlag( INT_SOURCE_UART_RX( UART1 ) ) )
    {
        if (uart1_rx->length < UART1_BUFFER_LEN)
        {
            uart1_rx->buffer[uart1_rx->length++] = UARTGetDataByte( UART1 );
        }
        else  
        {
            UARTGetDataByte( UART1 );
        }
    }
    /*重新设置10ms超时*/
    TIMER_ResetCount(uart1_RX_timeout_proc);
    TIMER_Start(uart1_RX_timeout_proc );
    INTClearFlag( INT_SOURCE_UART_RX( UART1 ) );
}

void uart1_sendData( const uint8_t *data, uint16_t length )
{
    if ( length == 0 || length > UART1_BUFFER_LEN )
    {
        return;
    }
    if(uart1_tx->remain == 0)
    {
        uart1_tx->busyFlag = 0;
    }
            
    if (uart1_tx->busyFlag)
    {
        uint16_t bufferLen = uart1_tx->pos + uart1_tx->remain;
        if(length + bufferLen <= UART1_BUFFER_LEN) //承担不住的丢掉
        {
            UART1_TX_INT_DISABLE;// 关掉中断
            uart1_tx->remain += length;
            memcpy(uart1_tx->buffer + bufferLen, data, length);
            UART1_TX_INT_ENABLE;// 使能中断
        }
        return;
    }
    else
    {
        uart1_tx->pos       = 0;
        uart1_tx->busyFlag  = 1;
        uart1_tx->remain    = length;
        memcpy( uart1_tx->buffer, data, length );
        UART1_TX_INT_ENABLE;
        /*开始新的发送*/
        UARTSendDataByte(UART1, uart1_tx->buffer[uart1_tx->pos]);
    }
}

void __ISR( _UART_1_VECTOR, ipl5) _UART1_INT_handle( void )
{
    
	if( INTGetFlag( INT_SOURCE_UART_RX( UART1 ) ) )
	{
        uart1_RX_interrupt_proc();        
	}
	else if( INTGetFlag( INT_SOURCE_UART_TX( UART1 ) ) )
	{
        uart1_TX_interrupt_proc();
	}
}
