
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






#define LED P41
extern u8 EEPROMDATA;






#endif
