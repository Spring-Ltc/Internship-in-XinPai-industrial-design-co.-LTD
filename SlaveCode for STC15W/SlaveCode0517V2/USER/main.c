//--------------------------------------------------------------------------------------
//通信使用串口1，USART1
//Timer0用来做任务的时间分片
//		基础任务100ms允许一次，上传任务最小500ms允许一次
//		设置Timer0定时时间为10ms
//Timer1用来做串口的超时接收,设置Timer1定时时间为10ms(4ms左右就可以接收约40Byte)
//Timer2做波特率发生器
//------------------------------------------------------------------------------------------

#include "config.h"
#include "TaskUart.h"
#include "TimeProcess.h"
#include "TaskState.h"
#include "delay.h"



//任务使能标志初始化
  u8 TaskUartFlag = 0;//任务1：数据帧接收解析任务
  u8 TaskStateScanFlag = 0;//任务2：设备状态扫描检测任务
  u8 TaskUploadFlag = 0;//任务3：数据上传任务
  u8 TaskEEPROMFlag = 0;//任务4：EEPROM读写任务
  u8 TaskControlFlag = 0;//任务5：设备运行控制任务
	
//两个数据区的定义和初始化，每个位置的数据参考总协议文档表10、表11
u8 SlaveDataBuff[SDB_LEN] = {0};//本地状态数据区
u8 MasterDataBuff[MDB_LEN] = {0};//主控STM32下发的数据区




void main()
{
	//系统初始化，包括GPIO、USART、Timer、上电自检等
	GPIO_config();
	Timer0Config();
	Timer1Config();
	TaskUart1_Init(115200);
	FramePacking(0x70,SlaveDataBuff);
	
	LED=1;delay_ms(500);
	LED=0;delay_ms(500);
	LED=1;delay_ms(500);
	
	EA = 1;
	
	MasterDataBuff[MA_time] = 0x00;
	
	while(1)
	{

		if(TaskUartFlag != 0)//数据解析任务允许工作，该标志位由定时器中断服务函数置位
		{
			LED = ~LED;
			TaskUartFlag = 0;//清除标志位
			//if(DataValidityCheck(ReceiveBuff) == 0)//校验成功
				DataAnalyze(ReceiveBuff);//解析数据
				ReceiveEnable=0;//清允许接收标志位
				//SendDataLen(ReceiveBuff,15);//发回来看看？？
		}
		
		if(TaskStateScanFlag != 0)//基础任务允许工作
		{	
			TaskStateScanFlag = 0;//清除标志位
			delay_ms(50);//假设需要50ms
			//基础任务，完成对杯座状态的检测，根据检测的结果，可以写其他任务的允许标志位
			//将检测的数据更新保存到本地状态数据区
		}
		
		if(TaskUploadFlag != 0)//上传状态任务允许工作
		{
			TaskUploadFlag = 0;//清除标志位
			FramePacking(0x71,MasterDataBuff);//通过串口，把本地状态数据区的内容打包上传
		}
		
		if(TaskEEPROMFlag != 0)//EEPROM任务允许工作
		{
				//直接读取MasterDataBuff的内容写EEPROM,写完在读取EEPROM，读取的结果写到SlaveDataBuff
				TaskEEPROMFlag = 0;//清除标志位
		}
		
		if(TaskControlFlag != 0)//控制任务允许工作
		{
			TaskControlFlag = 0;//清除标志位
			//读取指令数据区的内容，根据数据完成对各设备的控制
		}
	}
}
