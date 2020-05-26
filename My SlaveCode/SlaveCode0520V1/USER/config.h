
#ifndef		__CONFIG_H
#define		__CONFIG_H


/*********************************************************/

//#define MAIN_Fosc		22118400L	//定义主时钟
//#define MAIN_Fosc		12000000L	//定义主时钟
#define MAIN_Fosc		11059200L	//定义主时钟
//#define MAIN_Fosc		 5529600L	//定义主时钟
//#define MAIN_Fosc		24000000L	//定义主时钟


/*********************************************************/

#include	"STC15Fxxxx.H"


/**************************************************************************/

#define Main_Fosc_KHZ	(MAIN_Fosc / 1000)

/***********************************************************/






//任务使能标志声明
extern u8 TaskUartFlag;//任务1：数据帧接收解析任务
extern u8 TaskStateScanFlag;//任务2：设备状态扫描检测任务
extern u8 TaskUploadFlag;//任务3：数据上传任务
extern u8 TaskEEPROMFlag;//任务4：EEPROM读写任务
extern u8 TaskControlFlag;//任务5：设备运行控制任务


//数据区声明，每个位置的数据参考总协议文档表10、表11
#define SDB_LEN 23	//本地状态数据长度
#define MDB_LEN 23	//主控STM32下发的数据区长度
extern u8 SlaveDataBuff[SDB_LEN];//本地状态数据区
extern u8 MasterDataBuff[MDB_LEN];//主控STM32下发的数据区



#define MaxSendStr 35


//数据区位置宏定义
#define MA_time 20
#define SA_time 20

#define MA_limit 21
#define SA_limit 21







extern u8 EEPROMDATA;






//IO口宏定义
#define LED P41		//指示灯LED

#define Pump1        P04//泵1 控制继电器REL1
#define Pump3        P03//泵3 控制继电器REL2
#define Pump5        P02//泵5 控制继电器REL3
#define EXValve1     P01//空气阀1 控制继电器REL4
#define EXValve3     P00//空气阀3 控制继电器REL5
#define EXValve5     P46//空气阀5 控制继电器REL6
#define CapSensor    P27// 传感器板子供电 OUT5V 控制继电器REL8

#define RLED1  P06//P4接口
#define RLED3  P11//P5接口
#define RLED5  P13//P6接口
#define GLED1  P07//P4接口
#define GLED3  P47//P5接口
#define GLED5  P14//P6接口
#define Cup1    P10//1无杯子 P4接口
#define Cup3    P12//3无杯子 P5接口
#define Cup5    P15//5无杯子 P6接口

#define Water1Sensor P21// 0:无水  1：有水  P1接口
#define Water3Sensor P44// 0:无水  1：有水  P2接口
#define Water5Sensor P42// 0:无水  1：有水  P3接口


#define ON 0                         //启动
#define OFF 1                      //停止




//数据区地址定义
#define		Reled1	0
#define		Reled3	1
#define		Reled5	2  //红灯
#define		Grled1	3
#define		Grled3	4
#define		Grled5  5   //绿灯
#define		CupOne	6
#define		CupTwo	 7
#define		CupThree 8   //杯子  00 00 00 00 00 00 00 00 00 01 01 01 00 01 00 00 00 00 00 00 00 01 00 00 FF FF
#define		Water1	 9
#define		Water3	10
#define		Water5	11   //水流
#define		pum1		12
#define		pum3		13
#define		pum5	  14   //水泵
#define		Valve1	15
#define		Valve3	16
#define		Valve5	17   //阀门
#define		Sensor	18   //5v控制
#define		Lock 	  19   //锁
#define		Interval	20 //时间
#define		TimH	    21      //剩余杯子次数
#define		TimL	    22
#define		Conf1	    23
#define		Conf3	    24
#define		Conf5	    25  //杯子有效性







#endif
