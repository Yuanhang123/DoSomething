#include "io430.h"
#include <stdint.h>

#include "a7339.h"
#include "common.h"
//#include "Ta_uart.h"
#define MSP430_F149

#define     KEY_IO      BIT3 //p1.3

#define     LED_IO      BIT7 //p1.6
#define     LED_ON()    (P2OUT &= ~LED_IO)
#define     LED_OFF()   (P2OUT |= LED_IO)
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
   // IE1 |= URXIE0;                            // ???????ж? 
}

void USART_SendByte(uint8_t ch)
{
    TXBUF0 = ch;
    while (!(IFG1 & UTXIFG0));                // TX???????У?       
}
void UART_print(char *string)
{
    while (*string) {
        USART_SendByte(*string++);
    }
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
    /*------选择系统主时钟为8MHz-------*/
    BCSCTL1 &= ~XT2OFF;                 //打开XT2高频晶体振荡器
    do
    {
        IFG1 &= ~OFIFG;                 //清除晶振失败标志
        for (int i = 0xFF; i > 0; i--);     //等待8MHz晶体起振
    }
    while ((IFG1 & OFIFG));             //晶振失效标志仍然存在？
    BCSCTL2 |= SELM_2 + SELS;           //MCLK和SMCLK选择高频晶振
    P2DIR |= LED_IO;
	USART_Init();
    UART_print("uart OK");
	LED_OFF();
	P1DIR &= ~BIT3; //f149 key
	InitRF_M(); //init RF
	while(1)
    {
/*
        if(!(P1IN&BIT3))
        {
                delay_ms(50);
                
                if(!(P1IN&BIT3))
                {*/
       		LED_ON();
			WriteFIFO();	//write data to TX FIFO
			StrobeCMD(CMD_TX);
			delay_us(10);
			//while(GIO1);		//wait transmit completed

			delay_ms(500);
			LED_OFF();
			dump_a7339reg();
			delay_ms(1000);

/*
                    
                }
          }
*/

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
