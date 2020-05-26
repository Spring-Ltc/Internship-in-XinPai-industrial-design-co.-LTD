


#include "led.h"
#include "timer.h"

u8 TIM4Flag500Ms2;


//----------------------------------����˵��--------------------------------------------
//�������ܣ�	LED��ʼ������
//��ڲ�����	��
//����ֵ��		��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/02
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void LEDKEY_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA,PD,PC�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED0-->PA.8 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
 GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PA.8 �����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED1-->PD.2 �˿�����, �������
 GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 //PD.2 ����� 
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	    		 //LED10-->PC.0 �˿�����, �������
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_ResetBits(GPIOC,GPIO_Pin_0); 						 //PC.0 ����� 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	    		 //LED11-->PC.1 �˿�����, �������
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_1); 						 //PC.1 �����
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED12-->PC.2 �˿�����, �������
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_2); 						 //PC.2 �����
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	    		 //LED13-->PC.3 �˿�����, �������
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_3); 						 //PC.3 �����

}






//----------------------------------����˵��--------------------------------------------
//�������ܣ�	LED��KEY�Ĳ�������
//��ڲ�����	��
//����ֵ��		��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/16
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void LedKeyTask(void)
{
	if(TIM4Flag10Ms2 == 1)
	{
		TIM4Flag10Ms2 = 0;
		if((TIM4Flag500Ms2++) >=50 )
		{
			TIM4Flag500Ms2 = 0;
			LED10=!LED10;
		}
		
//		TIM4Flag = 0;
	}
}
 
