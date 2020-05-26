#ifndef __SLAVE_H__
#define __SLAVE_H__
#include "stdio.h"	
#include "sys.h" 

#include "user.h"

#define PortSlave UART4
#define EN_UART4_RX 			1		//使能（1）/禁止（0）串口1接收

#define DMA_Rec_Len 40
#define DMA_Tx_Len 40
	  	
extern u8 BoradSlaveBuf[29];//【串口屏从这里读取从控板的状态数据，从控板负责更新这里的数据】

void SlaveInit(u32 bound);
void BoradSlaveTask(void);


u8 ConfigLimit(u16 LimitNum);//设置从控板限制抽液次数，不需要应答
u8 ConfigUploadtime(u16 Period);//设置上传时间间隔等级
u16 QueryLimit(void);//询问剩余的限制次数
u8 QueryWorkState(u8 *Ackbuff);//询问从机工作状态
u8 ControlSlave(u8 *State);//控制从机工作状态


#endif









