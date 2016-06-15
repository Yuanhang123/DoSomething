#include "setup.h"

UINT8 work_sta = IDLE;
UINT8 RxCMD[4] ={0};
void cmd_save()
{

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
	swith (work_sta)
	{
		case IDLE:
		case CLEAN:
			break;
			
		case CONNECT:
			if(rx_data(RxCMD))
			{
				cmd_save();
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


