#ifndef  __RESIDUEDEGREE_H__
#define  __RESIDUEDEGREE_H__

#include	"config.h"
#include  "eeprom.h"
#include  "USART1.h"
#include "CupDet.h"

u16 DataProcessing(u16  SurplusCount );
void Confine(void);//������󼸴κ������Ƴ�����eeprom����
void MasterControlEE(void);//�޸�ʣ�౭�Ӵ���

extern u16 SurplusCount;//�����ڱ��ο�������ʱ�ԶԱ���ʹ�ü���
extern u8 Cup1Eff;//���ձ�����Ч ������
extern u8 Cup3Eff; //���ձ�����Ч ������//���ձ�����Ч ������
extern u8 Cup5Eff; //���ձ�����Ч ������

#endif