#include  "lock.h" 
#include "hmi_handle.h"




void LockInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //PC.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOPC.8
	GPIO_ResetBits(GPIOC,GPIO_Pin_8);						 //PC.8 输出低
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