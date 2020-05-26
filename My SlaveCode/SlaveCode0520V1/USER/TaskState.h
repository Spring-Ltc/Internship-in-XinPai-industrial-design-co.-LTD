

#ifndef __TaskState_H__
#define __TaskState_H__



#include "config.h"

void	GPIO_config(void);
void SelfCheckUp(void);
void TaskBasic(void);

void Delay1000ms();
void Delay50ms();
u8 SensorCupOut (u8 NumberN) ;
u8 SensorCup (u8 NumberN) ;
void WriteStateToSlaveDataBuff(void);
void PumpControl(void);


extern u8 CupUsedCounter;//一个程序周期检测到已经使用过的次数



#endif


