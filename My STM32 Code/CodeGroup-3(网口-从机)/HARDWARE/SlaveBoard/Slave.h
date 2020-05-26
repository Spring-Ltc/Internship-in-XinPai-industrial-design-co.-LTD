#ifndef __SLAVE_H__
#define __SLAVE_H__

#include "sys.h"

#define PortSlave UART4
#define UART4_RX_DMA_Channel  DMA2_Channel3	//���崮��4���յ�DMAͨ��
#define UART4_RX_DMA_DataLen   250		//���崮��4���յ�DMA����������
extern u8 UART4_RX_DMA_BUF[UART4_RX_DMA_DataLen];//����1����DMA����

#define DMA_SourceAddress  UART4->DR	//DMAԴ��ַ
#define DMA_DestinationAddress  UART4_RX_DMA_BUF //DMAĿ���ַ

extern u8 ReceiveBuff[UART4_RX_DMA_DataLen];
extern u8 Flag_UART4_Solve;//����4�������ݴ���״̬��־��Ϊ1��ʾ���յ���δ����





#define MaxSendStr 30//���͵��ӿذ����󳤶�
#define MaxRx1Len 50	//
extern u16 UART4_RX_STA;

void DMA_START(void);
void SlaveInit(u32 bound);
u8 ConfigLimit(u16 LimitNum);//���ôӿذ����Ƴ�Һ����������ҪӦ��
u8 ConfigUploadtime(u16 Period);//�����ϴ�ʱ�����ȼ�
u16 QueryLimit(void);//ѯ��ʣ������ƴ���
u8 QueryWorkState(u8 *Ackbuff);//ѯ�ʴӻ�����״̬
u8 ControlSlave(u8 *State);//���ƴӻ�����״̬
#endif


