#include "io430.h"

#include "a7339.h"
#include "common.h"
//#include "Ta_uart.h"
#define MSP430_F149

#define     KEY_IO      BIT5 //p1.5

#define     LED_IO      BIT6 //p1.6
#define     LED_ON()    (P1OUT &= ~LED_IO)
#define     LED_OFF()   (P1OUT |= LED_IO)


int main( void )
{
    WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer
    
    if (CALBC1_1MHZ==0xFF)					// If calibration constants erased
    {											
      while(1);                             // do not load, trap CPU!!	
    }
    DCOCTL = 0;                             // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    
    P2DIR |= LED_IO;

	LED_OFF();
	P1DIR &= ~KEY_IO; // key
	
	InitRF_M(); //init RF
	while(1)
    {
        delay_ms(1000);
        if (1)//((P1IN & KEY_IO)==0)
        {
            for(int i = 0;i<3;i++){
                LED_ON();
              WriteFIFO();	//write data to TX FIFO
              StrobeCMD(CMD_TX);
                delay_us(10);
                while(GIO1);		//wait transmit completed
                delay_ms(100);
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
