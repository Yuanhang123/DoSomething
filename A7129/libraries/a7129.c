#include "io430.h"
#include "A7129.h"
#include "A7129config.h"
#include "Ta_uart.h"



/*********************************************************************
**  Global Variable Declaration
*********************************************************************/
UINT8       timer;
UINT8       TimeoutFlag;
UINT16     RxCnt;
UINT32     Err_ByteCnt;
UINT32     Err_BitCnt;
UINT16     TimerCnt0;
UINT8       *Uartptr;
UINT8       UartSendCnt;
UINT8       CmdBuf[11];
UINT8      tmpbuf[64];

const UINT8 BitCount_Tab[16]={0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
const UINT8 ID_Tab[8]={0x34,0x75,0xC5,0x8C,0xC7,0x33,0x45,0xE7};   //ID code
const UINT8 PN9_Tab[]=
{   0xFF,0x83,0xDF,0x17,0x32,0x09,0x4E,0xD1,
    0xE7,0xCD,0x8A,0x91,0xC6,0xD5,0xC4,0xC4,
    0x40,0x21,0x18,0x4E,0x55,0x86,0xF4,0xDC,
    0x8A,0x15,0xA7,0xEC,0x92,0xDF,0x93,0x53,
    0x30,0x18,0xCA,0x34,0xBF,0xA2,0xC7,0x59,
    0x67,0x8F,0xBA,0x0D,0x6D,0xD8,0x2D,0x7D,
    0x54,0x0A,0x57,0x97,0x70,0x39,0xD2,0x7A,
    0xEA,0x24,0x33,0x85,0xED,0x9A,0x1D,0xE0
};  // This table are 64bytes PN9 pseudo random code.


void Delay10us(UINT8 n)
{
    UINT8 i;
	
	while(n--)
	{
        for(i=0; i<2; i++);
	}
}
void SPI_IO_Init(void)
{
	P1DIR |= SPI_DATA_IO + SPI_SCS_IO + SPI_CLK_IO;
	P1DIR &=	~GIO2_IO;
	P1OUT |= SPI_DATA_IO + SPI_SCS_IO + SPI_CLK_IO;
   //CS_1;
	SPI_CLK_L();
	delay_ms(1);
   
}

void ByteSend(UINT8 src)
{
    UINT8 i;
	SPI_DATA_OUT();

    for(i=0; i<8; i++)
    {
		if(src & 0x80)
		    SPI_DATA_H();//SDIO = 1;
		else
		    SPI_DATA_L();//SDIO = 0;

		SPI_delay(1);// _nop_();
		SPI_CLK_H();// SCK=1;
		SPI_delay(1);// _nop_();
		SPI_CLK_L();//SCK=0;
		src<<=1;
    }
}

UINT8 ByteRead(void)
{
	UINT8 i,tmp;
	//read data code
	SPI_DATA_H();//=1;		//SDIO pull high
	SPI_DATA_IN();
	for(i=0; i<8; i++)
	{
	  if(SPI_DATA)
	      tmp = (tmp << 1) | 0x01;
	  else
	      tmp = tmp << 1;

	  SPI_CLK_H();// SCK=1;
	  SPI_delay(1);// _nop_();
	  SPI_CLK_L();//SCK=0;
	}
	return tmp;
}

void StrobeCMD(UINT8 cmd)
{
    SPI_SCS_L();//SPI_SCS_L();
	ByteSend(cmd);
	SPI_SCS_H();//SCS=1;
}

void A7129_WriteReg(UINT8 address, UINT16 dataWord)
{
    UINT8 i;

    SPI_DATA_OUT();
	SPI_SCS_L();//SPI_SCS_L();
    address |= CMD_Reg_W;
    for(i=0; i<8; i++)
    {
        if(address & 0x80)
           	SPI_DATA_H();
        else
            SPI_DATA_L();

		SPI_CLK_H();// SCK=1;
		SPI_delay(1);// _nop_();
		SPI_CLK_L();//SCK=0;

        address<<=1;
    }
    SPI_delay(1);// _nop_();
    //send data word
    for(i=0; i<16; i++)
    {
        if(dataWord & 0x8000)
            SPI_DATA_H();//SDIO = 1;
        else
            SPI_DATA_L();//SDIO = 0;
		SPI_CLK_H();// SCK=1;
		SPI_delay(1);// _nop_();
		SPI_CLK_L();//SCK=0;

        dataWord<<=1;
    }
    SPI_SCS_H();//SCS=1;
}

/************************************************************************
**  ReadReg
************************************************************************/
UINT16 A7129_ReadReg(UINT8 address)
{
    UINT8 i;
    UINT16 tmp;

    SPI_DATA_OUT();
	SPI_SCS_L();//SCS=0;
    address |= CMD_Reg_R;
    for(i=0; i<8; i++)
    {
        if(address & 0x80)
            SPI_DATA_H();
        else
            SPI_DATA_L();

		SPI_delay(1);// _nop_();
 		SPI_CLK_H();// SCK=1;
		SPI_delay(1);// _nop_();
		SPI_CLK_L();//SCK=0;

        address<<=1;
    }
	SPI_DATA_IN();
    SPI_delay(1);// _nop_();

    //read data code
    SPI_DATA_H();//SDIO=1;		//SDIO pull high
    for(i=0; i<16; i++)
    {
        if(SPI_DATA)
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;

 		SPI_CLK_H();// SCK=1;
		SPI_delay(1);// _nop_();
		SPI_CLK_L();//SCK=0;
    }
    SPI_SCS_H();//SCS=1;
    return tmp;
}


/************************************************************************
**  A7129_WritePageA
************************************************************************/
void A7129_WritePageA(UINT8 address, UINT16 dataWord)
{
    UINT16 tmp;

    tmp = address;
    tmp = ((tmp << 12) | A7129Config[CRYSTAL_REG]);
    A7129_WriteReg(CRYSTAL_REG, tmp);
    A7129_WriteReg(PAGEA_REG, dataWord);
}

/************************************************************************
**  A7129_ReadPageA
************************************************************************/
UINT16 A7129_ReadPageA(UINT8 address)
{
    UINT16 tmp;

    tmp = address;
    tmp = ((tmp << 12) | A7129Config[CRYSTAL_REG]);
    A7129_WriteReg(CRYSTAL_REG, tmp);
    tmp = A7129_ReadReg(PAGEA_REG);
    return tmp;
}

/************************************************************************
**  A7129_WritePageB
************************************************************************/
void A7129_WritePageB(UINT8 address, UINT16 dataWord)
{
    UINT16 tmp;

    tmp = address;
    tmp = ((tmp << 7) | A7129Config[CRYSTAL_REG]);
    A7129_WriteReg(CRYSTAL_REG, tmp);
    A7129_WriteReg(PAGEB_REG, dataWord);
}

/************************************************************************
**  A7129_ReadPageB
************************************************************************/
UINT16 A7129_ReadPageB(UINT8 address)
{
    UINT16 tmp;

    tmp = address;
    tmp = ((tmp << 7) | A7129Config[CRYSTAL_REG]);
    A7129_WriteReg(CRYSTAL_REG, tmp);
    tmp = A7129_ReadReg(PAGEB_REG);
    return tmp;
}

/************************************************************************
**  WriteID
************************************************************************/
UINT8 A7129_WriteID(void)
{
	UINT8 i;
	UINT8 d1, d2, d3, d4;

	SPI_SCS_L();//SCS=0;
	ByteSend(CMD_ID_W);
	for(i=0; i<4; i++)
		ByteSend(ID_Tab[i]);
	SPI_SCS_H();//SCS=1;

	SPI_SCS_L();//SCS=0;
	ByteSend(CMD_ID_R);
	d1=ByteRead();
	d2=ByteRead();
	d3=ByteRead();
	d4=ByteRead();
	SPI_SCS_H();//SCS=1;
	
  if((d1!=ID_Tab[0]) || (d2!=ID_Tab[1]) || (d3!=ID_Tab[2]) || (d4!=ID_Tab[3]))
  {
      return 1;
  }
  return 0;
}


/*********************************************************************
** entry_deep_sleep_mode
*********************************************************************/
void entry_deep_sleep_mode(void)
{
    StrobeCMD(CMD_RF_RST);              //RF reset
    A7129_WriteReg(PIN_REG, A7129Config[PIN_REG] | 0x0800);             //SCMDS=1
    A7129_WritePageA(PM_PAGEA, A7129Config_PageA[PM_PAGEA] | 0x0010);   //QDS=1
    StrobeCMD(CMD_SLEEP);               //entry sleep mode
    delay_us(600);                      //delay 600us for VDD_A shutdown, C load=0.1uF
    StrobeCMD(CMD_DEEP_SLEEP);          //entry deep sleep mode
    delay_us(200);                      //delay 200us for VDD_D shutdown, C load=0.1uF
}

/*********************************************************************
** wake_up_from_deep_sleep_mode
*********************************************************************/
void wake_up_from_deep_sleep_mode(void)
{
    StrobeCMD(CMD_STBY);    //wake up
    delay_ms(2);            //delay 2ms for VDD_D stabilized
    //InitRF();
}


/*********************************************************************
** A7129_POR
*********************************************************************/
void A7129_POR(void)
{
	//power on only
    delay_ms(10);   			//for regulator settling time (power on only)
    
    StrobeCMD(CMD_RF_RST);  	//reset A7129 chip
    while(A7129_WriteID())		//check SPI
    {
		StrobeCMD(CMD_RF_RST);  //reset A7129 chip    	
    }
    A7129_WritePageA(PM_PAGEA, A7129Config_PageA[PM_PAGEA] | 0x1000);   //STS=1
    delay_ms(2);
    
    entry_deep_sleep_mode();		//deep sleep
    delay_ms(2);
    wake_up_from_deep_sleep_mode();	//wake up
    
    StrobeCMD(CMD_RF_RST);  	//reset A7129 chip
    while(A7129_WriteID())		//check SPI
    {
		StrobeCMD(CMD_RF_RST);  //reset A7129 chip    	
    }
    A7129_WritePageA(PM_PAGEA, A7129Config_PageA[PM_PAGEA] | 0x1000);   //STS=1
    delay_ms(2);
}


/*********************************************************************
** A7129_Config
*********************************************************************/
UINT8 A7129_Config(void)
{
    UINT8 i;
    UINT16 tmp;

    for(i=0; i<8; i++)
        A7129_WriteReg(i, A7129Config[i]);

    for(i=10; i<16; i++)
        A7129_WriteReg(i, A7129Config[i]);

    for(i=0; i<16; i++)
        A7129_WritePageA(i, A7129Config_PageA[i]);

    for(i=0; i<5; i++)
        A7129_WritePageB(i, A7129Config_PageB[i]);

    //for check        
    tmp = A7129_ReadReg(SYSTEMCLOCK_REG);
    if(tmp != A7129Config[SYSTEMCLOCK_REG])
    {
        return 1;   
    }
    
    return 0;
}


/*********************************************************************
** A7129_Cal
*********************************************************************/
UINT8 A7129_Cal(void)
{
    UINT8 fb, fcd, fbcf;    //IF Filter
    UINT8 vb,vbcf;          //VCO Current
    UINT8 vcb, vccf;        //VCO Band
    UINT16 tmp;
    
    StrobeCMD(CMD_STBY);

    //IF calibration procedure @STB state
    A7129_WriteReg(MODE_REG, A7129Config[MODE_REG] | 0x0802);           //IF Filter & VCO Current Calibration
    do{
        tmp = A7129_ReadReg(MODE_REG);
    }while(tmp & 0x0802);
    
    //for check(IF Filter)
    tmp = A7129_ReadReg(CALIBRATION_REG);
    fb = tmp & 0x0F;
    fcd = (tmp>>11) & 0x1F;
    fbcf = (tmp>>4) & 0x01;
    if(fbcf)
    {
         return 1;
    }

    //for check(VCO Current)
    tmp = A7129_ReadPageA(VCB_PAGEA);
    vcb = tmp & 0x0F;
    vccf = (tmp>>4) & 0x01;
    if(vccf)
    {
         return 1;
    }
    
    
    //RSSI Calibration procedure @STB state
    A7129_WriteReg(ADC_REG, 0x4C00);                                    //set ADC average=64
    A7129_WriteReg(MODE_REG, A7129Config[MODE_REG] | 0x1000);           //RSSI Calibration
    do{
        tmp = A7129_ReadReg(MODE_REG);
    }while(tmp & 0x1000);
    A7129_WriteReg(ADC_REG, A7129Config[ADC_REG]);


    //VCO calibration procedure @STB state
    A7129_WriteReg(PLL1_REG, A7129Config[PLL1_REG]);
    A7129_WriteReg(PLL2_REG, A7129Config[PLL2_REG]);
    A7129_WriteReg(MODE_REG, A7129Config[MODE_REG] | 0x0004);       //VCO Band Calibration
    do{
        tmp = A7129_ReadReg(MODE_REG);
    }while(tmp & 0x0004);
    
    //for check(VCO Band)
    tmp = A7129_ReadReg(CALIBRATION_REG);
    vb = (tmp >>5) & 0x07;
    vbcf = (tmp >>8) & 0x01;
    if(vbcf)
    {
         return 1;
    }
    
    return 0;
}

/*********************************************************************
** A7129_WriteFIFO
*********************************************************************/
void A7129_WriteFIFO(void)
{
    UINT8 i;

    StrobeCMD(CMD_TFR);     //TX FIFO address pointer reset

    SPI_SCS_L();
    ByteSend(CMD_FIFO_W);   //TX FIFO write command
    for(i=0; i <64; i++)
        ByteSend(PN9_Tab[i]);
    SPI_SCS_H();
}

/*********************************************************************
** RxPacket
*********************************************************************/
void RxPacket(void)
{
    UINT8 i;
    UINT8 recv;
    UINT8 tmp;

	StrobeCMD(CMD_RX);
	Delay10us(1);
	while(GIO2);		//wait receive completed

    RxCnt++;

    StrobeCMD(CMD_RFR);     //RX FIFO address pointer reset

   SPI_SCS_L();
    ByteSend(CMD_FIFO_R);   //RX FIFO read command
    for(i=0; i <64; i++)
    {
        tmpbuf[i] = ByteRead();
    }
    SPI_SCS_H();

    for(i=0; i<64; i++)
    {
        recv = tmpbuf[i];
		//TimerA_UART_tx(recv);
        tmp = recv ^ PN9_Tab[i];
        if(tmp!=0)
        {
            Err_ByteCnt++;
            Err_BitCnt += (BitCount_Tab[tmp>>4] + BitCount_Tab[tmp & 0x0F]);
        }
    }
    if(Err_BitCnt < 1)
    {
    LED_ON();
    delay_ms(500);
    LED_OFF();
    }
	//TimerA_UART_tx(Err_BitCnt);
}

/*********************************************************************
** Err_State
*********************************************************************/
void Err_State(void)
{
    //ERR display
    //Error Proc...
    //...
    while(1);
}


/*********************************************************************
** InitRF
*********************************************************************/
UINT8 Init_a7129(void)
{
    //initial pin
    /*
    SCS = 1;
    SCK = 0;
    SDIO= 1;
    CKO = 1;
    GIO1= 1;
    GIO2= 1;
*/
    delay_ms(1);            //delay 1ms for regulator stabilized
    StrobeCMD(CMD_RF_RST);  //reset A7129 chip
    delay_ms(1);
    
    if(A7129_Config())      //config A7129 chip
        return 1;

    delay_us(800);          //delay 800us for crystal stabilized

    if(A7129_WriteID())     //write ID code
        return 1;

    if(A7129_Cal())         //IF and VCO Calibration
        return 1;

    return 0;
}

void InitRF(void)
{
    SPI_IO_Init();
	//A7129_POR();	//power on only
	while(1)
    {
        if(Init_a7129()) //init RF
        {
           // entry_deep_sleep_mode();
            delay_ms(2);
            //wake_up_from_deep_sleep_mode();
        }
        else
        {
            break;
        }
    }
	RxCnt = 0;
	Err_ByteCnt = 0;
	Err_BitCnt = 0;

}

