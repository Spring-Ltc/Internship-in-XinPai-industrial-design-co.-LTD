
/*************	����˵��	**************

��������ʾ3����ʱ����ʹ�á�

��ʱ��0��16λ�Զ���װ10us��ʱ�����жϣ�
    �û������޸�ʱ��Ӷ��޸�Ƶ�ʣ�Ҳ���������жϣ���ע��ʱ�䲻Ҫ̫�̣�һ��10us���ϣ���

��ʱ��1��16λ�Զ���װ1ms��ʱ�� �жϣ�

��ʱ��2��16λ�Զ���װ20ms��ʱ���жϣ���

******************************************/

#include	"timer.h"
u16 T1_1MS1=0,T0_500MS2=0 ;
u16 T0_100MS1=0;
extern u8 TaskStateScanFlag,TaskUartFlag;
//extern u8	TimeOutSet1;

void	Timer_config(void)//��ʼ�� ��ʱ��`
{
	TIM_InitTypeDef		TIM_InitStructure;					// 
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;	// 16λ�Զ���װ  TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			// ���ȼ��� PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				// ʹ��  ENABLE?��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//ʹ��ʱ��1  TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//  �ɱ��ʱ�����ʹ�� ENABLE?��DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 500);		   //5MS 
	TIM_InitStructure.TIM_Run       = ENABLE;				//���� ENABLE?��DISABLE
	Timer_Inilize(Timer1,&TIM_InitStructure);				//  ��ʱ��1 Timer0,Timer1,Timer2

  TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE. (ע��: Timer2�̶�Ϊ16λ�Զ���װ, �жϹ̶�Ϊ�����ȼ�)
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / (100));		//��ֵ
	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer2,&TIM_InitStructure);				//��ʼ��Timer2	  Timer0,Timer1,Timer2
	
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//  ENABLE?��DISABLE.  
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//  TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				// ENABLE?��DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / (100));		// 120MS
	TIM_InitStructure.TIM_Run       = ENABLE;				//  ENABLE?��DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//   Timer0,Timer1,Timer2
}

//����ʱ���Ƭ
u8 timer10ms=0;
u8 timer200ms=0;
/********************* Timer0�жϺ���************************/
//void timer0_int (void) interrupt TIMER0_VECTOR
//{
//	timer10ms++;
//	if(timer10ms/20)
//	{
//		timer10ms=0;
//		timer200ms++;
//		TaskStateScanFlag=1;//��������Լ200ms����һ��
//	}
//	
//	if(MasterDataBuff[20] !=0 )//�����ϴ�
//	{
//		if(timer200ms >= MasterDataBuff[20])
//			TaskUploadFlag=1;//�ϴ�����
//	}
//	else	timer200ms=0;

//}


/////********************* Timer0�жϺ���************************/
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

/********************* Timer1�жϺ���************************/
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
//			if(COM1.RX_TimeOut==0)//������һ֡
//			{
//			 COM1.RcvFlag=0;
//			 COM1.RX_Cnt=0;
//			 RX1_Buffer[COM1.RX_Cnt]=0;
//			}
//		}
//		
//	 }
	
}


/********************* Timer2�жϺ���************************/
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
// ����: u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
// ����: ��ʱ����ʼ������.
// ����: TIMx: �ṹ����,��ο�timer.h��Ķ���.
// ����: �ɹ�����0, �ղ�������1,���󷵻�2.
// �汾: V1.1, 2020-5-7
//========================================================================
u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
{
	if(TIM > Timer2)	return 1;	//�ղ���

	if(TIM == Timer0)
	{
		TR0 = 0;		//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		ET0 = 1;	//�����ж�
		else									ET0 = 0;	//��ֹ�ж�
		if(TIMx->TIM_Polity == PolityHigh)		PT0 = 1;	//�����ȼ��ж�
		else									PT0 = 0;	//�����ȼ��ж�
		if(TIMx->TIM_Mode >  TIM_16BitAutoReloadNoMask)	return 2;	//����
		TMOD = (TMOD & ~0x03) | TIMx->TIM_Mode;	//����ģʽ,0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ, 3: 16λ�Զ���װ, ���������ж�
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	AUXR &= ~0x80;	//��ʱ��0���ٶ��Ǵ�ͳ8051��12��������Ƶ��12T��
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  0x80;	//��ʱ��1�Ǵ�ͳ8051�ٶȣ�12��Ƶ(1T)  AUXR(�����Ĵ���)
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	TMOD |=  0x04;	//����������Ƶ
		else										TMOD &= ~0x04;	//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x01;	//���ʱ��
		else							INT_CLKO &= ~0x01;	//�����ʱ��
		
		TH0 = (u8)(TIMx->TIM_Value >> 8);
		TL0 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	TR0 = 1;	//��ʼ����
		return	0;		//�ɹ�
	}

	if(TIM == Timer1)
	{
		TR1 = 0;		//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		ET1 = 1;	//�����ж�
		else									ET1 = 0;	//��ֹ�ж�
		if(TIMx->TIM_Polity == PolityHigh)		PT1 = 1;	//�����ȼ��ж�
		else									PT1 = 0;	//�����ȼ��ж�
		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//����
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode;	//����ģʽ,0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	AUXR &= ~0x40;	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  0x40;	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	TMOD |=  0x40;	//����������Ƶ
		else										TMOD &= ~0x40;	//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x02;	//���ʱ��
		else							INT_CLKO &= ~0x02;	//�����ʱ��
		
		TH1 = (u8)(TIMx->TIM_Value >> 8);
		TL1 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	TR1 = 1;	//��ʼ����
		return	0;		//�ɹ�
	}

	if(TIM == Timer2)		//Timer2,�̶�Ϊ16λ�Զ���װ, �ж������ȼ�
	{
		AUXR &= ~(1<<4);	//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)			IE2  |=  (1<<2);	//�����ж�
		else										IE2  &= ~(1<<2);	//��ֹ�ж�
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	AUXR &= ~(1<<2);	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  (1<<2);	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	AUXR |=  (1<<3);	//����������Ƶ
		else										AUXR &= ~(1<<3);	//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x04;	//���ʱ��
		else							INT_CLKO &= ~0x04;	//�����ʱ��

		TH2 = (u8)(TIMx->TIM_Value >> 8);
		TL2 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	AUXR |=  (1<<4);	//��ʼ����
		return	0;		//�ɹ�
	}
	return 2;	//����
}
