#include <msp430.h>
#include "kt403.h"
#include "Ta_uart.h"

UINT8 kt403_cmd[2][8] =
{
    {0xFF,0x06,0x01,0x00,0x00,0x00,0xFE,0xFA},
    {0xFF,0x06,0x02,0x00,0x00,0x00,0xFE,0xF9}
};

void kt403_init()
{
 // USART_Init();
}
void kt403_send_cmd(UINT8 *cmd)
{
    UINT8 i = 0 ;
    //USART_SendByte(0x7E); //起始
    for(i=0; i<8; i++)//数据
    {
       // USART_SendByte(cmd[i]) ;
    }
   // USART_SendByte(0xEF) ;//结束
}
