
#ifndef __USER_H__
#define __USER_H__

#include "sys.h"

//身份识别编码
#define ProducerServerID 0x00	//生产商服务器
#define UserServerID 0x01		//用户服务器
#define MasterBoardID	0x20	//设备主控板
#define SlaveBoardID  0x21		//设备从控
//应答功能码
#define AckTypeProducerServer	0x03	//生产商应答
#define AckTypeUserServer	0xA7		//用户服务器应答
#define AckTypeMasterBoard	0x72	//设备应答


//函数声明
void ClearArray(char *array,u8 len);
void ClearArray_u8(u8 *array,u8 len);

void USART_SendLenData(USART_TypeDef *USARTx,u8 *DataBuff,u16 Len);
void USART_SendString(USART_TypeDef *USARTx,char *str);

u8 DataPacking(u8 len, u8 localID,u8 destinationID,u8 IfAck,u8 FunctionCode,u8 *Data,u8 *Packed);
u8 AckPacking(u8 LocalID,u8 *FrameReceive);
u8 FrameCheckIfSendAck(USART_TypeDef *USARTx,u8 *FrameReceive);



#endif


