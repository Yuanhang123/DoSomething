#ifndef _A7201_H_
#define _A7201_H_

#include "io430.h"
#include <stdint.h>
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

#define PACKET_SIZE 64
/*
#define		Premable	(1<<6)  // = RF_Flag ^ 6;
#define		SynWord		(1<<5)  // = RF_Flag ^ 5;
#define		Start		(1<<3)  // = RF_Flag ^ 3;
*/

/*
#define TX      0
#define RX      1
#define CE_ON   0
#define CE_OFF  1
#define ASK     0
#define FSK     1
#define HI_BAND 0
#define LO_BAND 1
#define IF0_MIN 1
#define IF1_MIN 1
#define IF0_MED 0
#define IF1_MIN 1
#define IF0_MAX 0
#define IF1_MAX 1
#define CKO_ON  0
#define CKO_OFF 1
#define CKO_D4  0
#define CKO_D16 1
#define UP_SIDE 0
#define LO_SIDE 1
#define AGC_ON  0
#define AGC_OFF 1



#define pSPISTB      P3_4
#define pSPICLK      P3_1
#define pSPIDATA     P3_0
#define pSPIS        P1_6
#define pCE          P1_5
#define pTX_RX       P1_4
#define pDATAIN      P1_3
#define pDATAOUT     P1_2
*/
void A7302_init(void);

#endif