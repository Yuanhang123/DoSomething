#include "io430.h"
#include "a7201/kt403.h"
#include "a7201/a7201.h"
#include "a7201/common.h"
#include "a7201/Ta_uart.h"
#include "a7201/a7201.h"
#include "a7201/key.h"
#include "a7201/work.h"


void key_init(void);
void receiver_HW_test();
void uart1_init(void);
void send_cmd();

#define		IO_HW_CE		BIT1
#define		IO_HW_S 		BIT0
#define		IO_HW_AFSK		BIT5
#define		IO_HW_BAND		BIT4
int rx_data();

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

   // uart1_init();
   TimerA_UART_init();
    //receiver_HW_test();
    //kt403_init();
    key_init();

	A7201_init();
	ReceiveData();
    P1DIR |= LED_IO;
    P2DIR |= BIT2;
    while(1)
    {
		//LED_ON();
		key_polling();
		setup_normal();
    	work_main();
	 	delay_ms(10);
       // LED_OFF();
       // delay_ms(1000);
    
    }
  //return 0;
}
/*
void uart1_init(void)
{

    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 1MHz 9600
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  
  //IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

 // __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
}*/
/*
void USART_SendByte(uint8_t ch)
{

    while (!(IFG2 & UCA0TXIFG));         // USCI_A0 TX   
    UCA0TXBUF = ch;
   // while (!(IFG1 & UTXIFG0));                // TX???????§µ?       
}

*/
void receiver_HW_test()
{
    P2DIR |=  IO_HW_CE + IO_HW_S;
    P1DIR |=  IO_HW_AFSK + IO_HW_BAND + LED_IO;
    
    P2OUT &= ~(IO_HW_CE + IO_HW_S);
    P1OUT |=  IO_HW_AFSK + IO_HW_BAND;
    delay_ms(50);
    /*
    for(;;)
    {P2OUT |= IO_HW_CE;
        P1OUT |= LED;
        delay_ms(500);
        P1OUT &= ~LED;
        delay_ms(500);
    }*/
}