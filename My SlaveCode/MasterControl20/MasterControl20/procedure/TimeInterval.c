
#include "TimeInterval.h"

//u8	idata TX1_Buffer[COM_TX1_Lenth];	//���ͻ���
//u8 	idata RX1_Buffer[COM_RX1_Lenth];	//���ջ���

extern T0_500MS2,T0_100MS10,T1_1MS1,T0_10MS2,MS3,T0_100MS1,T2_10MS2  ;  //�����ļ�����ı���(�ⲿ����)
extern u8 TaskUploadFlag;//����3�������ϴ�����
/**********************************************************************************/
//���ƣ�void TimeInterval()
//���ܣ���ѯ�����ϱ�ʱ����
//������ 
//����ֵ������ֵΪ���ƶ�ʱ���жϴ�����void
//--------------------------------
//���ߣ�XiongXiaoping 	
//�޸����ڣ�2020/05/09
//�޸����ݣ����ôӿذ�������ϴ����ݵ�ʱ����
//----------------------------
/**********************************************************************************/
void TimeInterval()
{
	         u8 vt1=0;
	//         u8 i;

					if(MasterDataBuff[20]!=0x00)	//���ܱ�־��
					{
				 if((MasterDataBuff[20]<=0x64))	//���ܱ�־�������ϱ�ʱ�������
					{
            if( T0_500MS2)//100ms
						{ 
							vt1++;
							T0_500MS2=0;
							if(vt1>=(10*(u8)(MasterDataBuff[20])))
							{
							vt1=0;
							FramePacking(0x71,SlaveDataBuff);
							}
						 }					
					}
				}
				 		
}					


/**********************************************************************************/	
/**********************************************************************************/