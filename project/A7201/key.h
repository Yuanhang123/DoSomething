#ifndef _KEY_H_
#define _KEY_H_

#include "io430.h"
#include "common.h"

#define		K1_IO		BIT3	//port 1.3
#define		K2_IO		BIT6	//port 2.6

void key_init(void);

void Key_scan(UINT8*key_info);

#define		KEY_NULL	(0)
#define		KEY_PREV	(1)
#define		KEY_NEXT	(2)
#define		KEY_VOLUME	(KEY_PREV|KEY_NEXT)
#define		KEY_MATCH	(KEY_NEXT|0x80)
#define		KEY_CLEAN	(KEY_VOLUME|0x80)

#define		IO_KEY_PREV		(P1IN & K1_IO)
#define		IO_KEY_NEXT		(P2IN & K2_IO)

#endif