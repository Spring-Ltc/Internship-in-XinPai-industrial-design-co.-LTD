#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
								  
////////////////////////////////////////////////////////////////////////////////// 

#define LED0 PCout(0)
#define LED1 PCout(1)
#define LED2 PCout(2)
#define LED3 PCout(3)

#define LED0_ON 	PCout(0)=0
#define LED1_ON 	PCout(1)=0
#define LED2_ON		PCout(2)=0
#define LED3_ON		PCout(3)=0

#define LED0_OFF 	PCout(0)=1
#define LED1_OFF 	PCout(1)=1
#define LED2_OFF	PCout(2)=1
#define LED3_OFF	PCout(3)=1

void LED_Init(void);//≥ı ºªØ

		 				    
#endif
