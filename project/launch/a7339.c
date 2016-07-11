#include "io430.h"
#include "a7339.h"


/*********************************************************************
**  Global Variable Declaration
*********************************************************************/
	UINT8		timer;
	UINT8		TimeoutFlag;
	UINT16		RxCnt;
	UINT32		Err_ByteCnt;
	UINT32		Err_BitCnt;
	UINT16		TimerCnt0;
	UINT8		*Uartptr;
	UINT8		UartSendCnt;
	UINT8		CmdBuf[11];
	UINT8		tmpbuf[64];
	UINT16			Config[16];
	UINT16			Config_PageA[16];
	UINT16			Config_PageB[5];

extern void USART_SendByte(uint8_t ch);
const UINT8 BitCount_Tab[16]={0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
const UINT8 ID_Tab[8]={0xAA,0xAA,0xAA,0xC0,0xAA,0xAA,0xAA,0xC0};   
const UINT8 PN9_Tab[]=
{	0xAA,0xAA,0xAA,0xC0,0x5A,0x5A,0x5A,0x5A,
		0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,
		0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,
		0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,
		0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,
		0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,
		0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,
		0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A

};	// This table are 64bytes PN9 pseudo random code.

/*********************************************************************
**  A7339 Config
*********************************************************************/
const UINT16  A7339Config[]=		//433MHz, 10kbps (Fdev = 18.75KHz), Crystal=12.8MHz
{
    0x0823,   //SYSTEM CLOCK register,
    0x0821,   //PLL1 register,
    0xE666,   //PLL2 register,  433.301MHz0xDA05,   
    0x0000,   //PLL3 register,
    0x0A20,   //PLL4 register,  PDL=80us
    0x0024,   //PLL5 register,
    0x0000,   //PLL6 register,
    0x0011,   //CRYSTAL register,
    0x0000,   //PAGEA,
    0x0000,   //PAGEB,
    0x0040,   //RX1 register, 
    0x0000,   //Reserved,
    0x0000,   //ADC register,     
    0x0800,   //PIN CONTROL register,   Use Strobe CMD
    0x4880,   //CALIBRATION register,
    0x20D0    //MODE CONTROL register,  Use FIFO mode
};

const UINT16  A7339Config_PageA[]=   //433MHz, 10kbps (Fdev = 18.75KHz), Crystal=12.8MHz
{
    0x1606,   //TX1 register,   Fdev = 18.75kHz
    0x0000,   //WOR1 register,
    0x0000,   //WOR2 register,
    0x1107,   //RFI register,   Enable Tx Ramp up/down  
    0x1870,   //PM register,
    0x0000,   //Reserved,
    0x0000,   //AGC1 register,  
    0x2000,   //AGC2 register, 
    0x0041,   //GIO register,   GIO1=WTR
    0xD182,   //CKO register,   CKO=DCK, WSEL=800us
    0x00DE,   //VCB register,
    0x0A21,   //CHG1 register,  430MHz
    0x0022,   //CHG2 register,  435MHz
    0x003F,   //FIFO register,  FEP=63+1=64bytes
    0x1507,   //CODE register,  Preamble=4bytes, ID=4bytes
    0x0000    //WCAL register,
};

const UINT16  A7339Config_PageB[]=   //433MHz, 10kbps (Fdev = 18.75KHz), Crystal=12.8MHz
{
	0x0337,		//TX2 register,		TDL=80us
	0x0000,		//Reserved,
	0x0000,		//IF2 register,
};

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
	P2DIR |= SPI_DATA_IO + SPI_SCS_IO + SPI_CLK_IO;
	P2DIR &=	~GIO1_IO;
	P2OUT |= SPI_DATA_IO + SPI_SCS_IO + SPI_CLK_IO;
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
    SPI_SCS_L();//SCS=0;
	ByteSend(cmd);
	SPI_SCS_H();//SCS=1;
}

void WriteReg(UINT8 address, UINT16 dataWord)
{
    UINT8 i;

    SPI_DATA_OUT();
	SPI_SCS_L();//SCS=0;
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
UINT16 ReadReg(UINT8 address)
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
**  WritePageA
************************************************************************/
void WritePageA(UINT8 address, UINT16 dataWord)
{
    UINT16 tmp;

    tmp = address;
    tmp = ((tmp << 12) | Config[CRYSTAL_REG]);
    WriteReg(CRYSTAL_REG, tmp);
    WriteReg(PAGEA_REG, dataWord);
}

/************************************************************************
**  ReadPageA
************************************************************************/
UINT16 ReadPageA(UINT8 address)
{
    UINT16 tmp;

    tmp = address;
    tmp = ((tmp << 12) | Config[CRYSTAL_REG]);
    WriteReg(CRYSTAL_REG, tmp);
    tmp = ReadReg(PAGEA_REG);
    return tmp;
}

/************************************************************************
**  WritePageB
************************************************************************/
void WritePageB(UINT8 address, UINT16 dataWord)
{
    UINT16 tmp;

    tmp = address;
    tmp = ((tmp << 7) | Config[CRYSTAL_REG]);
    WriteReg(CRYSTAL_REG, tmp);
    WriteReg(PAGEB_REG, dataWord);
}

/************************************************************************
**  ReadPageB
************************************************************************/
UINT16 ReadPageB(UINT8 address)
{
    UINT16 tmp;

    tmp = address;
    tmp = ((tmp << 7) | Config[CRYSTAL_REG]);
    WriteReg(CRYSTAL_REG, tmp);
    tmp = ReadReg(PAGEB_REG);
    return tmp;
}

void WriteFIFO(void)
{
	UINT8 i;

	StrobeCMD(CMD_TFR);		//TX FIFO address pointer reset

	SPI_SCS_L();//SCS=0;
	ByteSend(CMD_FIFO_W);	//TX FIFO write command
	for(i=0; i <64; i++)
		ByteSend(PN9_Tab[i]);
	SPI_SCS_H();//SCS=1;
}
void Err_State(void)
{
    //ERR display
    //Error Proc...
    //...
    while(1);
}

void A7339_Config(void)
{
	UINT8 i;
	UINT16 tmp;

	for(i=0; i<8; i++)
	{
  		Config[i] = A7339Config[i];
		WriteReg(i, Config[i]);

	}
	
	for(i=10; i<16; i++)
	{
		Config[i] = A7339Config[i];
	  if(i!=11)
			WriteReg(i, Config[i]); 

	}
	
	for(i=0; i<16; i++)
	{
		Config_PageA[i] = A7339Config_PageA[i];
	  if(i!=5)
			WritePageA(i, Config_PageA[i]);

	}
	
	for(i=0; i<3; i++)
	{
		Config_PageB[i] = A7339Config_PageB[i];
	  if(i!=1)
			WritePageB(i, Config_PageB[i]);

	}
	
	//for check        
	tmp = ReadReg(SYSTEMCLOCK_REG);
    //USART_SendByte('d');
   // USART_SendByte(tmp);
	if(tmp != A7339Config[SYSTEMCLOCK_REG])
	{
		Err_State();	
	}
}

void WriteID(void)
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
      Err_State();
  }
}

void A7339_Cal(void)
{
	UINT8 vb,vbcf;			//VCO Current
	UINT8 vcb, vccf;		//VCO Band
	UINT16 tmp;

  //VCO Current calibration procedure @STB state
	WriteReg(MODE_REG, Config[MODE_REG] | 0x0800);			//VCO Current Calibration
  do{
      tmp = ReadReg(MODE_REG);
  }while(tmp & 0x0800);


	//for check(VCO Current)
  tmp = ReadPageA(VCB_PAGEA);
	vcb = tmp & 0x0F;
	vccf = (tmp>>4) & 0x01;
	if(vccf)
	{
        Err_State();
  }

  //VCO calibration procedure @STB state
	WriteReg(PLL1_REG, Config[PLL1_REG]);
	WriteReg(PLL2_REG, Config[PLL2_REG]);
	WriteReg(MODE_REG, Config[MODE_REG] | 0x0004);		//VCO Band Calibration
	do{
		tmp = ReadReg(MODE_REG);
	}while(tmp & 0x0004);
	
	//for check(VCO Band)
	tmp = ReadReg(CALIBRATION_REG);
	vb = (tmp >>5) & 0x07;
	vbcf = (tmp >>8) & 0x01;
	if(vbcf)
	{
		Err_State();
	}
}

void InitRF_M(void)
{
    //initial pin
 
	SPI_IO_Init();
    StrobeCMD(CMD_RF_RST);	//reset A7339 chip
    A7339_Config();			//config A7339 chip
    delay_us(800);//Delay100us(8);			//delay 800us for crystal stabilized
    WriteID();					//write ID code
    A7339_Cal();				//VCC and VCO calibration

}

void dump_a7339reg()
{
	UINT16 i,data;

	for( i =0;i<=MODE_REG;i++ )
	{
		data = ReadReg(i);
		USART_SendByte(data>>8);
		USART_SendByte(data&0xff);
	}
	USART_SendByte(55);

	for( i =0;i<=WCAL_PAGEA;i++ )
	{

		data = ReadPageA(i);
		USART_SendByte(data>>8);
		USART_SendByte(data&0xff);
	}
	USART_SendByte(ReadPageB(TX2_PAGEB));
	USART_SendByte(ReadPageB(IF2_PAGEB));
}
