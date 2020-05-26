//--------------------------------------------------------------------------------------
//ͨ��ʹ�ô���1��USART1
//Timer0�����������ʱ���Ƭ
//		��������100ms����һ�Σ��ϴ�������С500ms����һ��
//		����Timer0��ʱʱ��Ϊ10ms
//Timer1���������ڵĳ�ʱ����,����Timer1��ʱʱ��Ϊ10ms(4ms���ҾͿ��Խ���Լ40Byte)
//Timer2�������ʷ�����
//------------------------------------------------------------------------------------------

#include "config.h"
#include "TaskUart.h"
#include "TimeProcess.h"
#include "TaskState.h"
#include "TaskEEPROM.h"
#include "TaskControl.h"

#include "delay.h"



//����ʹ�ܱ�־��ʼ��
  u8 TaskUartFlag = 0;//����1������֡���ս�������
  u8 TaskStateScanFlag = 0;//����2���豸״̬ɨ��������
  u8 TaskUploadFlag = 0;//����3�������ϴ�����
  u8 TaskEEPROMFlag = 0;//����4��EEPROM��д����
  u8 TaskControlFlag = 0;//����5���豸���п�������
	
//�����������Ķ���ͳ�ʼ����ÿ��λ�õ����ݲο���Э���ĵ���10����11
u8 SlaveDataBuff[SDB_LEN] = {0};//����״̬������
u8 MasterDataBuff[MDB_LEN] = {0};//����STM32�·���������




void main()
{
	//ϵͳ��ʼ��������GPIO��USART��Timer���ϵ��Լ��
	GPIO_config();
	Timer0Config();
	Timer1Config();
	TaskUart1_Init();

	
	SelfCheckUp();//�ϵ��Լ�
	EA = 1;
	delay_ms(500);
	while(1)
	{
		PumpControl();//�������ޱ��ӽ��бõĿ���
		
		if(TaskUartFlag != 0)//���ݽ����������������ñ�־λ�ɶ�ʱ���жϷ�������λ
		{
			LED = ~LED;
			TaskUartFlag = 0;//�����־λ
			if(DataValidityCheck(ReceiveBuff) == 0)//У��ɹ�
				DataAnalyze(ReceiveBuff);//��������
			ReceiveEnable=0;//��������ձ�־λ
		}
		
		if(TaskStateScanFlag != 0)//��������������
		{	
			TaskStateScanFlag = 0;//�����־λ
			TaskBasic();
			WriteStateToSlaveDataBuff();//����������״̬
			//����������ɶԱ���״̬�ļ�⣬���ݼ��Ľ��������д��������������־λ
		}
		
		if(TaskUploadFlag != 0)//�ϴ�״̬����������
		{
			TaskUploadFlag = 0;//�����־λ
			FramePacking(0x71,MasterDataBuff);//ͨ�����ڣ��ѱ���״̬�����������ݴ���ϴ�
		}
		
		if(TaskEEPROMFlag != 0)//EEPROM����������
		{
				TaskEEPROM();//ֱ�Ӷ�ȡMasterDataBuff������дEEPROM,д���ڶ�ȡEEPROM����ȡ�Ľ��д��SlaveDataBuff
				TaskEEPROMFlag = 0;//�����־λ
		}
		
		if(TaskControlFlag != 0)//��������������
		{
			TaskControlFlag = 0;//�����־λ
			MasterControl();//��ȡָ�������������ݣ�����������ɶԸ��豸�Ŀ���
		}
	}
}
