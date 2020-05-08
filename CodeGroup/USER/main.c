#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "AT24C256.h"
#include "USRK6.h"
#include "user.h"


int main(void)
{
	
	uint8_t i=0;
	uint8_t temp=0;
	char ackbuff[50] = {0};

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	
	delay_init();	    	 //延时函数初始化	  
	uart_init(115200);	 	//串口初始化为9600
	LED_Init();		  		//初始化与LED连接的硬件接口
	AT24C256Init();			//初始化IIC接口相关
	USRK6Init(115200);		//初始化以太网模块相关硬件

	

		temp = ExitAT();//退出AT模式
		if(temp)
			USART_SendString(USART1,"Exit AT Failed\r\n");//先执行一次退出,其实没有必要
		else
			USART_SendString(USART1,"Exit AT OK\r\n");
		
		delay_ms(500);
		
		temp = EnterAT();//切换到AT模式
		if(temp == 1)
			USART_SendString(USART1,"Enter AT Failed\r\n");//进入失败
		else//进入成功才进行配置
		{
			USART_SendString(USART1,"Enter AT OK\r\n");
			temp = ConfigSOCK("TCPC","192.168.0.117","40");//配置SOCK
			if(temp == 0)
				USART_SendString(USART1,"Config SOCK OK\r\n");
			else
			{
				USART_SendString(USART1,"Config SOCK Failed,");
				printf("Error = %c\r\n",temp);
			}
			
			temp = ConfigWANN("STATIC","192.168.0.5","255.255.255.0","192.168.0.1");//配置WANN
			if(temp == 0)
				USART_SendString(USART1,"Config WANN OK\r\n");
			else
			{
				USART_SendString(USART1,"Config WANN Failed,");
				printf("Error = %c\r\n",temp);
			}
			
			temp = ConfigUART("115200","8","1","NONE","NFC");//配置UART
			if(temp == 0)
				USART_SendString(USART1,"Config UART OK\r\n");
			else
			{
				USART_SendString(USART1,"Config UART Failed,");
				printf("Error = %c\r\n",temp);
			}
			
		}
		
		temp = ATQuery("UART",ackbuff);//查询串口配置
		printf("\r\n%s\r\n",ackbuff);//通过串口1输出配置信息方便查看
		
		temp = ATQuery("WANN",ackbuff);//查询WANN配置
		printf("\r\n%s\r\n",ackbuff);
		
		temp = ATQuery("SOCK",ackbuff);//查询SOCK配置
		printf("\r\n%s\r\n",ackbuff);
		
		temp = ExitAT();
		if(temp)
			printf("Failed, Errer is %c\r\n",temp);//退出之后还需要重启模块才能正常工作
		temp = Reboot();
		if(temp)
			printf("Failed, Errer is %c\r\n",temp);
		
	ClearArray(UART5_RX_BUF,MaxRx5Len);//先清空接收区
	UART5_RX_STA=0;//允许接收		
	while(1)
	{
		delay_ms(300);
		LED0=!LED0;//DS0闪烁
		if(UART5_RX_STA >= 5)
		{
			USART_SendString(USART1,UART5_RX_BUF);
			ClearArray(UART5_RX_BUF,MaxRx5Len);//先清空接收区
			UART5_RX_STA=0;//允许接收
		}
	}
}
