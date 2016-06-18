#include "io430.h"
#include <stdint.h>

#include "a7339.h"
#include "common.h"
//#include "Ta_uart.h"
#define MSP430_F149

#define     KEY_IO      BIT3 //p1.3

#define     LED_IO      BIT6 //p1.6
#define     LED_ON()    (P1OUT &= ~LED_IO)
#define     LED_OFF()   (P1OUT |= LED_IO)
void USART_Init(void)
{
  P3SEL |= 0x30;                            // P3.4,5 = USART0 TXD/RXD
  ME1 |= UTXE0 + URXE0;                     // Enable USART0 TXD/RXD
  UCTL0 |= CHAR;                            // 8-bit character
  UTCTL0 |= SSEL0;                          // UCLK = ACLK
  UBR00 = 0x03;                             // 32k/9600 - 3.41
  UBR10 = 0x00;                             //
  UMCTL0 = 0x4A;                            // Modulation
  UCTL0 &= ~SWRST;                          // Initialize USART state machine
   // IE1 |= URXIE0;                            // ???????§Ø? 
}

void USART_SendByte(uint8_t ch)
{
    TXBUF0 = ch;
    while (!(IFG1 & UTXIFG0));                // TX???????§µ?       
}

int main( void )
{
    WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer
  /*  
    if (CALBC1_1MHZ==0xFF)					// If calibration constants erased
    {											
      while(1);                             // do not load, trap CPU!!	
    }*/
   // DCOCTL = 0;                             // Select lowest DCOx and MODx settings
  //  BCSCTL1 = CALBC1_1MHZ;
   // DCOCTL = CALDCO_1MHZ;

	//TimerA_UART_init();
    P1DIR |= LED_IO;
	LED_OFF();
	USART_Init();
  /*  
   // P1DIR &= ~KEY_IO;                            // Set P1.0 to output direction
    P1IE |= KEY_IO;                             // P1.4 interrupt enabled
    P1IES |= KEY_IO;                            // P1.4 Hi/lo edge
    P1IFG &= ~KEY_IO;                           // P1.4 IFG cleared
*/
  //__bis_SR_register(LPM1_bits + GIE);       // Enter LPM4 w/interrupt
	P1DIR &= ~BIT3; //f149 key
	USART_SendByte('s');
    while(1)
    {

        if(!(P1IN&BIT3))
            {
                delay_ms(50);
                if(!(P1IN&BIT3))
                {
               		 USART_SendByte('a');
                	LED_ON();
					InitRF_M(); //init RF
					WriteFIFO();	//write data to TX FIFO
					StrobeCMD(CMD_TX);
					delay_us(10);//Delay10us(1);
					while(GIO1);		//wait transmit completed
					delay_ms(500);
					LED_OFF();

                    
                }
            }

    }
}

// Port 1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not supported!
#endif
{
    if(P1IFG & KEY_IO)
    {
        P1IFG &= ~KEY_IO;                           // IFG cleared
        LED_ON();
        __bic_SR_register_on_exit(LPM1_bits);   // Clear LPM4 bits from 0(SR)
    }
}
