#ifndef __SLAVE_H__
#define __SLAVE_H__

#include "sys.h"

#define MaxSendStr 30//���͵��ӿذ����󳤶�
#define MaxRx1Len 50	//
extern u8 UART4_RX_BUF[MaxRx1Len];//����1���ջ���
extern u16 UART4_RX_STA;

void SlaveInit(u32 bound);
u8 ConfigLimit(u16 LimitNum,u8 IfAck);
u8 ConfigUploadtime(u16 Period,u8 IfAck);
u16 QueryLimit(u8 IfAck);
u8 QueryWorkState(u8 IfAck);
u8 ControlSlave(u8 *State,u8 IfAck);
#endif


