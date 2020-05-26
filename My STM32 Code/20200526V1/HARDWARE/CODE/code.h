#ifndef __CODE_H__
#define __CODE_H__

#include "sys.h"

#define MaxRx3Len 20
#define USART3_REC_LEN  			200  	//定义最大接收字节数 200

extern char USART3_RX_BUF[18];//串口3接收缓存
extern char BarCodeBuf[18];
extern u16 USART3_RX_STA;
extern int BCTtoHMITFlag;

void CodeInit(u32 bound);
void BarCodeTask(void);

#endif




