
#ifndef __USER_H__
#define __USER_H__

#include "sys.h"

extern u8 AckFrame[14];




void ClearArray(char *array,u8 len);
void ClearArray_u8(u8 *array,u8 len);
u8 DataValidityCheck(u8 *DataFrame);
void SendACK(USART_TypeDef *USARTx,u8 AckSource,u8 *DataFrame);

void USART_SendLenData(USART_TypeDef *USARTx,u8 *DataBuff,u16 Len);
void USART_SendString(USART_TypeDef *USARTx,char *str);
	





#endif


