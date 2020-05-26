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
#define ON 0                         //启动
#define OFF 1                      //停止
#define VALVEON  1              //开泵
#define VALVEOFF 0                //关泵

#define WaitingWaterTimeOut 22  //第一次等待来水时间 22

#define SensorCupTimerOut 3       //传感器 放置杯子后启动时间
#define SensorCupOutTime 5        //传感器 杯子取出后回待杯时间
#define SensorWaterTimeOut  28    //第二次来水 传感器检测到无水后确认时间  12

extern COMx_Define	COM1;
extern T0_100MS2,T0_100MS10,T1_1MS1,T1_10MS2,MS3,Second1  ;  //其他文件定义的变量(外部变量)

u8 SensorCup    (u8 Number );//检测有无杯子函数
u8 SensorCupOut (u8 Number );//检测底座无杯子函数

void SlaveControl(void);
void	SlaveData(void);
void  CupDetconfig(void);  
void CupDet(void);
void PumpControl();
u8  CheckSumTxd(u8 *dat,u8 length);
//void  CupDetconfig();
#endif