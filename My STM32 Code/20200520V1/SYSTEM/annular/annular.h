#ifndef __ANNULAR_H
#define __ANNULAR_H
#include "stdio.h"	
#include "sys.h"

uint16_t BufferWrite(USART_TypeDef* USARTx);
u8 BufferRead(u8* data);

#endif