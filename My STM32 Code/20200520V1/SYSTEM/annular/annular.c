#include "annular.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"


#define BUFFERMAX  64
static u8 UsartBuffer[BUFFERMAX];
static u8 BufferWptr = 0;
static u8 BufferRptr = 0;
static u8 BufferRead(u8* data);

static u8 DMARXbuff[10];
static u8 DMATXbuff[10];

uint16_t BufferWrite(USART_TypeDef* USARTx)
{
	if(BufferWptr == (BufferRptr - 1))
	{
		return ;
	}
	UsartBuffer[BufferRptr] = USART_ReceiveData(USARTx);
	BufferWptr++;
	BufferWptr = BufferWptr % BUFFERMAX;
}


u8 BufferRead(u8* data)
{
	if(BufferRptr == BufferWptr)
	{
		return 0;
	}
	*data = UsartBuffer[BufferRptr];
	BufferRptr++;
	BufferRptr = BufferRptr % BUFFERMAX;
	return 1;
}
