

/*----------------------------------------------------------------
2020/05/08修改：
	增加了退出AT模式、重启以太网模块、模块恢复出厂设置、AT指令查询配置参数这4个功能


------------------------------------------------------------------*/


#ifndef __USRK6_H__
#define __USRK6_H__

#include "sys.h"

#define MaxCommandLen 80	//AT指令最长字符数
#define MaxRx5Len 50	//
extern char UART5_RX_BUF[MaxRx5Len];//串口5接收缓存
extern u16 UART5_RX_STA;

void USRK6Init(u32 bound);
u8 EnterAT(void);//进入AT模式
u8 ExitAT(void );//退出AT模式
u8 Reboot(void);//重启模块
u8 ManufacturerReset(void);//恢复出厂设置
u8 ATControl(char *str,char *ack);
u8 ATQuery(char *Command,char *Respond);//查询配置参数

u8 ConfigWANN(char *Mode,char *Address,char *Mask,char *Gateway);
u8 ConfigUART(char *BaudRate,char *DataBits,char *StopBits,char *Parity,char *Flowctrl);
u8 ConfigSOCK(char *Protocol,char *IP,char *Port);









#endif


/*		部分函数使用示例

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


*/



