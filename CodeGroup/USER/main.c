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
	uint8_t databuff1[100] ={0};
	uint8_t databuff2[100] ={0};
	uint8_t databuff3[100] ={0};
	
	uint8_t readbuff1[100] ={0};
	uint8_t readbuff2[100] ={0};
	uint8_t readbuff3[100] ={0};
	
	char ackbuff[50] = {0};
	u8 MaxAckLen=50;

	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	
	delay_init();	    	 //延时函数初始化	  
	uart_init(115200);	 	//串口初始化为9600
	LED_Init();		  		//初始化与LED连接的硬件接口
	AT24C256Init();			//初始化IIC接口相关
	USRK6Init(115200);		//初始化以太网模块相关硬件
	
	for(i=0;i<10;i++)	//给数据数组赋值
		databuff1[i]=i+'0';
	for(i=0;i<10;i++)
		databuff2[i]=i+'A';
	for(i=0;i<10;i++)
		databuff3[i]=i+'a';	
	
		WriteLenByte(DataAddress0,databuff1,100);
		WriteLenByte(DataAddress1,databuff2,100);
		WriteLenByte(DataAddress2,databuff3,100);	//写入数据
		LED1_ON;//提示写入完毕
		
		ReadLenByte(DataAddress0,readbuff1,50);
		ReadLenByte(DataAddress1,readbuff2,50);
		ReadLenByte(DataAddress2,readbuff3,50);
		LED2_ON;//提示读取完毕
		
		USART_SendLenData(USART1,readbuff1,10);
		USART_SendLenData(USART1,readbuff2,10);
		USART_SendLenData(USART1,readbuff3,10);


		temp=EnterAT();
		if(temp == 0)
			USART_SendString(USART1,"Enter AT Mode OK\r\n");
		else
			USART_SendString(USART1,"Enter AT Mode Failed\r\n");//进入AT模式
		
		ClearArray(ackbuff,MaxAckLen);
		USART_SendString(USART1,"\r\n 1th Config ack:\r\n");//配置模块的（IP类型、静态IP地址、子网掩码、网关）
		temp=ATControl("AT+WANN=STATIC,192.168.0.5,255.255.255.0,192.168.0.1\r\n",ackbuff);
		USART_SendString(USART1,ackbuff);

		ClearArray(ackbuff,MaxAckLen);
		USART_SendString(USART1,"\r\n 2th Config ack:\r\n");//配置模块的串口参数（波特率、数据位、停止位、校验位、NFC）
		temp=ATControl("AT+UART=115200,8,1,NONE,NFC\r\n",ackbuff);
		USART_SendString(USART1,ackbuff);
		
		ClearArray(ackbuff,MaxAckLen);
		USART_SendString(USART1,"\r\n 3th Config ack:\r\n");//配置模块的SOCK（模块工作方式、目标IP、远程端口）
		temp=ATControl("AT+SOCK=TCPC,192.168.0.117,40\r\n",ackbuff);
		USART_SendString(USART1,ackbuff);	
		
	
		ClearArray(ackbuff,MaxAckLen);
		temp=ATControl("AT+ENTM\r\n",ackbuff);//退出AT模式,退出成功返回 +OK
		USART_SendString(USART1,ackbuff);	
		if(temp == 0)
			USART_SendString(USART1,"Exit AT Mode OK\r\n");
		else
			USART_SendString(USART1,"Exit AT Mode Failed\r\n");
	
		
		
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
