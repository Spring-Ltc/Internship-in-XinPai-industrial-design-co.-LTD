

#include "TimeProcess.h"
#include "timer.h"
#include "TaskUart.h"



void Timer0Config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityHigh;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 1200);		//初值,	10ms
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2
}


void Timer1Config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 100);		//初值,10ms
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer1,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2

}


//任务时间分片
u8 timer10ms=0;
u8 timer500ms=0;
/********************* Timer0中断函数************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	timer10ms++;
	if(timer10ms>=10)
	{
		timer10ms=0;
		timer500ms++;
		TaskStateScanFlag=1;//基础任务，约100ms允许一次
	}
	if(MasterDataBuff[MA_time] !=0 )//允许上传
	{
		if(timer500ms >= 5 * MasterDataBuff[MA_time])
		{
			TaskUploadFlag=1;//上传任务，约500ms X 倍数 允许一次
			timer500ms=0;
		}
	}
	else	timer500ms=0;
}

//串口超时接收用
/********************* Timer1中断函数************************/
void timer1_int (void) interrupt TIMER1_VECTOR
{
	TR1 = 0;//关定定时器
	TaskUartFlag = 1;
//	TH1 = (u8)((65536UL - (MAIN_Fosc / 100)) >> 8);
//	TL1 = (u8)(65536UL - (MAIN_Fosc / 100));	//重装初值,10ms
}

