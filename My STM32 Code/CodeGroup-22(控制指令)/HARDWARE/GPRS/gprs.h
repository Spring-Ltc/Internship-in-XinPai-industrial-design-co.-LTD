

#ifndef __GPRS_H__
#define __GPRS_H__

#include "sys.h"

#define MaxGPRSLen 240
#define MaxRx1Len 240
#define MaxGPRSCommandLen 80	//AT指令最长字符数

#define DMA_Rec_Len_USART1 240
#define DMA_Tx_Len_USART1 240


extern int MSTtoHMITGetGprsStateSuccessFlag;
extern int MSTtoUSTTranspondDataFlag;

extern u8 USART1_RX_BUF[MaxRx1Len];//串口5接收缓存
extern u16 USART1_RX_STA;

void GPRSInit(u32 bound);
void GPRSConfig(char *Protocol, char *IP, char *Port);
u8 GPRSEnterAT(void);
u8 GPRSATControl(char *str,char *ack);
void ManufacturerServerTask(void);

extern u8 TranspondDataBuf[240];

#endif




