#ifndef __CODE_H__
#define __CODE_H__

#include "sys.h"

#define MaxRx3Len 20
#define USART3_REC_LEN  			200  	//�����������ֽ��� 200

extern char USART3_RX_BUF[18];//����3���ջ���
extern char BarCodeBuf[18];
extern u16 USART3_RX_STA;
extern int BCTtoHMITFlag;

void CodeInit(u32 bound);
void BarCodeTask(void);

#endif




