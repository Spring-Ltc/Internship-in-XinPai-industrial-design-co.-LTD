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

	u8 ackbuf[30]={0};
	u8 writebuf[30]={0};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	
	delay_init();	    	 //��ʱ������ʼ��	  
	
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	AT24C256Init();			//��ʼ��IIC�ӿ����
	USRK6Init(115200);		//��ʼ����̫��ģ�����Ӳ��
	LOCK_Init();
	SlaveInit(9600);
	
//	WriteLenByte(AddUSRK6+16*0,"115200",16);
//	WriteLenByte(AddUSRK6+16*1,"8",16);
//	WriteLenByte(AddUSRK6+16*2,"1",16);
//	WriteLenByte(AddUSRK6+16*3,"NONEE",16);
//	WriteLenByte(AddUSRK6+16*4,"NFC",16);	
	
	
	
	//ExitAT();Reboot();
	
	USRK6Config();
	

	while(1)
	{
		delay_ms(200);
		LED0=!LED0;//DS0��˸
	}
}



//������̫��ģ�飬������
void USRK6Config(void)
{
	u8 temp=11;
	u8 time=0;
	char str1[16],str2[16],str3[16],str4[16],str5[16];
		
	
	time=0;
	while(EnterAT())//����ATģʽ������ʧ�ܳ��Զ�ν���
	{
		delay_ms(500);
		USART_SendString(UART4,"Enter AT Failed\r\n");//����ʧ��
		time--;
		if(time>20)return;//��ʱ�˳�����������������
	}
	USART_SendString(UART4,"Enter AT OK\r\n");	

		ReadLenByte(AddUSRK6+16*0,str1,16);
		ReadLenByte(AddUSRK6+16*1,str2,16);
		ReadLenByte(AddUSRK6+16*2,str3,16);
		ReadLenByte(AddUSRK6+16*3,str4,16);
		ReadLenByte(AddUSRK6+16*4,str5,16);
		temp = ConfigUART(str1,str2,str3,str4,str5);//����UART
			if(temp == 0)
				USART_SendString(UART4,"Config UART OK\r\n");
			else
			{
				USART_SendString(UART4,"Config UART Failed,\r\n");
			}
	
	
	
	
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

			
			
			ATQuery("UART",packedbuff);
			USART_SendString(UART4,packedbuff);
			
		temp = Reboot();//�˳�֮����Ҫ����ģ�������������
		if(temp)
			USART_SendString(UART4,"Reboot Failed\r\n");
		
	}
			
}





