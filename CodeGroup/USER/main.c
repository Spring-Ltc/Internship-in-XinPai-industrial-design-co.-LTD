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

	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	AT24C256Init();			//��ʼ��IIC�ӿ����
	USRK6Init(115200);		//��ʼ����̫��ģ�����Ӳ��
	
	for(i=0;i<10;i++)	//���������鸳ֵ
		databuff1[i]=i+'0';
	for(i=0;i<10;i++)
		databuff2[i]=i+'A';
	for(i=0;i<10;i++)
		databuff3[i]=i+'a';	
	
		WriteLenByte(DataAddress0,databuff1,100);
		WriteLenByte(DataAddress1,databuff2,100);
		WriteLenByte(DataAddress2,databuff3,100);	//д������
		LED1_ON;//��ʾд�����
		
		ReadLenByte(DataAddress0,readbuff1,50);
		ReadLenByte(DataAddress1,readbuff2,50);
		ReadLenByte(DataAddress2,readbuff3,50);
		LED2_ON;//��ʾ��ȡ���
		
		USART_SendLenData(USART1,readbuff1,10);
		USART_SendLenData(USART1,readbuff2,10);
		USART_SendLenData(USART1,readbuff3,10);


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
	
		
		
	ClearArray(UART5_RX_BUF,MaxRx5Len);//����ս�����
	UART5_RX_STA=0;//�������		
	while(1)
	{
		delay_ms(300);
		LED0=!LED0;//DS0��˸
		

		if(UART5_RX_STA >= 5)
		{
			USART_SendString(USART1,UART5_RX_BUF);
			ClearArray(UART5_RX_BUF,MaxRx5Len);//����ս�����
			UART5_RX_STA=0;//�������
		}
	}
}
