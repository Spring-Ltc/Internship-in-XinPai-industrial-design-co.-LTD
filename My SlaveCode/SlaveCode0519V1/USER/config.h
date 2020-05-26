
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







extern u8 EEPROMDATA;






//IO�ں궨��
#define LED P41		//ָʾ��LED

#define Pump1        P04//��1 ���Ƽ̵���REL1
#define Pump3        P03//��3 ���Ƽ̵���REL2
#define Pump5        P02//��5 ���Ƽ̵���REL3
#define EXValve1     P01//������1 ���Ƽ̵���REL4
#define EXValve3     P00//������3 ���Ƽ̵���REL5
#define EXValve5     P46//������5 ���Ƽ̵���REL6
#define CapSensor    P27// ���������ӹ��� OUT5V ���Ƽ̵���REL8

#define RLED1  P06//P4�ӿ�
#define RLED3  P11//P5�ӿ�
#define RLED5  P13//P6�ӿ�
#define GLED1  P07//P4�ӿ�
#define GLED3  P47//P5�ӿ�
#define GLED5  P14//P6�ӿ�
#define Cup1    P10//1�ޱ��� P4�ӿ�
#define Cup3    P12//3�ޱ��� P5�ӿ�
#define Cup5    P15//5�ޱ��� P6�ӿ�

#define Water1Sensor P21// 0:��ˮ  1����ˮ  P1�ӿ�
#define Water3Sensor P44// 0:��ˮ  1����ˮ  P2�ӿ�
#define Water5Sensor P42// 0:��ˮ  1����ˮ  P3�ӿ�


#define ON 0                         //����
#define OFF 1                      //ֹͣ




//��������ַ����
#define		Reled1	0
#define		Reled3	1
#define		Reled5	2  //���
#define		Grled1	3
#define		Grled3	4
#define		Grled5  5   //�̵�
#define		CupOne	6
#define		CupTwo	 7
#define		CupThree 8   //����  00 00 00 00 00 00 00 00 00 01 01 01 00 01 00 00 00 00 00 00 00 01 00 00 FF FF
#define		Water1	 9
#define		Water3	10
#define		Water5	11   //ˮ��
#define		pum1		12
#define		pum3		13
#define		pum5	  14   //ˮ��
#define		Valve1	15
#define		Valve3	16
#define		Valve5	17   //����
#define		Sensor	18   //5v����
#define		Lock 	  19   //��
#define		Interval	20 //ʱ��
#define		TimH	    21      //ʣ�౭�Ӵ���
#define		TimL	    22
#define		Conf1	    23
#define		Conf3	    24
#define		Conf5	    25  //������Ч��







#endif
