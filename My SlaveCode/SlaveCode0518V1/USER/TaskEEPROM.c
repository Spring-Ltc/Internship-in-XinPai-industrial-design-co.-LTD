

#include "TaskEEPROM.h"
#include "EEPROM.h"

void TaskEEPROM()
{
		EEPROM_read_n(0,&EEPROMDATA, 1);  //读出EEPROM的1个值
//	  if((--EEPROMDATA)<=245)  
//	       while(1){ EEPROM_SectorErase(0);            //写扇区
//	                 EEPROM_write_n(0,&(EEPROMDATA++),1);   
//	                 PrintString1("ERROR \r\n");
//                  }  //减去读出值一次
	
	  EEPROMDATA--;
	  EEPROM_SectorErase(0);            //写扇区
	  EEPROM_write_n(0,&EEPROMDATA,1);  //写回
}