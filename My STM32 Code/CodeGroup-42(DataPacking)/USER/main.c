#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "AT24C256.h"
#include "USRK6.h"
#include "user.h"
#include "Slave.h"

void USRK6Config(void);

u8 packedbuff[30]={0};


int main(void)
{
	
	uint8_t i=0;
	u16 temp=0;
#include "stm32f10x.h"                  // Device header
	u8 ackbuf[30]={0};
	u8 writebuf[30]={0};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	
	delay_init();	    	 //��ʱ������ʼ��	  
	
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	//AT24C256Init();			//��ʼ��IIC�ӿ����
	//USRK6Init(115200);		//��ʼ����̫��ģ�����Ӳ��
	LOCK_Init();
	SlaveInit(9600);
	

	USART_SendString(UART4,"pcck Text");

	
	Flag_UART4_Solve=0;
	ClearArray_u8(ReceiveBuff,200);
	while(1)
	{
		delay_ms(200);
		LED0=!LED0;//DS0��˸
		if(Flag_UART4_Solve)//�յ����ݣ�������
		{

			ClearArray_u8(packedbuff,30);
			DataPacking(ReceiveBuff[0],0x20,0x21,0x01,0x58,&ReceiveBuff[1],packedbuff);
			USART_SendLenData(UART4,packedbuff,ReceiveBuff[0]+10);
			//RX_DMA_ENABLE();//�ٿ���DMA
		  Flag_UART4_Solve=0;//�����־
			ClearArray_u8(ReceiveBuff,200);
		}
	}
}



//������̫��ģ�飬������
void USRK6Config(void)
{
	u8 temp=11;

	ManufacturerReset();	//�ָ���������
	delay_ms(500);
		
		temp = EnterAT();//�л���ATģʽ
		if(temp)
		{
			USART_SendString(UART4,"Enter AT Failed\r\n");//����ʧ��
			return;//����ATģʽʧ�ܣ���ӡ��Ϣ��ֱ���˳�
		}
		else
		{
		//����ATģʽ�ɹ������濪ʼ������ز���
			USART_SendString(UART4,"Enter AT OK\r\n");
			
			temp = ConfigSOCK("TCPS","192.168.0.117","40");//����SOCK
			if(temp == 0)
				USART_SendString(UART4,"Config SOCK OK\r\n");
			else
			{
				USART_SendString(UART4,"Config SOCK Failed,\r\n");
//				printf("Error = %c\r\n",temp);
			}
			
			
			temp = ConfigWANN("STATIC","192.168.0.117","255.255.255.0","192.168.0.1");//����WANN
			if(temp == 0)
				USART_SendString(UART4,"Config WANN OK\r\n");
			else
			{
				USART_SendString(UART4,"Config WANN Failed,\r\n");
//				printf("Error = %c\r\n",temp);
			}
			
			
			temp = ConfigUART("115200","8","1","NONE","NFC");//����UART
			if(temp == 0)
				USART_SendString(UART4,"Config UART OK\r\n");
			else
			{
				USART_SendString(UART4,"Config UART Failed,\r\n");
//				printf("Error = %c\r\n",temp);
			}

		temp = Reboot();//�˳�֮����Ҫ����ģ�������������
		if(temp)
			USART_SendString(UART4,"Reboot Failed\r\n");
		
	}
			
}





