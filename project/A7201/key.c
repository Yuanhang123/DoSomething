
#include "key.h"

void key_init(void)
{
    P1DIR &= ~(K1_IO | K2_IO);	//P1 input      
    P1REN |=  (K1_IO | K2_IO);	//P1 pull up
}

#if 1

//#define		IO_KEY_VOLUME	(P1IN & BIT3)



void Key_scan(UINT8*key_info) 
{
 
	static UINT8 key_value_old=0;//°´¼üÇ°Ò»´ÎµÄ×´Ì¬¼Ä´æÆ÷
	static UINT8 key_repeat=0;//ÑÓÊ±¶¶¶¯Ê±¼ä´æÆ÷

	int key_value=0; 
	if(!IO_KEY_PREV)//°´¼üÉ¨ÃèSW1
	{
	key_value = KEY_PREV; 
	} 

	if(!IO_KEY_NEXT)//°´¼üÉ¨ÃèSW2
	{
	key_value |= KEY_NEXT; 
	}

//	if(!P_K3)//°´¼üÉ¨ÃèSW2
//		key_value=KEY_VOLUME; 
	
	if((key_value==key_value_old)&&(key_value > 0))
	{	
		if(key_value > 0)
		{
			//key_repeat ;

			if(key_repeat++ > 100)
			{
				*key_info = key_value | 0x80;
				key_repeat = 0;
			}

		}
		
	}
	else
	{
	//	if(key_value_old)
	//	{
			if(key_repeat < 10)
				*key_info = key_value_old;
		//}	
		key_value_old = key_value;
		key_repeat = 0;
	}
}



void key_test()
{
	//key_init();
	for(;;)
	{
		UINT8 KeyInfo = KEY_NULL;
		delay_ms(10);
		Key_scan(&KeyInfo);

		switch(KeyInfo)
		{
			case KEY_NULL:

			break;
			case KEY_PREV:

			break;
			case KEY_NEXT:

			break;
			case KEY_VOLUME:

			break;
			case KEY_MATCH:

			break;
			case KEY_CLEAN:
			
			break;
				
		}

	}
}
	
#endif

