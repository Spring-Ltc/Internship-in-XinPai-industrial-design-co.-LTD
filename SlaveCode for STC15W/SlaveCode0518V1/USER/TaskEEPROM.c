

#include "TaskEEPROM.h"
#include "EEPROM.h"

void TaskEEPROM()
{
		EEPROM_read_n(0,&EEPROMDATA, 1);  //����EEPROM��1��ֵ
//	  if((--EEPROMDATA)<=245)  
//	       while(1){ EEPROM_SectorErase(0);            //д����
//	                 EEPROM_write_n(0,&(EEPROMDATA++),1);   
//	                 PrintString1("ERROR \r\n");
//                  }  //��ȥ����ֵһ��
	
	  EEPROMDATA--;
	  EEPROM_SectorErase(0);            //д����
	  EEPROM_write_n(0,&EEPROMDATA,1);  //д��
}