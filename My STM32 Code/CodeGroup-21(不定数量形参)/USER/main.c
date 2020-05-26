#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "AT24C256.h"
#include "USRK6.h"
#include "user.h"
#include "Slave.h"




#define MaxAckLen 50
int main(void)
{
	
//	uint8_t i=0;
	uint8_t temp=0;
	char ackbuff[MaxAckLen] ={0};
	u8 controldata[23]={1,1,1,1,1,   0,0,0,0,0,
											2,2,2,2,2,   1,1,1,1,1,  0,1,2};
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	
	delay_init();	    	 //��ʱ������ʼ��	  
//	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
	SlaveInit(115200);
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	AT24C256Init();			//��ʼ��IIC�ӿ����
	USRK6Init(115200);		//��ʼ����̫��ģ�����Ӳ��

											
			ManufacturerReset();	//�ָ���������	
		temp=EnterAT();
		if(temp == 0)
			USART_SendString(UART4,"Enter AT Mode OK\r\n");
		else
			USART_SendString(UART4,"Enter AT Mode Failed\r\n");//����ATģʽ
		
		ClearArray(ackbuff,MaxAckLen);
		USART_SendString(UART4,"\r\n 1th Config WANN ack:\r\n");//����ģ��ģ�IP���͡���̬IP��ַ���������롢���أ�
		temp=ATControl("AT+WANN=STATIC,192.168.0.5,255.255.255.0,192.168.0.1\r\n",ackbuff);
		if(temp)
			USART_SendString(UART4,"Config Error\r\n");
		else
			USART_SendString(UART4,ackbuff);

		ClearArray(ackbuff,MaxAckLen);
		USART_SendString(UART4,"\r\n 2th Config UART ack:\r\n");//����ģ��Ĵ��ڲ����������ʡ�����λ��ֹͣλ��У��λ��NFC��
		temp=ATControl("AT+UART=115200,8,1,NONE,NFC\r\n",ackbuff);
		if(temp)
			USART_SendString(UART4,"Config Error\r\n");
		else
			USART_SendString(UART4,ackbuff);
		
		ClearArray(ackbuff,MaxAckLen);
		USART_SendString(UART4,"\r\n 3th Config SOCK ack:\r\n");//����ģ���SOCK��ģ�鹤����ʽ��Ŀ��IP��Զ�̶˿ڣ�
		temp=ATControl("AT+SOCK=TCPC,192.168.0.117,40\r\n",ackbuff);
		if(temp)
			USART_SendString(UART4,"Config Error\r\n");
		else
			USART_SendString(UART4,ackbuff);
		
	
		ClearArray(ackbuff,MaxAckLen);
		temp=ATControl("AT+ENTM\r\n",ackbuff);//�˳�ATģʽ,�˳��ɹ����� +OK
		USART_SendString(UART4,ackbuff);	
		if(temp == 0)
			USART_SendString(UART4,"Exit AT Mode OK\r\n");
		else
			USART_SendString(UART4,"Exit AT Mode Failed\r\n");							
											
											
											
											
											
		//USART_SendString(UART4,"Tese\r\n");									

//		temp = ConfigLimit(4321,1);//��ҪӦ��				//	AA 55 06 20 21 01 57 10 E1 A0 55 AA
//		//printf("%x",temp);
											
		temp = ConfigUploadtime(10000,0);						//	AA 55 06 20 21 00 73 0A 00 7E 55 AA
//		printf("%x",temp);
//											
//		temp = QueryLimit(1);												//	AA 55 06 20 21 01 58 FF FF 5E 55 AA
////		printf("%x",temp);
//											
//		temp = QueryWorkState(1);										//	AA 55 06 20 21 01 72 FF FF 74 55 AA 
////		printf("%x",temp);
//		
//		temp = ControlSlave(controldata,1);// AA 55 1B 20 21 01 55 01 01 01 01 01 00 00 00 00 00 02 02 02 02 02 01 01 01 01 01 00 01 02 4F 55 AA 
////		printf("%x",temp);
	
	
	
//	temp = MyFunction("AT+WANN=","SCIENCE,192.168.1.1");
//	
//	temp = MyFunction("AT+WANN=","SCIENCE,192.168.1.1",46,89);
	while(1)
	{
		delay_ms(300);
		LED0=!LED0;//DS0��˸
		if(UART4_RX_STA >= 10)
		{
			USART_SendString(UART5,UART4_RX_BUF);
			ClearArray(UART4_RX_BUF,MaxRx5Len);//����ս�����
			UART4_RX_STA=0;//�������
		}
	}
}
