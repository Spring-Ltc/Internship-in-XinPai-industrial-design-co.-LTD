#ifndef		__CONFIG_H
#define		__CONFIG_H
 // #define MAIN_Fosc		22118400L	// 
//#define MAIN_Fosc		12000000L	// 
 #define MAIN_Fosc		11059200L	// 
//#define MAIN_Fosc		 5529600L	// 
//#define MAIN_Fosc		24000000L	// 
#include	"STC15Fxxxx.H"



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

#define MaxSendStr 30



#define MAlimit 21//MasterDataBuff����Limit����ʼAddress�±�Ϊ21
#define MAuploadtime 20	//�ϴ�ʱ������MasterDataBuff��Address
#define SAuploadtime 20



///* TX_BUFF���������־ */
//#define		Reled1	0
//#define		Reled3	1
//#define		Reled5	2  //���
//#define		Grled1	3
//#define		Grled3	4
//#define		Grled5  5   //�̵�
//#define		CupOne	6
//#define		CupTwo	 7
//#define		CupThree 8   //����
//#define		Water1	 9
//#define		Water3	10
//#define		Water5	11   //ˮ��
//#define		pum1		12
//#define		pum3		13
//#define		pum5	  14   //ˮ��
//#define		Valve1	15
//#define		Valve3	16
//#define		Valve5	17   //����
//#define		Sensor	18   //5v����
//#define		Lock 	  19   //��
//#define		Interval	20 //ʱ��
//#define		TimH	    21      //ʣ�౭�Ӵ���
//#define		TimL	    22
//#define		Conf1	    23
//#define		Conf3	    24
//#define		Conf5	    25   //������Ч��




#endif









