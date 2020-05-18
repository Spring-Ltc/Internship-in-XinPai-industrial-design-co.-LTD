#ifndef __SLAVE_H__
#define __SLAVE_H__

#include "sys.h"

#define PortSlave UART4
#define UART4_RX_DMA_Channel  DMA2_Channel3	//定义串口4接收的DMA通道
#define UART4_TX_DMA_Channel  DMA2_Channel5 //定义串口4发送的DMA通道

#define UART4_DMA_DataLen   50		//定义串口4接收和发送的DMA缓存区长度


extern u8 UART4_RX_DMA_BUF[UART4_DMA_DataLen];//串口1接收DMA缓存
extern u8 UART4_TX_DMA_BUF[UART4_DMA_DataLen];//串口1发送DMA缓存
#define DMA_PortAddress  &UART4->DR	//DMA外设端口地址
#define DMA_ReceiveAddress  UART4_RX_DMA_BUF //DMA接收到这个地址
#define DMA_SendAddress  UART4_TX_DMA_BUF //DMA发送这个地址的数据

extern u8 SendBuff[UART4_DMA_DataLen];
extern u8 ReceiveBuff[UART4_DMA_DataLen];
extern u8 Flag_UART4_Solve;//串口4接收数据处理状态标志，为1表示接收到了未处理
extern u16 ReceiveLen;//当前收到的数据长度


#define MaxSendStr 30//发送到从控板的最大长度
#define MaxRxLen 50	//


void SlaveInit(u32 bound);
void SlaveUSART_Init(u32 bound);
void SlaveDMA_Init(void);
void TX_DMA_ENABLE(void);
void RX_DMA_ENABLE(void);


u8 ConfigLimit(u16 LimitNum);//设置从控板限制抽液次数，不需要应答
u8 ConfigUploadtime(u16 Period);//设置上传时间间隔等级
u16 QueryLimit(void);//询问剩余的限制次数
u8 QueryWorkState(u8 *Ackbuff);//询问从机工作状态
u8 ControlSlave(u8 *State);//控制从机工作状态
#endif


