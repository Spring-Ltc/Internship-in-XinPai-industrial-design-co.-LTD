#ifndef __HMI_HANDLE_H__
#define __HMI_HANDLE_H__
#include "stdio.h"	
#include "sys.h" 


#define EquipmentNo "KLE032005001"
#define SetMaintainerPassword 1234   //设置维护人员登陆密码


extern u8 FunctionCode;

extern int HMITtoBCTFlag;
extern int HMITtoAPTFlag;
extern int HMITtoMSTGetGprsStateFlag;
extern int HMITtoLKTFlag;
extern int HMITtoUSTTranspondBarCodeFlag;
extern int HMITtoMSTSetGprsPramFlag;
extern int HMITtoBSTGetSensorStateFlag;
extern int HMITtoUSTTranspondAccountDataFlag;
extern int HMITtoMSTRequestSynSeriesNumFlag;

extern char UserIDBuf[4], UserPasswordBuf[4], UserIDAndPasswordBuf[8];
extern char UserChangePasswordBuf[4];
extern char EquipmentNoAndPasswordBuf[16];

void HMITask(void);
void SensorStatusJudgmentInit(void);
#endif


