#ifndef __SLAVE_H__
#define __SLAVE_H__
#include "stdio.h"	
#include "sys.h" 

#define EN_UART4_RX 			1		//使能（1）/禁止（0）串口1接收

#define DMA_Rec_Len 40
#define DMA_Tx_Len 40
	  	
extern u8 BoradSlaveBuf[35];

extern u8 RLED1State, RLED3State, RLED5State, GLED1State, GLED3State, GLED5State;
extern u8 Cup1State, Cup3State, Cup5State;
extern u8 Water1SensorState, Water3SensorState, Water5SensorState, WaterSensorPowerState;
extern u8 PUMP1State, PUMP3State, PUMP5State;
extern u8 VALVE1State, VALVE3State, VALVE5State;
extern u8 LockState;
extern u8 SetUploadTimeInterval;
extern u16 ReportRemainLimits;

void SlaveInit(u32 bound);
void BoradSlaveTask(void);
void BoradSlaveBufDeal(void);

extern int BSTtoHMITGetSensorStateSuccessFlag;



void QueryWorkState(void);
void ConfigUploadtime(u8 Period);
void ConfigLimit(u8 *LimitNum);
void ControlSlave(u8 CUPx,u8 CupDisEnable,u8 PumpDisEnable);

#endif



























