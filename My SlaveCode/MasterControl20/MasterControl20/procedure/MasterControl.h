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
extern T0_100MS2,T0_100MS10,T1_1MS1,T1_10MS2,MS3,Second1  ;  //�����ļ�����ı���(�ⲿ����)
extern u8 Cup1Eff;//���ձ�����Ч ������
extern u8 Cup3Eff; //���ձ�����Ч ������//���ձ�����Ч ������
extern u8 Cup5Eff; //���ձ�����Ч ������

#endif