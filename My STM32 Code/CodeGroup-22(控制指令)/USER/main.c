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
	 delay_init();	    	 														//延时函数初始化
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	// 设置中断优先级分组2	
	 //TIM3_Int_Init(29999,7199);											//TIM3初始化（3s一次）
	 TIM4_Int_Init(99,7199);    										  //TIM4初始化（500ms一次）
	 Interrupts_Config();        											//配置屏幕串口中断
	 ScreenInit(9600);           											//屏幕初始化
	 queue_reset();              											//清空屏幕串口接收缓冲区   
	 AT24C256Init();                                  //初始化存储器
	 LEDInit();                										    //LED初始化
	 KEYInit();       																//KEY初始化
	 LockInit();                                      //Lock初始化
	 AdcInit();	       														  	//ADC初始化
	 LanInit(115200);
	 CodeInit(115200);		 														//扫码器初始化
	 SlaveInit(115200);    														//主从控板串口初始化
	 GPRSInit(115200);	 															//GPRS串口初始化
	 GPRSConfig("TCP", "test.usr.cn", "2317");  			//配置GPRS主要参数
	 
	 delay_ms(500);     															//延时等待串口屏初始化完毕,必须等待300ms 
	 
	 
	 while(1)
		 {
			 if(TIM4Flag10Ms==1)      //10ms运行一次
			 {
				 TIM4Flag10Ms=0;
				 if((TIM4Flag100Ms++)>=20)   //200ms运行一次
				 {
					 TIM4Flag100Ms = 0;
					 
					 AirPressureTask();
					 
					 BarCodeTask();
					 
					 ManufacturerServerTask();
			 
					 UserServerTask();
					 
					 BoradSlaveTask();
					 
					 LockerTask();
					 
					 LED13=!LED13;
					 
//					 ReadLenByte((uint16_t)DataAddress0, (uint8_t *)SaveTest,(uint8_t)sizeof(TranspondDataBuf));  //读取存储器测试
				 }
				 if((TIM4Flag20Ms++)>=2)  //20ms运行一次
				 {
					 TIM4Flag20Ms = 0;
					 
					 HMITask();
					 
				 }
			 }
			 
			 LedKeyTask();
			 
			 Timer();    //应答定时器
			 
		 }	 
 }
 
 
//应答定时器
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
 
 
 
 
 
 


