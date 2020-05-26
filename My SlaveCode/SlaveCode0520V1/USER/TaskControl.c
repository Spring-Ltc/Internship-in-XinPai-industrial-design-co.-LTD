

#include "TaskControl.h"


void MasterControl(void)
{
	if(MasterDataBuff[Reled1]==0x01)RLED1=ON;  //亮
	else if(MasterDataBuff[Reled1]==0x00)RLED1=OFF; //灭

	if(MasterDataBuff[Reled3]==0x01)RLED3=ON;//亮
	else if(MasterDataBuff[Reled3]==0x00)RLED3=OFF;//灭

	if(MasterDataBuff[Reled5]==0x01)RLED5=ON;
	else if(MasterDataBuff[Reled5]==0x00)RLED5=OFF;
		
	if(MasterDataBuff[Grled1]==0x01)GLED1=ON;	//亮
	else if(MasterDataBuff[Grled1]==0x00)GLED1=OFF;
	
	if(MasterDataBuff[Grled3]==0x01)GLED3=ON;	//亮
	else if(MasterDataBuff[Grled3]==0x00)GLED3=OFF;

	if(MasterDataBuff[Grled5]==0x01)GLED5=ON;  //亮
	else if(MasterDataBuff[Grled5]==0x00)GLED5=OFF;
		
	
	if(MasterDataBuff[pum1]==0x01)Pump1=1;//控制泵启动
	else if(MasterDataBuff[pum1]==0x00)Pump1=0;//控制泵关闭

	if(MasterDataBuff[pum3]==0x01)Pump3=1;
	else if(MasterDataBuff[pum3]==0x00)Pump3=0;

	if(MasterDataBuff[pum5]==0x01)Pump5=1;
	else if(MasterDataBuff[pum5]==0x00)Pump5=0;

	
	if(MasterDataBuff[Valve1]==0x01)EXValve1=1;//控制继电器空气阀打开
	else if(MasterDataBuff[Valve1]==0x00)EXValve1=0;//控制继电器空气阀关闭
			
	if(MasterDataBuff[Valve3]==0x01)EXValve3=1;
	else if(MasterDataBuff[Valve3]==0x00)EXValve3=0;
	
	if(MasterDataBuff[Valve5]==0x01)EXValve5=1;
	else if(MasterDataBuff[Valve5]==0x00)EXValve5=0;

	if(MasterDataBuff[Sensor]==0x01)CapSensor=1;//打开5V水流总开关
	else if(MasterDataBuff[Sensor]==0x00)CapSensor=0;//关闭5V水流总开关
}

							