#ifndef _A7339_H_
#define _A7339_H_

#include "io430.h"
#include <stdint.h>
#include "common.h"

/*********************************************************************
**  I/O Declaration
*********************************************************************/

#define		SPI_CLK_IO		BIT5
#define		SPI_DATA_IO		BIT3	
#define		SPI_SCS_IO		BIT6 

//#define		CKO_IO			BIT1	
#define		GIO1_IO			BIT1

#define		SPI_CLK_L()			(P2OUT &= ~SPI_CLK_IO)
#define		SPI_CLK_H()			(P2OUT |= SPI_CLK_IO)

#define		SPI_DATA_L()		(P2OUT &= ~SPI_DATA_IO)
#define		SPI_DATA_H()		(P2OUT |= SPI_DATA_IO)

#define		SPI_SCS_L()			(P2OUT &= ~SPI_SCS_IO)
#define		SPI_SCS_H()			(P2OUT |= SPI_SCS_IO)

#define		SPI_DATA_OUT()		(P2DIR |= SPI_DATA_IO)
#define		SPI_DATA_IN()		(P2DIR &= ~SPI_DATA_IO)	

#define		SPI_DATA			(P2IN & SPI_DATA_IO)

#define		GIO1				(P2IN & GIO1_IO)

#define		SPI_delay(t)		Delay10us(t)//((void)0)

/********************************************************************
*   A7329REG.h
*   RF Chip-A7329 Hardware Definitions
*
*   This file provides the constants associated with the
*   AMICCOM A7329 device.
*
********************************************************************/

#define SYSTEMCLOCK_REG 	0x00
#define PLL1_REG 			0x01
#define PLL2_REG 			0x02
#define PLL3_REG 			0x03
#define PLL4_REG			0x04
#define PLL5_REG			0x05
#define PLL6_REG			0x06
#define CRYSTAL_REG			0x07
#define PAGEA_REG	  		0x08
#define PAGEB_REG			0x09
#define RX1_REG  			0x0A
#define ADC_REG  			0x0C
#define PIN_REG 			0x0D
#define CALIBRATION_REG  	0x0E
#define MODE_REG  			0x0F

#define TX1_PAGEA           0x00
#define WOR1_PAGEA          0x01
#define WOR2_PAGEA          0x02
#define RFI_PAGEA           0x03
#define PM_PAGEA            0x04
#define AGC1_PAGEA          0x06
#define AGC2_PAGEA          0x07
#define GIO_PAGEA           0x08
#define CKO_PAGEA           0x09
#define VCB_PAGEA           0x0A
#define CHG1_PAGEA          0x0B
#define CHG2_PAGEA          0x0C
#define FIFO_PAGEA			0x0D
#define CODE_PAGEA			0x0E
#define WCAL_PAGEA			0x0F

#define TX2_PAGEB			0x00
#define IF2_PAGEB			0x02



#define CMD_Reg_W			0x00	//000x,xxxx	control register write
#define CMD_Reg_R			0x80	//100x,xxxx	control register read
#define CMD_ID_W			0x20	//001x,xxxx	ID write
#define CMD_ID_R			0xA0	//101x,xxxx	ID Read
#define CMD_FIFO_W			0x40	//010x,xxxx	TX FIFO Write
#define CMD_FIFO_R			0xC0	//110x,xxxx	RX FIFO Read
#define CMD_RF_RST			0x70	//x111,xxxx	RF reset
#define CMD_TFR				0x60	//0110,xxxx	TX FIFO address pointrt reset
#define CMD_RFR				0xE0	//1110,xxxx	RX FIFO address pointer reset

#define CMD_SLEEP			0x10	//0001,0000	SLEEP mode
#define CMD_IDLE			0x12	//0001,0010	IDLE mode
#define CMD_STBY			0x14	//0001,0100	Standby mode
#define CMD_PLL				0x16	//0001,0110	PLL mode
#define CMD_TX				0x1A	//0001,1010	TX mode
//#define CMD_DEEP_SLEEP	0x1C	//0001,1100 Deep Sleep mode(tri-state)
#define CMD_DEEP_SLEEP		0x1F	//0001,1111 Deep Sleep mode(pull-high)

void InitRF_M(void);
void WriteFIFO(void);
void StrobeCMD(UINT8 cmd);
void dump_a7339reg(void);
#endif