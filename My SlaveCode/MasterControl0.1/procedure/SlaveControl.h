#ifndef	__SLAVECONTROL_H
#define	__SLAVECONTROL_H


#define	COM_TX1_Lenth	32
#define	COM_RX1_Lenth	40
//#define	COM_TX2_Lenth	128
#define	COM_RX2_Lenth	40

#include "CupDet.h"
#include "config.h" 
#include "CupDet.h"
#include "USART1.h"
#include "eeprom.h"

#include	"timer.h"

void SlaveControl();
void SlaveControlEE();

u8  CheckSumTxd(u8 *dat,u8 length)

#endif