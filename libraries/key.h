#ifndef _KEY_H_
#define _KEY_H_

#include "io430.h"
#include <stdint.h>
#include "common.h"

#define		K1_IO		BIT3	//port 3
#define		K2_IO		BIT4	//port 4

#define IsShortKey(k)	((k.dKeyRepeat &0x80)!=0)
#define IsLongKey_3(k)	(k.dKeyRepeat > 3000)
#define IsLongKey_5(k)	(k.dKeyRepeat > 5000)

typedef struct _KEY_
{
	UINT16    dKeyIO; 
	UINT16    dKeyRepeat;
}stKEY;

extern stKEY k1,k2;

void key_init(void);
void key_polling(void);

#endif