

#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"






extern u16 adcx1 ,adcx2 ,adcx3;
extern int AP1, AP2, AP3;

extern int APTtoHMITFlag;

void AdcInit(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 
void AirPressureTask (void);
 
#endif 
