#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "AT24C256.h"
#include "USRK6.h"
#include "user.h"
#include "Slave.h"

void USRK6Config(void);




int main(void)
{
	
	uint8_t i=0;
	u16 temp=0;
#include "stm32f10x.h"                  // Device header
	u8 ackbuf[30]={0};
	u8 writebuf[30]={0};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	
	delay_init();	    	 //延时函数初始化	  
	
	LED_Init();		  		//初始化与LED连接的硬件接口
	AT24C256Init();			//初始化IIC接口相关
	USRK6Init(115200);		//初始化以太网模块相关硬件
	LOCK_Init();
	SlaveInit(9600);
	

	USART_SendString(UART5,"AT24C256 Text:\r\n");
	ReadLenByte(DataAddress1,ackbuf,25);
	USART_SendLenData(UART5,ackbuf,30);
	USART_SendString(UART5," \r\n\r\n");
	delay_ms(200);
	USART_SendString(UART5,"\r\nDMA Text \r\n\r\n");delay_ms(200);
	
	//ConfigLimit(2345);
	
	temp = QueryLimit();
	if(temp == 0xFFFF)
		USART_SendString(UART5,"\r\nQuery Error!\r\n\r\n");
	else
	{
		USART_SendString(UART5,"\r\nQuery OK!\r\n\r\n");
		USART_SendData(UART5,temp);
	}
	
	Flag_UART4_Solve=0;
	ClearArray_u8(ReceiveBuff,200);
	while(1)
	{
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
		if(Flag_UART4_Solve)//收到数据，待处理
		{
			//USART_SendString(UART5,ReceiveBuff);
			temp = DataValidityCheck(ReceiveBuff);
			if(temp == 0)
			{
					USART_SendString(UART5,"\r\nCheck OK!\r\n");
					SendACK(UART4,ReceiveBuff[4],ReceiveBuff);
			}
			if(temp == 1)
					USART_SendString(UART5,"\r\nCheck Error! Header Error!\r\n");
			if(temp == 2)
					USART_SendString(UART5,"\r\nCheck Error! Tail Error!\r\n");
			if(temp == 3)
					USART_SendString(UART5,"\r\nCheck Error! Parity Error!\r\n");
			
			Flag_UART4_Solve=0;//清除标志
			ClearArray_u8(ReceiveBuff,200);
		}
	}
}



//配置以太网模块，测试用
void USRK6Config(void)
{
	u8 temp=11;

	ManufacturerReset();	//恢复出厂设置
	delay_ms(500);
		
		temp = EnterAT();//切换到AT模式
		if(temp)
		{
			USART_SendString(UART4,"Enter AT Failed\r\n");//进入失败
			return;//进入AT模式失败，打印信息后直接退出
		}
		else
		{
		//进入AT模式成功，下面开始配置相关参数
			USART_SendString(UART4,"Enter AT OK\r\n");
			
			temp = ConfigSOCK("TCPS","192.168.0.117","40");//配置SOCK
			if(temp == 0)
				USART_SendString(UART4,"Config SOCK OK\r\n");
			else
			{
				USART_SendString(UART4,"Config SOCK Failed,\r\n");
//				printf("Error = %c\r\n",temp);
			}
			
			
			temp = ConfigWANN("STATIC","192.168.0.117","255.255.255.0","192.168.0.1");//配置WANN
			if(temp == 0)
				USART_SendString(UART4,"Config WANN OK\r\n");
			else
			{
				USART_SendString(UART4,"Config WANN Failed,\r\n");
//				printf("Error = %c\r\n",temp);
			}
			
			
			temp = ConfigUART("115200","8","1","NONE","NFC");//配置UART
			if(temp == 0)
				USART_SendString(UART4,"Config UART OK\r\n");
			else
			{
				USART_SendString(UART4,"Config UART Failed,\r\n");
//				printf("Error = %c\r\n",temp);
			}

		temp = Reboot();//退出之后还需要重启模块才能正常工作
		if(temp)
			USART_SendString(UART4,"Reboot Failed\r\n");
		
	}
			
}





