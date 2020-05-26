#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "AT24C256.h"
#include "USRK6.h"
#include "user.h"
#include "Slave.h"

int main(void)
{
	
//	uint8_t i=0;
	uint8_t temp=0;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	
	delay_init();	    	 //延时函数初始化	  
//	uart_init(115200);	 	//串口初始化为9600
	SlaveInit(115200);
	LED_Init();		  		//初始化与LED连接的硬件接口
	AT24C256Init();			//初始化IIC接口相关
	USRK6Init(115200);		//初始化以太网模块相关硬件


		temp = ConfigLimit(4321,1);//需要应答
		printf("%x",temp);
 
		temp = ConfigLimit(1234,0);//不需要需要应答
		printf("%x",temp);

	while(1)
	{
		delay_ms(300);
		LED0=!LED0;//DS0闪烁
//		if(UART5_RX_STA >= 5)
//		{
//			USART_SendString(USART1,UART5_RX_BUF);
//			ClearArray(UART5_RX_BUF,MaxRx5Len);//先清空接收区
//			UART5_RX_STA=0;//允许接收
//		}
	}
}
