

#include "TaskControl.h"


void MasterControl(void)
{
	if(MasterDataBuff[Reled1]==0x01)RLED1=ON;  //��
	else if(MasterDataBuff[Reled1]==0x00)RLED1=OFF; //��

	if(MasterDataBuff[Reled3]==0x01)RLED3=ON;//��
	else if(MasterDataBuff[Reled3]==0x00)RLED3=OFF;//��

	if(MasterDataBuff[Reled5]==0x01)RLED5=ON;
	else if(MasterDataBuff[Reled5]==0x00)RLED5=OFF;
		
	if(MasterDataBuff[Grled1]==0x01)GLED1=ON;	//��
	else if(MasterDataBuff[Grled1]==0x00)GLED1=OFF;
	
	if(MasterDataBuff[Grled3]==0x01)GLED3=ON;	//��
	else if(MasterDataBuff[Grled3]==0x00)GLED3=OFF;

	if(MasterDataBuff[Grled5]==0x01)GLED5=ON;  //��
	else if(MasterDataBuff[Grled5]==0x00)GLED5=OFF;
		
	
	if(MasterDataBuff[pum1]==0x01)Pump1=1;//���Ʊ�����
	else if(MasterDataBuff[pum1]==0x00)Pump1=0;//���Ʊùر�

	if(MasterDataBuff[pum3]==0x01)Pump3=1;
	else if(MasterDataBuff[pum3]==0x00)Pump3=0;

	if(MasterDataBuff[pum5]==0x01)Pump5=1;
	else if(MasterDataBuff[pum5]==0x00)Pump5=0;

	
	if(MasterDataBuff[Valve1]==0x01)EXValve1=1;//���Ƽ̵�����������
	else if(MasterDataBuff[Valve1]==0x00)EXValve1=0;//���Ƽ̵����������ر�
			
	if(MasterDataBuff[Valve3]==0x01)EXValve3=1;
	else if(MasterDataBuff[Valve3]==0x00)EXValve3=0;
	
	if(MasterDataBuff[Valve5]==0x01)EXValve5=1;
	else if(MasterDataBuff[Valve5]==0x00)EXValve5=0;

	if(MasterDataBuff[Sensor]==0x01)CapSensor=1;//��5Vˮ���ܿ���
	else if(MasterDataBuff[Sensor]==0x00)CapSensor=0;//�ر�5Vˮ���ܿ���
}

							