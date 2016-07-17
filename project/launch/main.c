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
    P1DIR = 0xff;
    P2DIR = 0xff;
    P1DIR = 0xff;
    P2DIR = 0xff;
    /****/
      P1IE |= 0x20;                             // P1.4 interrupt enabled
  P1IES |= 0x20;                            // P1.4 Hi/lo edge
  P1IFG &= ~0x20;                           // P1.4 IFG cleared
        /***/
    P2DIR |= LED_IO;
	LED_OFF();
	P1DIR &= ~KEY_IO; // key
	
	InitRF_M(); //init RF
    StrobeCMD(CMD_DEEP_SLEEP);
	while(1)
    {
        __bis_SR_register(LPM4_bits + GIE);     // Enter LPM3
        if((P1IN & KEY_IO)==0)
        {   InitRF_M(); //init RF
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
        StrobeCMD(CMD_DEEP_SLEEP);
        delay_ms(1);

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
  P1IFG &= ~0x20;                           // P1.4 IFG cleared
  __bic_SR_register_on_exit(LPM4_bits);     // Clear LPM3 bits from 0(SR)
}