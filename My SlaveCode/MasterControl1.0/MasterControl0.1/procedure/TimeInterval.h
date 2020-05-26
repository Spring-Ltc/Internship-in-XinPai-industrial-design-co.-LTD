#ifndef	__TIMERINTERVAL_H
#define	__TIMERINTERVAL_H


#include " config.h" 
#include "CupDet.h"
#include "USART1.h"
#include "eeprom.h"
#include	"timer.h"
#include "TaskUart.h"
extern T0_100MS2,T0_100MS10,T1_1MS1,T0_10MS2,MS3, T0_100MS2 ;  //其他文件定义的变量(外部变量)

void TimeInterval();


//extern	u8	idata TX1_Buffer[COM_TX1_Lenth];	//发送缓冲
//extern	u8 	idata RX1_Buffer[COM_RX1_Lenth];	//接收缓冲
////extern	u8	xdata TX2_Buffer[COM_TX2_Lenth];	//发送缓冲
//extern	u8 	xdata RX2_Buffer[COM_RX2_Lenth];	//接收缓冲



#endif