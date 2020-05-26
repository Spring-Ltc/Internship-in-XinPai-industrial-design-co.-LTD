#ifndef __SLAVE_H__
#define __SLAVE_H__

#include "sys.h"

#define PortSlave UART4
#define UART4_RX_DMA_Channel  DMA2_Channel3	//���崮��4���յ�DMAͨ��
#define UART4_TX_DMA_Channel  DMA2_Channel5 //���崮��4���͵�DMAͨ��

#define UART4_DMA_DataLen   50		//���崮��4���պͷ��͵�DMA����������


extern u8 UART4_RX_DMA_BUF[UART4_DMA_DataLen];//����1����DMA����
extern u8 UART4_TX_DMA_BUF[UART4_DMA_DataLen];//����1����DMA����
#define DMA_PortAddress  &UART4->DR	//DMA����˿ڵ�ַ
#define DMA_ReceiveAddress  UART4_RX_DMA_BUF //DMA���յ������ַ
#define DMA_SendAddress  UART4_TX_DMA_BUF //DMA���������ַ������

extern u8 SendBuff[UART4_DMA_DataLen];
extern u8 ReceiveBuff[UART4_DMA_DataLen];
extern u8 Flag_UART4_Solve;//����4�������ݴ���״̬��־��Ϊ1��ʾ���յ���δ����
extern u16 ReceiveLen;//��ǰ�յ������ݳ���


#define MaxSendStr 30//���͵��ӿذ����󳤶�
#define MaxRxLen 50	//


void SlaveInit(u32 bound);
void SlaveUSART_Init(u32 bound);
void SlaveDMA_Init(void);
void TX_DMA_ENABLE(void);
void RX_DMA_ENABLE(void);


u8 ConfigLimit(u16 LimitNum);//���ôӿذ����Ƴ�Һ����������ҪӦ��
u8 ConfigUploadtime(u16 Period);//�����ϴ�ʱ�����ȼ�
u16 QueryLimit(void);//ѯ��ʣ������ƴ���
u8 QueryWorkState(u8 *Ackbuff);//ѯ�ʴӻ�����״̬
u8 ControlSlave(u8 *State);//���ƴӻ�����״̬
#endif


