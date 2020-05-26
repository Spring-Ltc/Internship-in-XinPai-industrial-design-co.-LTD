

#ifndef __GPRS_H__
#define __GPRS_H__

#include "sys.h"

#define MaxGPRSLen 50
#define MaxRx1Len 50

extern char USART1_RX_BUF[MaxRx1Len];//´®¿Ú5½ÓÊÕ»º´æ
extern u16 USART1_RX_STA;

void GPRSInit(u32 bound);
void GPRSConfig(char *Protocol, char *IP, char *Port);
u8 GPRSEnterAT(void);
u8 GPRSATControl(char *str,char *ack);
void ManufacturerServerTask(void);

#endif




