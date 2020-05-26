#ifndef		__CONFIG_H
#define		__CONFIG_H
 // #define MAIN_Fosc		22118400L	// 
//#define MAIN_Fosc		12000000L	// 
 #define MAIN_Fosc		11059200L	// 
//#define MAIN_Fosc		 5529600L	// 
//#define MAIN_Fosc		24000000L	// 
#include	"STC15Fxxxx.H"



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

#define MaxSendStr 30



#define MAlimit 21//MasterDataBuff里面Limit的起始Address下标为21
#define MAuploadtime 20	//上传时间间隔在MasterDataBuff的Address
#define SAuploadtime 20



///* TX_BUFF发送数组标志 */
//#define		Reled1	0
//#define		Reled3	1
//#define		Reled5	2  //红灯
//#define		Grled1	3
//#define		Grled3	4
//#define		Grled5  5   //绿灯
//#define		CupOne	6
//#define		CupTwo	 7
//#define		CupThree 8   //杯子
//#define		Water1	 9
//#define		Water3	10
//#define		Water5	11   //水流
//#define		pum1		12
//#define		pum3		13
//#define		pum5	  14   //水泵
//#define		Valve1	15
//#define		Valve3	16
//#define		Valve5	17   //阀门
//#define		Sensor	18   //5v控制
//#define		Lock 	  19   //锁
//#define		Interval	20 //时间
//#define		TimH	    21      //剩余杯子次数
//#define		TimL	    22
//#define		Conf1	    23
//#define		Conf3	    24
//#define		Conf5	    25   //杯子有效性




#endif









