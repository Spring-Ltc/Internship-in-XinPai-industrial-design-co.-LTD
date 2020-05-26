

#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

extern u8 TIM4Flag10Ms;
extern u8 TIM4Flag10Ms2;
extern u8 TIM4Flag10Ms3;
extern u8 TIM4Flag10Ms4;
extern u8 TIM4Flag10Ms5;

void TIM4_Int_Init(u16 arr,u16 psc); 
void TIM3_Int_Init(u16 arr,u16 psc); 
 
#endif
