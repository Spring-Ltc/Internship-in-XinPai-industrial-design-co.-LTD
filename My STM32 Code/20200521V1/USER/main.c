#include "led.h"
#include "delay.h"
#include "sys.h"

#include "timer.h"
#include "gprs.h"
#include "slave.h"
#include "code.h"
#include "user.h"
#include "adc.h"
#include "lan.h"
#include "lock.h"

#include "hmi_driver.h"
#include "hmi_user_uart.h"
#include "cmd_queue.h"
#include "cmd_process.h"
#include "stdio.h"
#include "hw_config.h"
#include "ulitity.h"
#include "string.h"

#include "hmi_handle.h"

u8 TIM4Flag10Ms = 0;
u8 TIM4Flag10Ms2 = 0;
u8 TIM4Flag10Ms3 = 0;
u8 TIM4Flag10Ms4 = 0;
u8 TIM4Flag100Ms = 0;

int main(void)
 {	
	 delay_init();	    	 														//��ʱ������ʼ��
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	// �����ж����ȼ�����2	
	 //TIM3_Int_Init(29999,7199);												//TIM3��ʼ����3sһ�Σ�
	 TIM4_Int_Init(99,7199);    										  //TIM4��ʼ����500msһ�Σ�
	 Interrupts_Config();        											//������Ļ�����ж�
	 ScreenInit(115200);           											//��Ļ��ʼ��
	 queue_reset();              											//�����Ļ���ڽ��ջ�����     
	 LEDKEY_Init();                										//LED��ʼ��
	 Adc_Init();	       															//ADC��ʼ��
	 CodeInit(115200);		 															//ɨ������ʼ��
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
				 }
				 
				 HMITask();
				 
			 }
			 LedKeyTask();
		 }	 
 }
 
 
 
 
 
 


