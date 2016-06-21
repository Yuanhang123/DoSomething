#include "io430.h"
#include "a7201.h"
#include "Ta_uart.h"


#define		SPI_CLK_IO		BIT6
#define		SPI_DATA_IO		BIT5	
#define		SPI_CE_IO		BIT1	//p1.7
#define		SPI_STB_IO		BIT4 	//p2.7
#define		SPI_S_IO		BIT0 	//p2.6

#define		RX_DATA_IO		BIT2


#define		SPI_CLK_L()			(P1OUT &= ~SPI_CLK_IO)
#define		SPI_CLK_H()			(P1OUT |= SPI_CLK_IO)

#define		SPI_DATA_L()		(P1OUT &= ~SPI_DATA_IO)
#define		SPI_DATA_H()		(P1OUT |= SPI_DATA_IO)

#define		SPI_CE_L()			(P2OUT &= ~SPI_CE_IO)
#define		SPI_CE_H()			(P2OUT |= SPI_CE_IO)

#define		SPI_STB_L()			(P1OUT &= ~SPI_STB_IO)
#define		SPI_STB_H()			(P1OUT |= SPI_STB_IO)

#define		SPI_S_L()			(P2OUT &= ~SPI_S_IO)
#define		SPI_S_H()			(P2OUT |= SPI_S_IO)

#define		SPI_delay(t)		//delay_us(t)



void initUart1(void);
//void ReceiveData(void);
//void SpiWrite( unsigned char spidataH, unsigned char spidataL );
void SetCH();
void SetMode();
void SetRF();

//unsigned char  BitBuffer;
unsigned char  Reg1_H;
unsigned char  Reg1_L;

unsigned char  RF_Flag;
/*
    sbit bPremable   = RF_Flag ^ 6;
    sbit bSynWord    = RF_Flag ^ 5;
    sbit bStart      = RF_Flag ^ 3;
*/

unsigned char  DataIndex;
unsigned char  RxIndex;

unsigned int   BitErrors;
unsigned char  RxData[4];
unsigned char  RxDataBuf[72];

const unsigned char  WAIT_TAB[ 16 ] =
{
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
};

const unsigned char  PN9_TAB[]=
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

const unsigned char  ID_CODE[] =
{
    0x76,0xC3,0x5A,0x6B
};

const unsigned char  FREQ_TAB[2][2] =
{   { 0x70, 0x03 },         // 433.92MHz
    { 0x74, 0x63 },         // 314.846MHz
};


/*
void initUart1(void)
{
    TMOD &= 0x0F;
    TMOD |= 0x20;       // timer 1 = 8bit auto-reload
    TL1 = ( 256 - 1 );
    TH1 = ( 256 - 1 );  // set UART timer (only timer1), 8M/12/32 = 20.83KHz

    TR1 = 1;            // start timer 1

    SM0_1 = 0;
    SM1_1 = 1;          // set UART mode 1
    REN_1 = 0;
    ES1 = 0;            // disable UART interrupt
}
*/
/*
void SpiWrite( unsigned char spidataH, unsigned char spidataL )
{
    unsigned char data i;

    pSPISTB = 1;
    BitBuffer = spidataL;
    for ( i = 0; i < 8; i++ )
    {
        pSPIDATA = bBitBuffer0;
        pSPICLK = 1;
        BitBuffer = BitBuffer >> 1;
        pSPICLK = 0;
    }

    BitBuffer = spidataH;
    for ( i = 0; i < 8; i++ )
    {
        pSPIDATA = bBitBuffer0;
        pSPICLK = 1;
        BitBuffer = BitBuffer >> 1;
        pSPICLK = 0;
    }
    pSPISTB = 0;
}
*/

void SPI_IO_Init(void)
{
   P1DIR |= SPI_CLK_IO + SPI_DATA_IO + SPI_STB_IO;
   P2DIR |=	SPI_CE_IO + SPI_S_IO;

   //CS_1;
   SPI_CLK_L();
   delay_ms(10);
   
}

void SpiWrite( unsigned char spidataH, unsigned char spidataL )
{
    unsigned char BitBuffer, i;

    SPI_STB_H();
    BitBuffer = spidataL;
    for ( i = 0; i < 8; i++ )
    {
		SPI_CLK_L();
        if(0x01 & BitBuffer)
			SPI_DATA_H();
		else
			SPI_DATA_L();
		
		SPI_delay(1);
        SPI_CLK_H();
		SPI_delay(1);
        BitBuffer >>= 1;
        
    }

    BitBuffer = spidataH;
    for ( i = 0; i < 8; i++ )
    {
		SPI_CLK_L();
        if(0x01 & BitBuffer)
			SPI_DATA_H();
		else
			SPI_DATA_L();
		
		SPI_delay(1);
        SPI_CLK_H();
		SPI_delay(1);
        BitBuffer >>= 1;
        
    }
	SPI_CLK_L();
	SPI_STB_L();

}

void SetCH()
{
   // pDATAIN = 1;
    SPI_DATA_H();
    SpiWrite( FREQ_TAB[0][0], FREQ_TAB[0][1] );
    SPI_DATA_H();
  //  pDATAIN = 1;
}

void SetMode()
{
   // pDATAIN = 1;
    SPI_DATA_H();
    SpiWrite( Reg1_H, Reg1_L );
    SPI_DATA_H();
   // pDATAIN = 1;
}

void SetRF()
{
    SetCH();
    SetMode();
}

void ReceiveData(void)
{
 //   if ( ES1 == 0 )
//    {
        Reg1_L |= CE_L;	//[0]: Active mode.[1]: Shut down mode (default).
		Reg1_L &= ~FASK_L;	//[0]: ASK.[1]: FSK (default).
        SetRF();
        //Delay1ms_CNT50US( 10 );
        delay_ms(10);
		SPI_CE_H();
        Reg1_L &= ~CE_L;	//[0]: Active mode.[1]: Shut down mode (default).
        Reg1_L |= FASK_L;//bFASK = ASK;  //for ASK Mode, Promise AGC working
        SetRF();
        //REN_1 = 1;
     //   ES1 = 1;        // start UART1 interrupt
  //  }
}

void A7201_init(void)
{

	SPI_IO_Init();
    SPI_CE_L();	//pCE = 0;
    			//pTX_RX = 0;
   	SPI_S_H(); //pSPIS = 1;           // SPI mode
    SPI_STB_L();
    SPI_CLK_L();
	
    RF_Flag = 0;
    DataIndex = 0;
    BitErrors = 0;
 
    Reg1_H = 0x77;
    Reg1_L = 0xA9;          // RX, FSK, HiBand, Clock out disable, Up side band, TX power max, AGC enable
    
	Reg1_L &= (~BAND_L);	//HI_BAND	// HI_BAND = 434MHz band, LO_BAND = 315MHz band
	Reg1_L |= FASK_L;	//FSK	//1:FSK or 0 ASK
	Reg1_L |= IFB0_L;	//IF filter bandwidth selection[mid. (ASK = 250KHz, FSK = 150KHz)]
	Reg1_H |= IFB1_H;

	SpiWrite ( 0xFA, 0x32 ); // For Power On(Initial)
   // initUart1();            // 8M/12/32 = 20.8K bps
/*
    while(1)
    {
        ReceiveData();
    }
	*/
}
unsigned char bSynWord = 0;
void A7201_Process(UINT8 date);
int rx_data()
{
	if(RxIndex > 70)
	{	
		P1OUT |= BIT0;
		for(UINT8 i=0; i<70; i++)
		{
			A7201_Process(RxDataBuf[i]);

		}
		delay_ms(10);
		RxIndex = 0;

            bSynWord = 0;
            DataIndex = 0;
			//TimerA_UART_tx(BitErrors);
            //ES1 = 0;        // disable UART1 interrupt

		P1OUT &= ~BIT0;
	}
	
	return BitErrors;
}
void A7201_Process(UINT8 date)
{
    unsigned char RfTmp, i, tmep;
	//unsigned char DataBuf;

    if ( bSynWord == 0 )
    {
        RxData[ 0 ] = RxData[ 1 ];
        RxData[ 1 ] = RxData[ 2 ];
        RxData[ 2 ] = RxData[ 3 ];
        RxData[ 3 ] = date;

        RfTmp = 0;
        for ( i = 0; i < 4; i++ )
        {
            tmep = RxData[ i ] ^ ID_CODE[ i ];
            RfTmp += ( WAIT_TAB[ tmep >> 4 ] + WAIT_TAB[ tmep & 0x0F ] );
        }

        if ( RfTmp <= 4 )
        {
            DataIndex = 0;
            bSynWord = 1;
        }
    }
    else
    {
        DataIndex++;

        tmep = date ^ PN9_TAB[ DataIndex - 1 ];
        RfTmp = ( WAIT_TAB[ tmep >> 4 ] + WAIT_TAB[ tmep & 0x0F ] );
        BitErrors += RfTmp;


    }

}

/*
//------------------------------------------------------------------------------
// Timer_A UART - Receive Interrupt Handler
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A1_VECTOR//#pragma vector = TIMERA1_VECTOR
__interrupt void Timer_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMERA1_VECTOR))) Timer_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    static unsigned char rxBitCnt = 8;
    static unsigned char rxData = 0;

    switch (__even_in_range(TA0IV, TA0IV_TAIFG)) { //switch (__even_in_range(TAIV, TAIV_TAIFG)) { // Use calculated branching
    case TA0IV_TACCR1://TAIV_TACCR1:                        // TACCR1 CCIFG - UART RX
            TACCR1 += UART_TBIT;                 // Add Offset to CCRx
            if (TACCTL1 & CAP) {                 // Capture mode = start bit edge
                TACCTL1 &= ~CAP;                 // Switch capture to compare mode
                TACCR1 += UART_TBIT_DIV_2;       // Point CCRx to middle of D0
            }
            else {
                rxData >>= 1;
                if (TACCTL1 & SCCI) {            // Get bit waiting in receive latch
                    rxData |= 0x80;
                }
                rxBitCnt--;
                if (rxBitCnt == 0) {             // All bits RXed?
                    rxBuffer = rxData;           // Store in global variable
                    A7201_Process(rxData);
                    rxBitCnt = 8;                // Re-load bit counter
                    TACCTL1 |= CAP;              // Switch compare to capture mode
                   // __bic_SR_register_on_exit(LPM0_bits);  // Clear LPM0 bits from 0(SR)
                }
            }
            break;
    }
}*/
//--------------------------------------------------------


//------------------------------------------------------------------------------
// Timer_A UART - Receive Interrupt Handler
//------------------------------------------------------------------------------
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer_A1_ISR(void)
{
    static unsigned char rxBitCnt = 8;
    static unsigned char rxData = 0;
   static UINT8 i= 0;
    switch (__even_in_range(TA0IV, TA0IV_TAIFG)) { // Use calculated branching
        case TA0IV_TACCR1:                        // TACCR1 CCIFG - UART RX
            TACCR1 += UART_TBIT;                 // Add Offset to CCRx
            if (TACCTL1 & CAP) {                 // Capture mode = start bit edge
                TACCTL1 &= ~CAP;                 // Switch capture to compare mode
                TACCR1 += UART_TBIT_DIV_2;       // Point CCRx to middle of D0
            }
            else {
                rxData >>= 1;
                if (TACCTL1 & SCCI) {            // Get bit waiting in receive latch
                    rxData |= 0x80;
                }
                rxBitCnt--;
                if (rxBitCnt == 0) {             // All bits RXed?
                   //rxBuffer = rxData;           // Store in global variable
					if(RxIndex < 72)
					{
						RxDataBuf[RxIndex++] = rxData;
					}
                    //A7201_Process(rxData);
                    rxBitCnt = 8;                // Re-load bit counter
                    TACCTL1 |= CAP;              // Switch compare to capture mode
                    __bic_SR_register_on_exit(LPM0_bits);  // Clear LPM0 bits from 0(SR)
                }
            }
            break;
    }
}

