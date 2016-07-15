#include "work.h"

eWorkStatus work_sta = NORMAL;
UINT8 cur_music = 1;

UINT8 RxCMD[4] ={0};
#define		MUSIC_MAX		52

void match_save(UINT8 MusicIndex,UINT8 *MatchVal)
{
	UINT8 TempData[64] = {0};
	UINT8 index = 0;
	read_SegC(TempData);
	
	while(1)
	{
		if(!(TempData[index] & 0x80))
		{
            if(MATCH_OK == compare_data(&TempData[index+1],MatchVal))
            {
                break;
            }
            else
            {
                index += SEG_LEN;
                if(index > (64 - SEG_LEN))
                {
                    index = 0;
                    break;
                }
            }
		} 
		else 
		{
			break;
		}
	}
  
	TempData[index] = MusicIndex & 0x7F;
	for (int i=0; i<4; i++)
	{
	  TempData[++index] = MatchVal[i];
	}
	write_SegC(TempData);

}

UINT8 match_play(UINT8 *MatchVal,UINT8 *music_index)
{
	UINT8 TempData[64] = {0};
	UINT8 index = 0;
	read_SegC(TempData);
	
	while(1)
	{
		if(!(TempData[index] & 0x80))
		{
			for (UINT8 i=0; i<DATA_LEN; i++)
			{
			  if(TempData[index+i+1] == MatchVal[i])
			  {
			  	if(i >= (DATA_LEN-1))
				{
					*music_index = TempData[index] & 0x7F ;
					return TRUE;
				}
				continue;
			  }
			  else
			  {
			  	break;	
			  }
			}

			index += SEG_LEN;
			if(index > (64 - SEG_LEN))
			{
				//index = 0;
				return FALSE;
			}
		} 
		else 
		{
			return FALSE;
		}
	}
    //return FALSE;
}

void setup_normal()
{
	UINT8 KeyInfo = KEY_NULL;
	Key_scan(KeyInfo);
	switch(KeyInfo)
	{
		case KEY_NULL:
		break;
		
		case KEY_PREV:
			if(cur_music-- <= 1)
			cur_music = MUSIC_MAX;
		//TimerA_UART_print("prev music cur_music");
		break;
		
		case KEY_NEXT:
			if(cur_music++ >= MUSIC_MAX)
				cur_music = 0;
			//TimerA_UART_print("next music cur_music");
			work_sta = MATCH;
		break;
		
		case KEY_VOLUME:

		break;
		
		case KEY_MATCH:
			k1.dKeyRepeat = 0;
			//TimerA_UART_print("clean mode");
			work_sta = MATCH;
		break;
		
		case KEY_CLEAN:
		k2.dKeyRepeat = 0;
		//TimerA_UART_print("clean mode");
		clean_SegC();		
		break;
		
		default:
			break;	
			
	}
}

	
void work_main()
{
	//static UINT8 setup_sta = IDLE;
	UINT8 data_index;
	if(match_data(&data_index))
	{
		switch (work_sta)
		{
			case NORMAL:
				for(UINT8 i = 0; i<3; i++){
					if(match_play(&RxDataBuf[DATA0_ADDR] + (i*DATA_LEN),&data_index))
                    {
						//TimerA_UART_tx(data_index);
                        LED_ON();
                    }
				}
				break;

			case MATCH:
				match_save(cur_music,&RxDataBuf[data_index]);
				work_sta = NORMAL;
				break;

			default:
				break;	
		}
        LED_OFF();
	}
}


