
/*****************************************************************************************/
//项目名称：废液抽取设备
//版    本：XPCYV1.0
//存储路径：E:\工作\MasterControl\5.7\MasterControl5.7\MAINCONTROLCSDV0826		
//--------------------------------
//作者：Xiong_Xiaoping	
//修改日期：2020/05/07
//修改内容：完成处次版本
//----------------------------
//作者：Xiong_Xiaoping	
//修改日期：2020/05/12
//修改内容：修改第五一检测杯，启动后无法停止等bug;
//----------------------------
//作者：Xiong_Xiaoping	
//修改日期：2020/05/12
//修改内容：分版块对数据做处理;
//----------------------------
//OSC 11.0592MHz
/*****************************************************************************************/

//头文件
/*****************************************************************************************/
#include	"config.h"
#include	"timer.h"
#include	"GPIO.h"
#include	"USART1.h"
#include  "eeprom.h"
#include  "intrins.h"
#include  "delay.h"
#include  "CupDet.h"
#include  "TaskUart.h"
#include  "MasterControl.h"
#include	"ResidueDegree.h"

extern COMx_Define	COM1;
extern	u8 	RX1_Buffer [COM_RX1_Lenth];	//接收缓冲
extern T0_100MS2,T1_1MS1,T2_10MS2,MS3,T0_500MS2 ;  //其他文件定义的变量(外部变量)
extern	u16 ScanCounter1, ScanCounter2, ScanCounter3, ScanCounter4, ScanCounter5,ExvalveCounter;//传感器标志|计数器
 
//任务使能标志初始化
  u8 TaskUartFlag = 0;//任务1：数据帧接收解析任务
  u8 TaskStateScanFlag = 0;//任务2：设备状态扫描检测任务
  u8 TaskUploadFlag = 0;//任务3：数据上传任务
  u8 TaskEEPROMFlag = 0;//任务4：EEPROM读写任务
  u8 TaskControlFlag = 0;//任务5：设备运行控制任务
	
//两个数据区的定义和初始化，每个位置的数据参考总协议文档表10、表11
//u8 SlaveDataBuff[SDB_LEN];//本地状态数据区
//u8 MasterDataBuff[MDB_LEN];//主控STM32下发的数据区

u8 SlaveDataBuff[SDB_LEN]={0} ;//本地状态数据区
u8 MasterDataBuff[MDB_LEN]={0};//主控STM32下发的数据区


 u8 vt1;



void main()
{
	  //u8 i;
   	GPIO_config ();
    Timer_config();
	  UART_config ();
	  EA = 1;
	  CupDetconfig();//自检初始化
	 while(1)
	{
		
		 PumpControl();
		if(TaskStateScanFlag)	//500ms 检测一次 第一个时间片  //基础任务允许工作
    { 
			TaskStateScanFlag=0; //清除标志位
		//	LED=~LED;//Twinkel
			ScanCounter1++;
			ScanCounter5++;
			ScanCounter3++;
			vt1++;
			CupDet(); //从板杯孔状态控制及处理函数//将检测的数据更新保存到本地状态数据区

			
		}
		
		if(TaskUartFlag)//数据解析任务允许工作，该标志位由定时器中断服务函数置位
		{

			TaskUartFlag = 0;//清除标志位
			LED=~LED;//Twinkel
      if(DataValidityCheck(RX1_Buffer) == 0)//校验成功
			DataAnalyze(RX1_Buffer);//解析数据
			 state=0;
		}
		
	
	if(T0_500MS2)
	{
		T0_500MS2=0;
		vt1++;
	  if(TaskEEPROMFlag != 0)//EEPROM任务允许工作
			{
				MasterControlEE();//剩余次数写入函数//直接写EEPROM,同时更新本地状态数据区的内容
				TaskEEPROMFlag = 0;//清除标志位
			
			}

		if(TaskControlFlag != 0)//控制任务允许工作
		{
			MasterControl();//控制函数//读取指令数据区的内容，根据数据完成对各设备的控制
			TaskControlFlag = 0;//清除标志位
		}				
	}	
    if(TaskUploadFlag!=0)//上传状态任务允许工作
		{
			FramePacking(0x71,SlaveDataBuff);
			TaskUploadFlag = 0;//清除标志位
		}
	
	}
}
