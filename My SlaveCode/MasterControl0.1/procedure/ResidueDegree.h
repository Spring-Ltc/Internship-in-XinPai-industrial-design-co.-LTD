#ifndef  __RESIDUEDEGREE_H__
#define  __RESIDUEDEGREE_H__

#include	"config.h"
#include  "eeprom.h"
#include  "USART1.h"
#include "CupDet.h"

u16 DataProcessing(u16  SurplusCount );
void Confine(void);//处理最后几次函数限制超出，eeprom重新
void MasterControlEE(void);//修改剩余杯子次数

extern u16 SurplusCount;//杯子在本次开机运行时对对杯子使用计数
extern u8 Cup1Eff;//接收杯子有效 缓冲区
extern u8 Cup3Eff; //接收杯子有效 缓冲区//接收杯子有效 缓冲区
extern u8 Cup5Eff; //接收杯子有效 缓冲区

#endif