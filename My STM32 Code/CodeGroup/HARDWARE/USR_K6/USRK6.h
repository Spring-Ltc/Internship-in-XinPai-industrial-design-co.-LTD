

#ifndef __USRK6_H__
#define __USRK6_H__

#include "sys.h"


#define MaxRx5Len 50
extern char UART5_RX_BUF[MaxRx5Len];//����5���ջ���
extern u16 UART5_RX_STA;

void USRK6Init(u32 bound);
u8 EnterAT(void);
u8 ATControl(char *str,char *ack);

#endif


/*		���ֺ���ʹ��ʾ��

		temp=EnterAT();
		if(temp == 0)
			USART_SendString(USART1,"Enter AT Mode OK\r\n");
		else
			USART_SendString(USART1,"Enter AT Mode Failed\r\n");//����ATģʽ
		
		ClearArray(ackbuff,MaxAckLen);
		USART_SendString(USART1,"\r\n 1th Config ack:\r\n");//����ģ��ģ�IP���͡���̬IP��ַ���������롢���أ�
		temp=ATControl("AT+WANN=STATIC,192.168.0.5,255.255.255.0,192.168.0.1\r\n",ackbuff);
		USART_SendString(USART1,ackbuff);

		ClearArray(ackbuff,MaxAckLen);
		USART_SendString(USART1,"\r\n 2th Config ack:\r\n");//����ģ��Ĵ��ڲ����������ʡ�����λ��ֹͣλ��У��λ��NFC��
		temp=ATControl("AT+UART=115200,8,1,NONE,NFC\r\n",ackbuff);
		USART_SendString(USART1,ackbuff);
		
		ClearArray(ackbuff,MaxAckLen);
		USART_SendString(USART1,"\r\n 3th Config ack:\r\n");//����ģ���SOCK��ģ�鹤����ʽ��Ŀ��IP��Զ�̶˿ڣ�
		temp=ATControl("AT+SOCK=TCPC,192.168.0.117,40\r\n",ackbuff);
		USART_SendString(USART1,ackbuff);	
		
	
		ClearArray(ackbuff,MaxAckLen);
		temp=ATControl("AT+ENTM\r\n",ackbuff);//�˳�ATģʽ,�˳��ɹ����� +OK
		USART_SendString(USART1,ackbuff);	
		if(temp == 0)
			USART_SendString(USART1,"Exit AT Mode OK\r\n");
		else
			USART_SendString(USART1,"Exit AT Mode Failed\r\n");


*/



