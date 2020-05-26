
/*************	功能说明	**************

本程序演示3个定时器的使用。

定时器0做16位自动重装10us定时，不中断，
    用户可以修改时间从而修改频率，也可以允许中断，但注意时间不要太短（一般10us以上）。

定时器1做16位自动重装1ms定时， 中断，

定时器2做16位自动重装20ms定时，中断，。

******************************************/

#include	"timer.h"
u16 T1_1MS1=0,T0_500MS2=0 ;
u16 T0_100MS1=0;
extern u8 TaskStateScanFlag,TaskUartFlag;
//extern u8	TimeOutSet1;

void	Timer_config(void)//初始化 定时器`
{
	TIM_InitTypeDef		TIM_InitStructure;					// 
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;	// 16位自动重装  TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			// 优先级低 PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				// 使能  ENABLE?òDISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//使用时钟1  TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//  可编程时钟输出使能 ENABLE?òDISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 500);		   //5MS 
	TIM_InitStructure.TIM_Run       = ENABLE;				//运行 ENABLE?òDISABLE
	Timer_Inilize(Timer1,&TIM_InitStructure);				//  定时器1 Timer0,Timer1,Timer2

  TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE. (注意: Timer2固定为16位自动重装, 中断固定为低优先级)
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / (100));		//初值
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer2,&TIM_InitStructure);				//初始化Timer2	  Timer0,Timer1,Timer2
	
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//  ENABLE?òDISABLE.  
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//  TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				// ENABLE?òDISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / (100));		// 120MS
	TIM_InitStructure.TIM_Run       = ENABLE;				//  ENABLE?òDISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//   Timer0,Timer1,Timer2
}

//任务时间分片
u8 timer10ms=0;
u8 timer200ms=0;
/********************* Timer0中断函数************************/
//void timer0_int (void) interrupt TIMER0_VECTOR
//{
//	timer10ms++;
//	if(timer10ms/20)
//	{
//		timer10ms=0;
//		timer200ms++;
//		TaskStateScanFlag=1;//基础任务，约200ms允许一次
//	}
//	
//	if(MasterDataBuff[20] !=0 )//允许上传
//	{
//		if(timer200ms >= MasterDataBuff[20])
//			TaskUploadFlag=1;//上传任务
//	}
//	else	timer200ms=0;

//}


/////********************* Timer0中断函数************************/
 void timer0_int (void) interrupt TIMER0_VECTOR
 {
	//TaskUartFlag++;
	T0_100MS1++;

	if(T0_100MS1>=10)
	{
		T0_100MS1=0;
		T0_500MS2++;
		TaskStateScanFlag=1;
		if(T0_500MS2>=5)
	  {
	   T0_500MS2=0;
    }
		
   }
	 	
 
 }

/********************* Timer1中断函数************************/
void timer1_int (void) interrupt TIMER1_VECTOR
{ 
  
	TaskUartFlag=1;
	TH1=(u8)(65536UL - (MAIN_Fosc / 200))/256;
	TL1=(u8)(65536UL - (MAIN_Fosc / 200))%256;
	
 TR1 = 0;
//	T1_1MS1++;
//	T1_10MS2++;
//	 
//	if(T1_10MS2>=10)//1
//    {
//			 T1_10MS2=0;
//			 
//			
//    }
//  if ((T1_1MS1)>=100)     //MilliSecond
//	{ 
//	
//		T1_1MS1=0;     //1MS Timer
//		
//  }
//	if(COM1.RcvFlag==1)
//   {
//		 if (COM1.RX_TimeOut >0)		//==
//		{
//			--COM1.RX_TimeOut;
//			if(COM1.RX_TimeOut==0)//接收完一帧
//			{
//			 COM1.RcvFlag=0;
//			 COM1.RX_Cnt=0;
//			 RX1_Buffer[COM1.RX_Cnt]=0;
//			}
//		}
//		
//	 }
	
}


/********************* Timer2中断函数************************/
//void timer2_int (void) interrupt TIMER2_VECTOR	
//{
// 		if(T2_10MS2>=10)
//	{
//	   T2_10MS2=0;
//   } 
//		 
//	// }
//	

//}



//========================================================================
// 函数: u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
// 描述: 定时器初始化程序.
// 参数: TIMx: 结构参数,请参考timer.h里的定义.
// 返回: 成功返回0, 空操作返回1,错误返回2.
// 版本: V1.1, 2020-5-7
//========================================================================
u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
{
	if(TIM > Timer2)	return 1;	//空操作

	if(TIM == Timer0)
	{
		TR0 = 0;		//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		ET0 = 1;	//允许中断
		else									ET0 = 0;	//禁止中断
		if(TIMx->TIM_Polity == PolityHigh)		PT0 = 1;	//高优先级中断
		else									PT0 = 0;	//低优先级中断
		if(TIMx->TIM_Mode >  TIM_16BitAutoReloadNoMask)	return 2;	//错误
		TMOD = (TMOD & ~0x03) | TIMx->TIM_Mode;	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	AUXR &= ~0x80;	//定时器0的速度是传统8051的12倍，不分频（12T）
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  0x80;	//定时器1是传统8051速度，12分频(1T)  AUXR(辅助寄存器)
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	TMOD |=  0x04;	//对外计数或分频
		else										TMOD &= ~0x04;	//定时
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x01;	//输出时钟
		else							INT_CLKO &= ~0x01;	//不输出时钟
		
		TH0 = (u8)(TIMx->TIM_Value >> 8);
		TL0 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	TR0 = 1;	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer1)
	{
		TR1 = 0;		//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		ET1 = 1;	//允许中断
		else									ET1 = 0;	//禁止中断
		if(TIMx->TIM_Polity == PolityHigh)		PT1 = 1;	//高优先级中断
		else									PT1 = 0;	//低优先级中断
		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//错误
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode;	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	AUXR &= ~0x40;	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  0x40;	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	TMOD |=  0x40;	//对外计数或分频
		else										TMOD &= ~0x40;	//定时
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x02;	//输出时钟
		else							INT_CLKO &= ~0x02;	//不输出时钟
		
		TH1 = (u8)(TIMx->TIM_Value >> 8);
		TL1 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	TR1 = 1;	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer2)		//Timer2,固定为16位自动重装, 中断无优先级
	{
		AUXR &= ~(1<<4);	//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)			IE2  |=  (1<<2);	//允许中断
		else										IE2  &= ~(1<<2);	//禁止中断
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	AUXR &= ~(1<<2);	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  (1<<2);	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	AUXR |=  (1<<3);	//对外计数或分频
		else										AUXR &= ~(1<<3);	//定时
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x04;	//输出时钟
		else							INT_CLKO &= ~0x04;	//不输出时钟

		TH2 = (u8)(TIMx->TIM_Value >> 8);
		TL2 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	AUXR |=  (1<<4);	//开始运行
		return	0;		//成功
	}
	return 2;	//错误
}
