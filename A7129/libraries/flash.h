#ifndef _FLASH_H_
#define _FLASH_H_

#include "io430.h"
#include <stdint.h>
#include "common.h"

int flash_init(void);

void write_SegC (char value);
void read_SegC (char * ReadData);


#endif

