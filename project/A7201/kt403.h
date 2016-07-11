#ifndef _KT403_H_
#define _KT403_H_

#include "common.h"

extern UINT8 kt403_cmd[2][8];
void kt403_init(void);
void kt403_send_cmd(UINT8 *cmd);
#endif