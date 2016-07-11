#ifndef __CPU_H
#define __CPU_H

/*********types **********/
#define W64     long long
#define W32     int
#define W16     short
#define W8      char

typedef W64 INT64;
typedef W32 INT32;
typedef W16 INT16;
typedef signed W8 INT8;

typedef unsigned W64 UINT64;
typedef unsigned W32 UINT32;
typedef unsigned W16 UINT16;
typedef unsigned W8 UINT8;

#define		TRUE			1
#define		FALSE			0

/*********types end**********/


#define CPU_CLOCK       (1000000)
#define delay_us(us)    __delay_cycles(CPU_CLOCK/1000000*(us))
#define delay_ms(ms)    __delay_cycles(CPU_CLOCK/1000*(ms))

#define	DBG()  //

#define     LED_IO      BIT0
#define     LED_OFF()    P1OUT &= ~LED_IO
#define     LED_ON()    P1OUT |= LED_IO
#endif /*__CPU_H*/

