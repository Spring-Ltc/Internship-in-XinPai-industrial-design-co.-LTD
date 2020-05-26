
#ifndef __USER_H__
#define __USER_H__

#include "sys.h"

void ClearArray(char *array,u8 len);
void ClearArray_u8(u8 *array,u8 len);

void USART_SendLenData(USART_TypeDef *USARTx,u8 *DataBuff,u16 Len);
void USART_SendString(USART_TypeDef *USARTx,char *str);
void USARTtoBuf(u8 *USARTx_RX_BUF, u8 *TargetBuf, u8 len);
	

u8 DataPacking(u8 len, u8 localID,u8 destinationID,u8 IfAck,u8 FunctionCode,u8 *Data,u8 *Packed);
u8 AckPacking(u8 LocalID,u8 *FrameReceive);
u8 FrameCheckIfSendAck(USART_TypeDef *USARTx,u8 *FrameReceive);

extern u8 AckPacked[12];




#endif


