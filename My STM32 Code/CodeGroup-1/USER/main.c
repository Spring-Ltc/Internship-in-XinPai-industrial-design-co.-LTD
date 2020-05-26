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

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	AT24C256Init();			//��ʼ��IIC�ӿ����
	USRK6Init(115200);		//��ʼ����̫��ģ�����Ӳ��

	

		temp = ExitAT();//�˳�ATģʽ
		if(temp)
			USART_SendString(USART1,"Exit AT Failed\r\n");//��ִ��һ���˳�,��ʵû�б�Ҫ
		else
			USART_SendString(USART1,"Exit AT OK\r\n");
		
		delay_ms(500);
		
		temp = EnterAT();//�л���ATģʽ
		if(temp == 1)
			USART_SendString(USART1,"Enter AT Failed\r\n");//����ʧ��
		else//����ɹ��Ž�������
		{
			USART_SendString(USART1,"Enter AT OK\r\n");
			temp = ConfigSOCK("TCPC","192.168.0.117","40");//����SOCK
			if(temp == 0)
				USART_SendString(USART1,"Config SOCK OK\r\n");
			else
			{
				USART_SendString(USART1,"Config SOCK Failed,");
				printf("Error = %c\r\n",temp);
			}
			
			temp = ConfigWANN("STATIC","192.168.0.5","255.255.255.0","192.168.0.1");//����WANN
			if(temp == 0)
				USART_SendString(USART1,"Config WANN OK\r\n");
			else
			{
				USART_SendString(USART1,"Config WANN Failed,");
				printf("Error = %c\r\n",temp);
			}
			
			temp = ConfigUART("115200","8","1","NONE","NFC");//����UART
			if(temp == 0)
				USART_SendString(USART1,"Config UART OK\r\n");
			else
			{
				USART_SendString(USART1,"Config UART Failed,");
				printf("Error = %c\r\n",temp);
			}
			
		}
		
		temp = ATQuery("UART",ackbuff);//��ѯ��������
		printf("\r\n%s\r\n",ackbuff);//ͨ������1���������Ϣ����鿴
		
		temp = ATQuery("WANN",ackbuff);//��ѯWANN����
		printf("\r\n%s\r\n",ackbuff);
		
		temp = ATQuery("SOCK",ackbuff);//��ѯSOCK����
		printf("\r\n%s\r\n",ackbuff);
		
		temp = ExitAT();
		if(temp)
			printf("Failed, Errer is %c\r\n",temp);//�˳�֮����Ҫ����ģ�������������
		temp = Reboot();
		if(temp)
			printf("Failed, Errer is %c\r\n",temp);
		
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
