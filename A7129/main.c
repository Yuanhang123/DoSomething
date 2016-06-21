#include "io430.h"
#include <stdint.h>
#include "libraries\a7129.h"
#include "libraries\common.h"
#include "libraries\Ta_uart.h"

void key_init(void);
void receiver_HW_test();
void uart1_init(void);
void send_cmd();

#define		LED     		BIT0
#define		IO_HW_CE		BIT1
#define		IO_HW_S 		BIT0
#define		IO_HW_AFSK		BIT5
#define		IO_HW_BAND		BIT4
int rx_data();
extern unsigned int   BitErrors;
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
  // TimerA_UART_init();
    //receiver_HW_test();
    //kt403_init();
    //key_init();
	//TimerA_UART_print("uart ok");

    P1DIR |= LED;

	InitRF();
    while(1)
    {

		RxPacket();
		 
		//y_polling();
    	//rk_main();
	 	delay_ms(500);
    
    }
  //return 0;
}

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
}

void USART_SendByte(uint8_t ch)
{

    while (!(IFG2 & UCA0TXIFG));         // USCI_A0 TX   
    UCA0TXBUF = ch;
   // while (!(IFG1 & UTXIFG0));                // TX???????§µ?       
}
const unsigned char  PN9_TAB_t[]=
{
    0xFF,0x83,0xDF,0x17,0x32,0x09,0x4E,0xD1,
    0xE7,0xCD,0x8A,0x91,0xC6,0xD5,0xC4,0xC4,
    0x40,0x21,0x18,0x4E,0x55,0x86,0xF4,0xDC,
    0x8A,0x15,0xA7,0xEC,0x92,0xDF,0x93,0x53,
    0x30,0x18,0xCA,0x34,0xBF,0xA2,0xC7,0x59,
    0x67,0x8F,0xBA,0x0D,0x6D,0xD8,0x2D,0x7D,
    0x54,0x0A,0x57,0x97,0x70,0x39,0xD2,0x7A,
    0xEA,0x24,0x33,0x85,0xED,0x9A,0x1D,0xE0
};

const unsigned char  PREMABLE_CODE_t[] =
{
    0xAA,0xAA,0xAA,0xC0,0xAA,0xAA,0xAA,0xC0
};

const unsigned char  ID_CODE_t[] =
{
    0x76,0xC3,0x5A,0x6B
};

void send_cmd()
{
    int DataIndex =0;
    for(DataIndex = 0;DataIndex < 8;DataIndex++)
    {
        USART_SendByte(PREMABLE_CODE_t[ DataIndex ]);
    }
    for(DataIndex = 0;DataIndex < 4;DataIndex++)
    {
        USART_SendByte(ID_CODE_t[ DataIndex ]);
    }
    for(DataIndex = 0;DataIndex < 64;DataIndex++)
    {
        USART_SendByte(PN9_TAB_t[ DataIndex ]);
    }

}


void receiver_HW_test()
{
    P2DIR |=  IO_HW_CE + IO_HW_S;
    P1DIR |=  IO_HW_AFSK + IO_HW_BAND + LED;
    
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