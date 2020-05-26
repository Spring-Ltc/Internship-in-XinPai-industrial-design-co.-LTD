#include "led.h"
#include "delay.h"
#include "sys.h"
#include "hmi_driver.h"
#include "hmi_user_uart.h"
#include "cmd_queue.h"
#include "cmd_process.h"
#include "stdio.h"
#include "hw_config.h"
#include "ulitity.h"
#include "string.h"

#include "timer.h"
#include "gprs.h"
#include "slave.h"
#include "code.h"
#include "user.h"
#include "adc.h"
#include "lan.h"
#include "lock.h"
#include "hmi_handle.h"
#include "AT24C256.h"




u8 TIM4Flag10Ms = 0;
u8 TIM4Flag10Ms2 = 0;
u8 TIM4Flag10Ms3 = 0;
u8 TIM4Flag10Ms4 = 0;
u8 TIM4Flag10Ms5 = 0;
u8 TIM4Flag100Ms = 0;

u8 TIM4Flag20Ms = 0;

int time=0;
int TimeStart = 0;
int TimeOut = 0;

//u8 SaveTest[256];

u8 ReceiveBuf[20];

void Timer(void);

int main(void)
 {	
	 delay_init();	    	 														//��ʱ������ʼ��
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	// �����ж����ȼ�����2	
	 //TIM3_Int_Init(29999,7199);											//TIM3��ʼ����3sһ�Σ�
	 TIM4_Int_Init(99,7199);    										  //TIM4��ʼ����500msһ�Σ�
	 Interrupts_Config();        											//������Ļ�����ж�
	 ScreenInit(9600);           											//��Ļ��ʼ��
	 queue_reset();              											//�����Ļ���ڽ��ջ�����   
	 AT24C256Init();                                  //��ʼ���洢��
	 LEDInit();                										    //LED��ʼ��
	 KEYInit();       																//KEY��ʼ��
	 LockInit();                                      //Lock��ʼ��
	 AdcInit();	       														  	//ADC��ʼ��
	 LanInit(115200);
	 CodeInit(115200);		 														//ɨ������ʼ��
	 SlaveInit(115200);    														//���ӿذ崮�ڳ�ʼ��
	 GPRSInit(115200);	 															//GPRS���ڳ�ʼ��
	 GPRSConfig("TCP", "test.usr.cn", "2317");  			//����GPRS��Ҫ����
	 
	 delay_ms(500);     															//��ʱ�ȴ���������ʼ�����,����ȴ�300ms 
	 
	 
	 while(1)
		 {
			 if(TIM4Flag10Ms==1)      //10ms����һ��
			 {
				 TIM4Flag10Ms=0;
				 if((TIM4Flag100Ms++)>=20)   //200ms����һ��
				 {
					 TIM4Flag100Ms = 0;
					 
					 AirPressureTask();
					 
					 BarCodeTask();
					 
					 ManufacturerServerTask();
			 
					 UserServerTask();
					 
					 BoradSlaveTask();
					 
					 LockerTask();
					 
					 LED13=!LED13;
					 
//					 ReadLenByte((uint16_t)DataAddress0, (uint8_t *)SaveTest,(uint8_t)sizeof(TranspondDataBuf));  //��ȡ�洢������
				 }
				 if((TIM4Flag20Ms++)>=2)  //20ms����һ��
				 {
					 TIM4Flag20Ms = 0;
					 
					 HMITask();
					 
				 }
			 }
			 
			 LedKeyTask();
			 
			 Timer();    //Ӧ��ʱ��
			 
		 }	 
 }
 
 
//Ӧ��ʱ��
void Timer(void)
{
	if(TimeStart == 1)
	{
		if(TIM4Flag10Ms5==1)
		{
			TIM4Flag10Ms5=0;
			if((time++)>=300)
			{
				time = 0;
				TimeOut = 1;
//				LED11=!LED11;
			}
		}
	}
	
}
 
 
 
 
 
 


