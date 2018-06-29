#ifndef __UART1_H__
#define __UART1_H__
void uart1_init( void );

void uart1_sendData(const uint8_t *data, uint16_t length );
#endif 
