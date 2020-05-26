
#ifndef __USER_H__
#define __USER_H__

#include "sys.h"

//���ʶ�����
#define ProducerServerID 0x00	//�����̷�����
#define UserServerID 0x01		//�û�������
#define MasterBoardID	0x20	//�豸���ذ�
#define SlaveBoardID  0x21		//�豸�ӿ�
//Ӧ������
#define AckTypeProducerServer	0x03	//������Ӧ��
#define AckTypeUserServer	0xA7		//�û�������Ӧ��
#define AckTypeMasterBoard	0x72	//�豸Ӧ��


//��������
void ClearArray(char *array,u8 len);
void ClearArray_u8(u8 *array,u8 len);

void USART_SendLenData(USART_TypeDef *USARTx,u8 *DataBuff,u16 Len);
void USART_SendString(USART_TypeDef *USARTx,char *str);

u8 DataPacking(u8 len, u8 localID,u8 destinationID,u8 IfAck,u8 FunctionCode,u8 *Data,u8 *Packed);
u8 AckPacking(u8 LocalID,u8 *FrameReceive);
u8 FrameCheckIfSendAck(USART_TypeDef *USARTx,u8 *FrameReceive);



#endif


