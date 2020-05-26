#ifndef __TOOL_H__
#define __TOOL_H__

#include "sys.h"

void DataPacking(u8 len, u8 localID,u8 destinationID,u8 IfAck,u8 FunctionCode,u8 *Data,u8 *Packed);
u8 DataValidityCheck(u8 *DataFrame);
void SendACK(USART_TypeDef *USARTx,u8 AckSource,u8 *DataFrame);

#endif




