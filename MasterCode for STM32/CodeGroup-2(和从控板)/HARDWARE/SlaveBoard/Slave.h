#ifndef __SLAVE_H__
#define __SLAVE_H__

#include "sys.h"

#define MaxSendStr 30//���͵��ӿذ����󳤶�
#define MaxRx1Len 50	//
extern u8 USART1_RX_BUF[MaxRx1Len];//����1���ջ���
extern u16 USART1_RX_STA;

void SlaveInit(u32 bound);
u8 ConfigLimit(u16 LimitNum,u8 IfAck);

#endif


