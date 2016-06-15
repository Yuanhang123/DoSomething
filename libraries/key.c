
#include "key.h"


stKEY k1 = {K1_IO,0};
stKEY k2 = {K2_IO,0};

void key_init(void)
{
    P1DIR &= ~(K1_IO | K2_IO);	//P1 input      
    P1REN |=  (K1_IO | K2_IO);	//P1 pull up
}

bool PaserKey(stKEY *key)
{
	UINT16 key_state  = P1IN;
	if(!(key_state & key.dKeyIO))
	{
		if(key.dKeyRepeat < 200)
			key.dKeyRepeat++;
	}
	else
	{
		if((key.dKeyRepeat > 10) && (key.dKeyRepeat & 0x8000 == 0))
			key.dKeyRepeat |= 0x8000;
		else
			key.dKeyRepeat = 0;
	}


}
0x8012

void key_polling()
{

	PaserKey(&k1);
	PaserKey(&k2);	
}

void key_test()
{
	key_init();
	for(;;)
	{
		key_polling();
		delay_ms(10);
		if(IsShortKey(k1))
			DBG();
		if(IsShortKey(k2))
			DBG();
		if(IsLongKey(k1))
			DBG();
		if(IsLongKey(k2))
			DBG();
		if(IsLongKey(k1) && IsLongKey(k2))
			DBG();

	}
}

