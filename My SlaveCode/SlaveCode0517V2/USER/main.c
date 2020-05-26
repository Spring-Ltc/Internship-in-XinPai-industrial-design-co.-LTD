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
	TaskUart1_Init(115200);
	FramePacking(0x70,SlaveDataBuff);
	
	LED=1;delay_ms(500);
	LED=0;delay_ms(500);
	LED=1;delay_ms(500);
	
	EA = 1;
	
	MasterDataBuff[MA_time] = 0x00;
	
	while(1)
	{

		if(TaskUartFlag != 0)//���ݽ����������������ñ�־λ�ɶ�ʱ���жϷ�������λ
		{
			if(DataValidityCheck(ReceiveBuff) == 0)//У��ɹ�
			{
				LED = ~LED;
				DataAnalyze(ReceiveBuff);//��������
			}
			ReceiveEnable=0;//��������ձ�־λ
			TaskUartFlag = 0;//�����־λ
		}
		
		if(TaskStateScanFlag != 0)//��������������
		{	
			TaskStateScanFlag = 0;//�����־λ
			delay_ms(50);//������Ҫ50ms
			//����������ɶԱ���״̬�ļ�⣬���ݼ��Ľ��������д��������������־λ
			//���������ݸ��±��浽����״̬������
		}
		
		if(TaskUploadFlag != 0)//�ϴ�״̬����������
		{
			TaskUploadFlag = 0;//�����־λ
			FramePacking(0x71,SlaveDataBuff);//ͨ�����ڣ��ѱ���״̬�����������ݴ���ϴ�
		}
		
		if(TaskEEPROMFlag != 0)//EEPROM����������
		{
				//ֱ�Ӷ�ȡMasterDataBuff������дEEPROM,д���ڶ�ȡEEPROM����ȡ�Ľ��д��SlaveDataBuff
				SlaveDataBuff[21] = MasterDataBuff[21];//�����ã������Ѿ�д��ȥ��
				SlaveDataBuff[22] = MasterDataBuff[22];
				TaskEEPROMFlag = 0;//�����־λ
		}
		
		if(TaskControlFlag != 0)//��������������
		{
			TaskControlFlag = 0;//�����־λ
			//��ȡָ�������������ݣ�����������ɶԸ��豸�Ŀ���
			SendDataLen(MasterDataBuff,26);//�����ã�����û�а�״̬����д��������
		}
	}
}
