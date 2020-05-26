#ifndef __LAN_H__
#define __LAN_H__

#include "sys.h"

#define UART5_REC_LEN  			200  	//定义最大接收字节数 200

extern int USTtoHMITTranspondBarCodeSuccessFlag;
extern int USTtoHMITComparisonBarCodeSuccessFlag;
extern int USTtoHMITTranspondAccountDataSuccessFlag;
extern int USTtoMSTTranspondDataSuccessFlag;

extern u8 USTtoLANBuf[28];

void LanInit(u32 bound);
void UserServerTask(void);
#endif
