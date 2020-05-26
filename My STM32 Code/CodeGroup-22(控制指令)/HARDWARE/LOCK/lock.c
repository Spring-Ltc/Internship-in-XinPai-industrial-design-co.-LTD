#include  "lock.h" 
#include "hmi_handle.h"




void LockInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PC�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //PC.8 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOPC.8
	GPIO_ResetBits(GPIOC,GPIO_Pin_8);						 //PC.8 �����
}


void LockerTask(void)
{
//	if(HMITtoLKTFlag == 1)
//	{
//		if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_8) == 0)
//			GPIO_SetBits(GPIOC, GPIO_Pin_8);
//		else if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_8) == 1)
//			GPIO_ResetBits(GPIOC, GPIO_Pin_8);
//		HMITtoLKTFlag = 0;
//	}
	
}