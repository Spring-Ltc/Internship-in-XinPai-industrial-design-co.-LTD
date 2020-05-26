


#include "led.h"
#include "timer.h"

u8 TIM4Flag500Ms2;


//----------------------------------函数说明--------------------------------------------
//函数功能：	LED初始化函数
//入口参数：	无
//返回值：		无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/02
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
void LEDKEY_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA,PD,PC端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED0-->PA.8 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
 GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PA.8 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED1-->PD.2 端口配置, 推挽输出
 GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 //PD.2 输出高 
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	    		 //LED10-->PC.0 端口配置, 推挽输出
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_ResetBits(GPIOC,GPIO_Pin_0); 						 //PC.0 输出高 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	    		 //LED11-->PC.1 端口配置, 推挽输出
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_1); 						 //PC.1 输出低
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED12-->PC.2 端口配置, 推挽输出
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_2); 						 //PC.2 输出高
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	    		 //LED13-->PC.3 端口配置, 推挽输出
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_3); 						 //PC.3 输出高

}






//----------------------------------函数说明--------------------------------------------
//函数功能：	LED和KEY的操作函数
//入口参数：	无
//返回值：		无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/16
//修改内容：初次完成基本功能
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
 
