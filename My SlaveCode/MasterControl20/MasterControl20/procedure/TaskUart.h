


#ifndef  __TASKUART_H__
#define  __TASKUART_H__

#include	"config.h"
#include "USART1.h"

u8 DataValidityCheck(u8 *DataFrame);
void DataAnalyze(u8 *DataBuff);
void FramePacking(u8 FunctionCode,u8 *DataBuff);

#endif




