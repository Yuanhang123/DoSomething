#ifndef _A7201_H_
#define _A7201_H_

#include "io430.h"
//#include <stdint.h>
#include "common.h"

//unsigned char  Reg1_H;
#define		EAGC_H		(1<<7)
#define		TXP1_H		(1<<6)
#define		TXP0_H		(1<<5)
#define		ULS_H		(1<<4)
#define		CKS_H		(1<<3)
#define		ECK_H		(1<<2)
#define		STBY_H		(1<<1)
#define		IFB1_H		(1<<0)

//unsigned char  Reg1_L;
#define		IFB0_L		(1<<7)
#define		BAND_L		(1<<6)
#define		FASK_L		(1<<5)
#define		CE_L		(1<<4)
#define		RTX_L		(1<<3)
#define		CTLS_L		(1<<2)

#define		PREAMBLE		 1
#define		SYNC_OK 		2
#define		IDLE			 0
#define		MATCH_CNT 		(30)
#define		DATA_LEN		(4)
#define		DATA0_ADDR		(0)
#define		DATA1_ADDR		(4)
#define		DATA2_ADDR		(8)
#define		DATA_BUF_LEN	(12)

#define		MATCH_OK		0xFE

typedef enum _RxDataStatus{
	eIDLE,
	ePREAMBLE,
	eSYNC_OK,

}RxDataStatus;

extern UINT8  RxDataBuf[DATA_BUF_LEN];
void A7201_init(void);
void ReceiveData(void);
UINT8 match_data(UINT8 *data_index);
UINT8 compare_data(UINT8 *src,UINT8 *data);
#endif
