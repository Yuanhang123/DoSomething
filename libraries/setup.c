#include "setup.h"

UINT8 work_sta = IDLE;
UINT8 RxCMD[4] ={0};

void match_save(UINT8 MusicIndex,UINT8 *MatchVal)
{
	UINT8 TempData[64] = {0};
	UINT8 index = 0;
	read_SegC(TempData);
	
	while(1)
	{
		if(TempData[index] & 0x80)
		{
			index +=SEG_LEN;
			if(index > (64 - SEG_LEN))
			{
				index = 0;
				break;
			}
		} 
		else 
		{
			break;
		}
	}
  
	TempData[index] = MusicIndex | 0x80;
	for (int i=0; i<4; i++)
	{
	  TempData[++index] = MatchVal[i];
	}

	write_SegC(TempData);

}
void match_clean(UINT8 *MatchVal)
{
	UINT8 TempData[64] = {0};
	UINT8 index = 1;
	read_SegC(TempData);
	
	while(1)
	{	
		for (int i=0; i<4; i++)
		{
		  if(TempData[index] = MatchVal[i])
		  {
		  	index++;
			continue;
		  }
		  else
		  {
		  	break;	
		  }
		}
		if(4 == i)
		{
			;
		}

	}
	write_SegC(TempData);

}



void setup_normal()
{
	if(IsShortKey(k1))
	{
		TimerA_UART_print("switch music");
	}
	if(IsShortKey(k2))
	{
		if(work_sta == CLEAN)
		{
			TimerA_UART_print("clean");
			flash_clean();
			work_sta = IDLE;
		}
		else
		{
			TimerA_UART_print("switch volume");
		}
	}
	if(IsLongKey_3(k1))
	{
		TimerA_UART_print("connect mode");
		work_sta = CONNECT;
	}
	if(IsLongKey_3(k2))
	{
		TimerA_UART_print("disconnect mode");
		work_sta = DISCONNECT;
	}
	if(IsLongKey_5(k2) )
	{
		TimerA_UART_print("clean mode");
		work_sta = CLEAN;
	}
}

void setup_clean()
{
	if(IsShortKey(k2))
	{
		TimerA_UART_print("clean");
		flash_clean();
	}
	else 
	{
		setup_idle();
	}

}
	
void work_main()
{
	//static UINT8 setup_sta = IDLE;
	setup_normal();
	switch (work_sta)
	{
		case IDLE:
		case CLEAN:
			break;
			
		case CONNECT:
			if(rx_data(RxCMD))
			{
				match_save(RxCMD);
				work_sta = IDLE;
			}
			break;
			
		case DISCONNECT:
			if(rx_data())
			{
				flash_dis();
				work_sta = IDLE;
			}

			break;
			
		default:
			break;	
	}
}


