

#ifndef __TaskUart_H__
#define __TaskUart_H__

#include "config.h"

#define MaxReceiveLen 35	//一帧最大长度

//串口接收相关声明
extern u8 ReceiveEnable;	//接收缓存使能控制
extern u8 ReceiveBuff[MaxReceiveLen];//接收缓存区



u8 DataValidityCheck(u8 *DataFrame);
void DataAnalyze(u8 *DataBuff);
void TaskUart1_Init(u16 bound);
void SendDataLen(u8 *DataBuff,u8 len);
void FramePacking(u8 FunctionCode,u8 *DataBuff);

void UART_Send_Byte(unsigned char dat);


#endif


