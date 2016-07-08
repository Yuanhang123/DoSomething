#include "io430.h"

#include <stdint.h>
#include "uart.h"


void USART_SendByte(uint8_t ch)
{
	while (!(IFG2 & UCA0TXIFG));         // USCI_A0 TX   
    UCA0TXBUF = ch;                    // TX -> RXed character
    
    
}

void USART_Init(void)
{
    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 1MHz 9600
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    //IE2 |= UCA0TXIE;                          // Enable USCI_A0 TX interrupt
    
}
