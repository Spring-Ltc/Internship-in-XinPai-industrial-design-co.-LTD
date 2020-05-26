


#include "GPIO.h"
#include	"timer.h"
#include "USART1.h"
//#include <STC15F2K60S2.H>
#include "config.h"
#include "eeprom.h"
#include "TaskUart.h"


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
	//SystermInit();//系统初始化，包括GPIO、USART、Timer、上电自检等

	while(1)
	{
		if(TaskUartFlag != 0)//数据解析任务允许工作，该标志位由定时器中断服务函数置位
		{
			TaskUartFlag = 0;//清除标志位
			if(DataValidityCheck(RX1_Buffer) == 0)//校验成功
				DataAnalyze(RX1_Buffer);//解析数据
		}
		
		if(TaskStateScanFlag != 0)//基础任务允许工作
		{
			TaskStateScanFlag = 0;//清除标志位
			//基础任务，完成对杯座状态的检测，根据检测的结果，可以写其他任务的允许标志位
			//将检测的数据更新保存到本地状态数据区
		}
		
		if(TaskUploadFlag != 0)//上传状态任务允许工作
		{
			TaskUploadFlag = 0;//清除标志位
			//通过串口，把本地状态数据区的内容打包上传
		}
		
		if(TaskEEPROMFlag != 0)//EEPROM任务允许工作
		{
			if(TaskEEPROMFlag == 1)//标志位为1，表示由任务5（TaskControl）发起,设置限制次数
			{
				//直接读取MasterDataBuff的内容写EEPROM,写完在读取EEPROM，读取的结果写到SlaveDataBuff
			}
				TaskEEPROMFlag = 0;//清除标志位
		}
		
		if(TaskControlFlag != 0)//控制任务允许工作
		{
			TaskControlFlag = 0;//清除标志位
			//读取指令数据区的内容，根据数据完成对各设备的控制
		}
	}
}



