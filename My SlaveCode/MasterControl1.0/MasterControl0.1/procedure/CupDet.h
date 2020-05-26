/*------------------------------------------------------------------*/
//
//
//
//
//
//
//
//
/*------------------------------------------------------------------*/


#ifndef __CUPDET_H
#define __CUPDET_H	 

#include " config.h" 
#include "USART1.h"
#include "delay.h"
#include  "eeprom.h"
#include	"ResidueDegree.h"

#define Exist 1
#define NoExist 0
#define ON 0                         //����
#define OFF 1                      //ֹͣ
#define VALVEON  1              //����
#define VALVEOFF 0                //�ر�

#define WaitingWaterTimeOut 22  //��һ�εȴ���ˮʱ�� 22

#define SensorCupTimerOut 3       //������ ���ñ��Ӻ�����ʱ��
#define SensorCupOutTime 5        //������ ����ȡ����ش���ʱ��
#define SensorWaterTimeOut  28    //�ڶ�����ˮ ��������⵽��ˮ��ȷ��ʱ��  12

extern COMx_Define	COM1;
extern T0_100MS2,T0_100MS10,T1_1MS1,T1_10MS2,MS3,Second1  ;  //�����ļ�����ı���(�ⲿ����)

u8 SensorCup    (u8 Number );//������ޱ��Ӻ���
u8 SensorCupOut (u8 Number );//�������ޱ��Ӻ���

void SlaveControl(void);
void	SlaveData(void);
void  CupDetconfig(void);  
void CupDet(void);
void PumpControl();
u8  CheckSumTxd(u8 *dat,u8 length);
//void  CupDetconfig();
#endif