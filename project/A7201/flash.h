#ifndef _FLASH_H_
#define _FLASH_H_

#include "io430.h"
#include "common.h"

void flash_init(void);

void write_SegC (UINT8 * WriteData);
void read_SegC (UINT8 * ReadData);
void clean_SegC();


#endif

