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

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	
	delay_init();	    	 //��ʱ������ʼ��	  
//	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
	SlaveInit(115200);
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	AT24C256Init();			//��ʼ��IIC�ӿ����
	USRK6Init(115200);		//��ʼ����̫��ģ�����Ӳ��


		temp = ConfigLimit(4321,1);//��ҪӦ��
		printf("%x",temp);
 
		temp = ConfigLimit(1234,0);//����Ҫ��ҪӦ��
		printf("%x",temp);

	while(1)
	{
		delay_ms(300);
		LED0=!LED0;//DS0��˸
//		if(UART5_RX_STA >= 5)
//		{
//			USART_SendString(USART1,UART5_RX_BUF);
//			ClearArray(UART5_RX_BUF,MaxRx5Len);//����ս�����
//			UART5_RX_STA=0;//�������
//		}
	}
}
