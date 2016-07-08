#include "io430.h"
#include "a7201.h"
#include "Ta_uart.h"
#include "common.h"

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

unsigned char  bBufIndex;
unsigned char  bDataRedly;

UINT8  RxData[4];
UINT8  RxDataBuf[4];

const unsigned char  FREQ_TAB[2][2] =
{   { 0x70, 0x03 },         // 433.92MHz
    { 0x74, 0x63 },         // 314.846MHz
};


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
    SPI_DATA_H();
    SpiWrite( 0x70, 0x03);
    SPI_DATA_H();
}

void SetMode()
{
    SPI_DATA_H();
    SpiWrite( Reg1_H, Reg1_L );
    SPI_DATA_H();
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
		Reg1_L |= FASK_L;	//[0]: ASK.[1]: FSK (default).
        SetRF();
        //Delay1ms_CNT50US( 10 );
        delay_ms(1);
		SPI_CE_H();
        Reg1_L &= ~CE_L;	//[0]: Active mode.[1]: Shut down mode (default).
       // Reg1_L |= FASK_L;//bFASK = ASK;  //for ASK Mode, Promise AGC working
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


//------------------------------------------------------------------------------
// Timer_A UART - Receive Interrupt Handler
//------------------------------------------------------------------------------




/*****************************************************************************/
/*     FE        |    CC   | 	*/
/* _______    __     __     __   		*/
/*             \_/  \__/  \__/    __  __------*/
/**/
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer_A1_ISR(void)
{
    static UINT8 bRxBitCnt = 8;
    static UINT8 bRxData = 0;
   	static UINT8 bRxsta = IDLE,bMatchCnt = 0,bRxIndex = 0;
   	static UINT16 PreambleBit = SCCI;
   
    switch (__even_in_range(TA0IV, TA0IV_TAIFG)) { // Use calculated branching
        case TA0IV_TACCR1:                        // TACCR1 CCIFG - UART RX
            
            TACCR1 += UART_TBIT;                 // Add Offset to CCRx
           // P2OUT &= ~BIT2;
            if (TACCTL1 & CAP) {                 // Capture mode = start bit edge
                TACCTL1 &= ~CAP;                 // Switch capture to compare mode
                TACCR1 += UART_TBIT_DIV_2;       // Point CCRx to middle of D0
            }
            else 
            {
				switch(bRxsta) {
					case IDLE:
						if ((TACCTL1 & SCCI) == (PreambleBit & SCCI)) { 
							PreambleBit = ~PreambleBit;
                            
							if(bMatchCnt++ >= MATCH_CNT) {
								TACCR1 += UART_TBIT_BYTE;
								bRxsta = PREAMBLE;
							}
                          //  if(int data < bMatchCnt)
                           //     data = bMatchCnt;
						} else {
							PreambleBit = SCCI;
							bRxsta = PREAMBLE;//TACCTL1 |= CAP; 			 // Switch compare to capture mode
						}
                        TACCR1 += UART_TBIT;
						break;
						
					case SYNC_OK:
						bRxData <<= 1;
		                if (TACCTL1 & SCCI) {            // Get bit waiting in receive latch
		                    bRxData |= 0x01;
		                }
		                bRxBitCnt--;
		                if (bRxBitCnt == 0) {             // All bits RXed?
		                	bRxBitCnt = 8;				 // Re-load bit counter
		                	
		                	RxDataBuf[bBufIndex] = bRxData;
							
							if(++bRxIndex >= DATA_LEN) {
								bRxIndex = 0;
                                bRxsta = IDLE;
                            }
							if( ++bBufIndex > DATA_BUF_LEN) {
								bBufIndex = 0;
								bDataRedly = TRUE;
							}
		                }
						break;
						
					case PREAMBLE:
						if(bMatchCnt >= MATCH_CNT)
							bRxsta = SYNC_OK;
						else
							bRxsta = IDLE;
						bMatchCnt = 0;
						TACCTL1 |= CAP;              // Switch compare to capture mode
						break;

				}
                   // __bic_SR_register_on_exit(LPM0_bits);  // Clear LPM0 bits from 0(SR)
          }
     }
	//P2OUT |= BIT2;
}
#define		MATCH_OK		0xFE



UINT8 compare_data(UINT8 *src,UINT8 *data)
{
	for(int i=0;i<4;i++)
	{
		if(src[i] != data[i])
			return i;	
	}
	return MATCH_OK;
}
UINT8 match_data(UINT8 *data_index)
{
	UINT8 Err_index = MATCH_OK;
	UINT8 Ret = FALSE;
	//UINT8 ReBufTemp[12] = {0};
	if(bDataRedly) {
		delay_ms(10);//
		bDataRedly = FALSE;
		Err_index = compare_data(&RxDataBuf[DATA0_ADDR],&RxDataBuf[DATA1_ADDR]);
		if(Err_index == MATCH_OK) {
			*data_index = DATA0_ADDR;
		} else {
			if(RxDataBuf[Err_index] == RxDataBuf[Err_index+DATA2_ADDR]) {
				*data_index = DATA0_ADDR;
			} else {
				*data_index = DATA1_ADDR;
			}
		}
		Ret = TRUE;
	}
	return Ret;
}
/*
55 55 55 55 00 00 ff xx
*/
