#ifndef	__MASTERCONTROL_H
#define	__MASTERCONTROL_H

#include " config.h" 

#include "USART1.h"
#include "eeprom.h"

#include	"timer.h"

#include "CupDet.h"

void MasterControl(void);
void MasterControlRead(void);
void MasterControlReadEE();
extern T0_100MS2,T0_100MS10,T1_1MS1,T1_10MS2,MS3,Second1  ;  //其他文件定义的变量(外部变量)
extern u8 Cup1Eff;//接收杯子有效 缓冲区
extern u8 Cup3Eff; //接收杯子有效 缓冲区//接收杯子有效 缓冲区
extern u8 Cup5Eff; //接收杯子有效 缓冲区

#endif