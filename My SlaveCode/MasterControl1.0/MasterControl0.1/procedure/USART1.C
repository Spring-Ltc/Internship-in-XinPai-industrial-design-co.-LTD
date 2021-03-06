
/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU RC Demo -----------------------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ---------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* --- QQ:  800003751 ----------------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/


#include "USART1.h"


COMx_Define	COM1;
u8	 TX1_Buffer [COM_TX1_Lenth];	//发送缓冲
u8 	 RX1_Buffer [COM_RX1_Lenth];	//接收缓冲
//u8	TimeOutSet1;
u8 state=0;//状态机状态标志
u8 USART_Configuration(u8 UARTx, COMx_InitDefine *COMx)
{
	u8	i;
	u32	j;
	
	if(UARTx == USART1)
	{
		COM1.id = 1;
		COM1.TX_read    = 0;
		COM1.TX_write   = 0;
		COM1.B_TX_busy  = 0;
		COM1.RX_Cnt     = 0;
		COM1.RX_TimeOut = 0;
		COM1.B_RX_OK    = 0;
		for(i=0; i<COM_TX1_Lenth; i++)	TX1_Buffer[i] = 0;
		for(i=0; i<COM_RX1_Lenth; i++)	RX1_Buffer[i] = 0;

		if(COMx->UART_Mode > UART_9bit_BRTx)	return 1;	//模式错误
		if(COMx->UART_Polity == PolityHigh)		PS = 1;	//高优先级中断
		else									PS = 0;	//低优先级中断
		SCON = (SCON & 0x3f) | COMx->UART_Mode;
		if((COMx->UART_Mode == UART_9bit_BRTx) ||(COMx->UART_Mode == UART_8bit_BRTx))	//可变波特率
		{
			j = (MAIN_Fosc / 4) / COMx->UART_BaudRate;	//按1T计算
			if(j >= 65536UL)	return 2;	//错误
			j = 65536UL - j;
			if(COMx->UART_BRT_Use == BRT_Timer1)
			{
				TR1 = 0;
				AUXR &= ~0x01;		//S1 BRT Use Timer1;
				TMOD &= ~(1<<6);	//Timer1 set As Timer
				TMOD &= ~0x30;		//Timer1_16bitAutoReload;
				AUXR |=  (1<<6);	//Timer1 set as 1T mode
				TH1 = (u8)(j>>8);
				TL1 = (u8)j;
				ET1 = 0;	//禁止中断
				TMOD &= ~0x40;	//定时
				INT_CLKO &= ~0x02;	//不输出时钟
				TR1  = 1;
			}
			else if(COMx->UART_BRT_Use == BRT_Timer2)
			{
				AUXR &= ~(1<<4);	//Timer stop
				AUXR |= 0x01;		//S1 BRT Use Timer2;
				AUXR &= ~(1<<3);	//Timer2 set As Timer
				AUXR |=  (1<<2);	//Timer2 set as 1T mode
				TH2 = (u8)(j>>8);
				TL2 = (u8)j;
				IE2  &= ~(1<<2);	//禁止中断
				AUXR &= ~(1<<3);	//定时
				AUXR |=  (1<<4);	//Timer run enable
			}
			else return 2;	//错误
		}
		else if(COMx->UART_Mode == UART_ShiftRight)
		{
			if(COMx->BaudRateDouble == ENABLE)	AUXR |=  (1<<5);	//固定波特率SysClk/2
			else								AUXR &= ~(1<<5);	//固定波特率SysClk/12
		}
		else if(COMx->UART_Mode == UART_9bit)	//固定波特率SysClk*2^SMOD/64
		{
			if(COMx->BaudRateDouble == ENABLE)	PCON |=  (1<<7);	//固定波特率SysClk/32
			else								PCON &= ~(1<<7);	//固定波特率SysClk/64
		}
		if(COMx->UART_Interrupt == ENABLE)	ES = 1;	//允许中断
		else								ES = 0;	//禁止中断
		if(COMx->UART_RxEnable == ENABLE)	REN = 1;	//允许接收
		else								REN = 0;	//禁止接收
		P_SW1 = (P_SW1 & 0x3f) | (COMx->UART_P_SW & 0xc0);	//切换IO
		if(COMx->UART_RXD_TXD_Short == ENABLE)	PCON2 |=  (1<<4);	//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
		else									PCON2 &= ~(1<<4);
		return	0;
	}
	return 3;	//其它错误
}




void	UART_config(void)
{
 	COMx_InitDefine		COMx_InitStructure;					//
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//BRT_Timer1, BRT_Timer2 (×￠òa: ′??ú21ì?¨ê1ó?BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率可变 110 ~ 115200  115200ul
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//允许接收,   ENABLE?DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE?DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断控制,   ENABLE?òDISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P36_P37;	//端口切换,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(±?D?ê1ó??ú2?ê±?ó)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		// USART1,USART2

}
/*************** 装载串口发送缓冲 *******************************/

//void TX1_write2buff(u8 dat)	//写入发送缓冲，指针+1
//{
//	TX1_Buffer[COM1.TX_write] = dat;	//装发送缓冲
//	if(++COM1.TX_write >= COM_TX1_Lenth)	COM1.TX_write = 0;

//	if(COM1.B_TX_busy == 0)		//空闲
//	{  
//		COM1.B_TX_busy = 1;		//标志忙
//		TI = 1;					//触发发送中断
//	}
//}

//void PrintString1(u8 *puts)
//{
//    for (; *puts != 0;	puts++)  TX1_write2buff(*puts); 	//遇到停止符0结束
//}

/**************************************************************************
功能：STC15单片机的串口发送字节的函数
参数：dat:要发送的一个字节   
**************************************************************************/
void UART_Send_Byte(unsigned char dat)
{
		ES=0;		 // 使用查询发送结束方式，禁止中断干预  ES 串行中断 允许位
		SBUF = dat;
		while(!TI);	    
		TI=0;	 //此句可以不要，不影响后面数据的发送，只供代码查询数据是否发送完成
		ES=1;
}	

/********************* UART1中断函数************************/
void UART1_int (void) interrupt UART1_VECTOR
{


	u8 res;
	if(RI)//接收中断标志位
	{
		res = SBUF;
		RX1_Buffer[COM1.RX_Cnt++] = res;//存到缓存区
		if(COM1.RX_Cnt>=37)COM1.RX_Cnt=0;
		if(state != 2)	//状态：定时器未打开
		{
			COM1.RX_Cnt=0;
			switch (state)
			{
				case 0://未收到AA
				{
					if(res == 0xAA)
						state = 1;
					break;
				}
				case 1://收到AA
				{
					if(res == 0x55)
					{
						state =2;
						RX1_Buffer[0]=0xAA,RX1_Buffer[1]=0x55;//确认收到头部，手动添加到缓存数组
						COM1.RX_Cnt=2;//下一个数据从RX1_Buffer[2]开始缓存
						TR1 = 1;
						//COM1.RX_TimeOut = TimeOutSet1;//【开定时器】
					}
					else if(res == 0xAA)	state = 1;
					else state = 0;
					break;
				}
				default:	break;
			}
		}
		RI=0;//清中断标志位，准备接收下一个字节
	}
}



void SendDataLen(u8 *DataBuff,u8 len)
{
	u8 i=0;
	u8 *point = DataBuff;
	for(i=0;i<len;i++)
	{
		UART_Send_Byte(*(point+i));
	}
}
