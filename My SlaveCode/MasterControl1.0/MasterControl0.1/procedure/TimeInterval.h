#ifndef	__TIMERINTERVAL_H
#define	__TIMERINTERVAL_H


#include " config.h" 
#include "CupDet.h"
#include "USART1.h"
#include "eeprom.h"
#include	"timer.h"
#include "TaskUart.h"
extern T0_100MS2,T0_100MS10,T1_1MS1,T0_10MS2,MS3, T0_100MS2 ;  //�����ļ�����ı���(�ⲿ����)

void TimeInterval();


//extern	u8	idata TX1_Buffer[COM_TX1_Lenth];	//���ͻ���
//extern	u8 	idata RX1_Buffer[COM_RX1_Lenth];	//���ջ���
////extern	u8	xdata TX2_Buffer[COM_TX2_Lenth];	//���ͻ���
//extern	u8 	xdata RX2_Buffer[COM_RX2_Lenth];	//���ջ���



#endif