

#ifndef __LED_H
#define __LED_H	 
#include "sys.h"


#define LED10 PCout(0)	// PC0	
#define LED11 PCout(1)	// PC1	
#define LED12 PCout(2)	// PC2	
#define LED13 PCout(3)	// PC3


void LEDInit(void);//≥ı ºªØ
void LedKeyTask(void);
void KEYInit(void);

		 				    
#endif
