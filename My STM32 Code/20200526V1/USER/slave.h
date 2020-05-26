#ifndef __SLAVE_H__
#define __SLAVE_H__
#include "stdio.h"	
#include "sys.h" 

#include "user.h"

#define PortSlave UART4
#define EN_UART4_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

#define DMA_Rec_Len 40
#define DMA_Tx_Len 40
	  	
extern u8 BoradSlaveBuf[29];//���������������ȡ�ӿذ��״̬���ݣ��ӿذ帺�������������ݡ�

void SlaveInit(u32 bound);
void BoradSlaveTask(void);


u8 ConfigLimit(u16 LimitNum);//���ôӿذ����Ƴ�Һ����������ҪӦ��
u8 ConfigUploadtime(u16 Period);//�����ϴ�ʱ�����ȼ�
u16 QueryLimit(void);//ѯ��ʣ������ƴ���
u8 QueryWorkState(u8 *Ackbuff);//ѯ�ʴӻ�����״̬
u8 ControlSlave(u8 *State);//���ƴӻ�����״̬


#endif









