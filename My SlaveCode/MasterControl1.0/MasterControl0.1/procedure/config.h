#ifndef		__CONFIG_H
#define		__CONFIG_H
 
 #define MAIN_Fosc		11059200L	// 

#define Pump1        P04 //��1  ���Ƽ̵���REL3
#define Pump3        P03//��3 CAPSENS ԭ��ͼ��Ӧ����  ���Ƽ̵���REL6
#define Pump5        P02//��5 EXVALVE ԭ��ͼ��Ӧ����  ���Ƽ̵���REL7
#define EXValve1     P01//������1 VALVE5 ���Ƽ̵���REL8
#define EXValve3     P00//������3 VALVE4  ���Ƽ̵���REL5
#define EXValve5     P46//������5 VALVE3   ���Ƽ̵���REL2
#define SPARE        P45//����  VALVE2  ���Ƽ̵���REL4

#define CapSensor    P27// ���������ӹ��� OUT5V   |VALVE1  ���Ƽ̵���REL1

#define Exist 1
#define NoExist 0
#define ON 0                         //����
#define OFF 1                      //ֹͣ
#define VALVEON  1              //����
#define VALVEOFF 0                //�ر�

#define	COM_TX1_Lenth	40
#define	COM_RX1_Lenth	40

#define RLED1  P06//P1�ӿ�
#define RLED2  P16//P2�ӿ�
#define RLED3  P11//P3�ӿ�
#define RLED4  P52//P4�ӿ�
#define RLED5  P13//P5�ӿ�

#define GLED1  P07//P1�ӿ�
#define GLED2  P17//P2�ӿ�
#define GLED3  P47//P3�ӿ�
#define GLED4  P53//P4�ӿ�
#define GLED5  P14//P5�ӿ�

/*INPUT PORTx  10 */
#define Cup1    P10//1�ޱ��� P1�ӿ�
#define Cup2    P54//2�ޱ��� P2�ӿ�
#define Cup3    P12//3�ޱ��� P3�ӿ�
#define Cup4    P05//4�ޱ��� P4�ӿ�
#define Cup5    P15//5�ޱ��� P5�ӿ�
#define Water1Sensor P21// 0:��ˮ  1����ˮ  P7�ӿ�
#define Water2Sensor P20//
#define Water3Sensor P44//
#define Water4Sensor P43//
#define Water5Sensor P42//
#define LED P41//


/* TX_BUFF���������־ */
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



#define MaxSendStr 50

#include	"STC15Fxxxx.H" 


////����ʹ�ܱ�־����

extern u8 TaskUartFlag;//����1������֡���ս�������
extern u8 TaskStateScanFlag;//����2���豸״̬ɨ��������
extern u8 TaskUploadFlag;//����3�������ϴ�����
extern u8 TaskEEPROMFlag;//����4��EEPROM��д����
extern u8 TaskControlFlag;//����5���豸���п�������


//������������ÿ��λ�õ����ݲο���Э���ĵ���10����11
#define SDB_LEN 26	//����״̬���ݳ���
#define MDB_LEN 31	//����STM32�·�������������
extern u8 SlaveDataBuff[SDB_LEN];//����״̬������
extern u8 MasterDataBuff[MDB_LEN];//����STM32�·���������



#endif