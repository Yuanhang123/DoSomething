#ifndef _KT403_H_
#define _KT403_H_
extern uint8_t kt403_cmd[2][8];
void kt403_init(void);
void kt403_send_cmd(uint8_t *cmd);
#endif