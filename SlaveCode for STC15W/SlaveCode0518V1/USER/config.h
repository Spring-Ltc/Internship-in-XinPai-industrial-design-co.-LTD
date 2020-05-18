
#ifndef		__CONFIG_H
#define		__CONFIG_H


/*********************************************************/

//#define MAIN_Fosc		22118400L	//������ʱ��
//#define MAIN_Fosc		12000000L	//������ʱ��
#define MAIN_Fosc		11059200L	//������ʱ��
//#define MAIN_Fosc		 5529600L	//������ʱ��
//#define MAIN_Fosc		24000000L	//������ʱ��


/*********************************************************/

#include	"STC15Fxxxx.H"


/**************************************************************************/

#define Main_Fosc_KHZ	(MAIN_Fosc / 1000)

/***********************************************************/






//����ʹ�ܱ�־����
extern u8 TaskUartFlag;//����1������֡���ս�������
extern u8 TaskStateScanFlag;//����2���豸״̬ɨ��������
extern u8 TaskUploadFlag;//����3�������ϴ�����
extern u8 TaskEEPROMFlag;//����4��EEPROM��д����
extern u8 TaskControlFlag;//����5���豸���п�������


//������������ÿ��λ�õ����ݲο���Э���ĵ���10����11
#define SDB_LEN 23	//����״̬���ݳ���
#define MDB_LEN 23	//����STM32�·�������������
extern u8 SlaveDataBuff[SDB_LEN];//����״̬������
extern u8 MasterDataBuff[MDB_LEN];//����STM32�·���������



#define MaxSendStr 35


//������λ�ú궨��
#define MA_time 20
#define SA_time 20

#define MA_limit 21
#define SA_limit 21






#define LED P41
extern u8 EEPROMDATA;






#endif
