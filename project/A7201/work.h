#ifndef _WORK_H_
#define _WORK_H_

#include "io430.h"
#include "common.h"
#include "key.h"
#include "flash.h"
#include "a7201.h"
#include "Ta_uart.h"

typedef enum _work_status
{
	MATCH,
	NORMAL,
}eWorkStatus;

#define		SEG_LEN		5
void setup_normal(void);
void work_main(void);


#endif

