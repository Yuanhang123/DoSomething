#ifndef _UART_H_
#define _UART_H_
#include "common.h"

//------------------------------------------------------------------------------
// Conditions for 9600 Baud SW UART, SMCLK = 1MHz
//------------------------------------------------------------------------------
#define UART_TBIT_DIV_2     (CPU_CLOCK / (9600 * 2))
#define UART_TBIT           (CPU_CLOCK / 9600)


//------------------------------------------------------------------------------
// Hardware-related definitions
//------------------------------------------------------------------------------
#define UART_TXD   0x02                     // TXD on P1.1 (Timer0_A.OUT0)
#define UART_RXD   0x04                     // RXD on P1.2 (Timer0_A.CCI1A)


void TimerA_UART_init(void);

void TimerA_UART_tx(unsigned char byte);

extern unsigned char rxBuffer;

#endif
