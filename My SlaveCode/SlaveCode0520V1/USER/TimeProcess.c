

#include "TimeProcess.h"
#include "timer.h"
#include "TaskUart.h"



void Timer0Config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//�ṹ����
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityHigh;			//ָ���ж����ȼ�, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 1200);		//��ֵ,	10ms
	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//��ʼ��Timer0	  Timer0,Timer1,Timer2
}


void Timer1Config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//�ṹ����
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//ָ���ж����ȼ�, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 100);		//��ֵ,10ms
	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer1,&TIM_InitStructure);				//��ʼ��Timer0	  Timer0,Timer1,Timer2

}


//����ʱ���Ƭ
u8 timer10ms=0;
u8 timer500ms=0;
/********************* Timer0�жϺ���************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	timer10ms++;
	if(timer10ms>=10)
	{
		timer10ms=0;
		timer500ms++;
		TaskStateScanFlag=1;//��������Լ100ms����һ��
	}
	if(MasterDataBuff[MA_time] !=0 )//�����ϴ�
	{
		if(timer500ms >= 5 * MasterDataBuff[MA_time])
		{
			TaskUploadFlag=1;//�ϴ�����Լ500ms X ���� ����һ��
			timer500ms=0;
		}
	}
	else	timer500ms=0;
}

//���ڳ�ʱ������
/********************* Timer1�жϺ���************************/
void timer1_int (void) interrupt TIMER1_VECTOR
{
	TR1 = 0;//�ض���ʱ��
	TaskUartFlag = 1;
//	TH1 = (u8)((65536UL - (MAIN_Fosc / 100)) >> 8);
//	TL1 = (u8)(65536UL - (MAIN_Fosc / 100));	//��װ��ֵ,10ms
}

