#include "SlaveControl.h"




/**********************************************************************************/
//���ƣ�void void SlaveControl()
//���ܣ��ӿص��������ݰ�/�ϴ�
//������֡ͷ+����+ʶ����+������(0x71������)+���ݶ�+Ч����+֡β
//       TX1_Buffer�����ͻ�������
//       RLED�������жϽ�ֵ���뷢�ͻ�������
//����ֵ��void
//--------------------------------
//���ߣ�XiongXiaoping 	
//�޸����ڣ�2020/05/07
//�޸����ݣ����ݲɼ����뷢�ͻ�������
//----------------------------
/**********************************************************************************/
u8 Buffer[12];
//void SlaveControl()
//{
//    TX1_Buffer[0]=0xAA;  //֡ͷ
//    TX1_Buffer[1]=0x55;  //֡ͷ
//    
//    TX1_Buffer[2]=0x17;   //���ݳ���
//			
//		TX1_Buffer[3]=0x20;    //�豸�����豸
//		TX1_Buffer[4]=0x21;    //�����豸���͸��豸
//			
//		TX1_Buffer[5]=0x00;    //δҪ��Ӧ��
//    TX1_Buffer[6]=0x71;    //����������
//			
//		
//			
//    TX1_Buffer[7]=(RLED1==ON)?0x01:0x00;
//    TX1_Buffer[8]=(RLED3==ON)?0x01:0x00;	//���״̬
//		TX1_Buffer[9]=(RLED5==ON)?0x01:0x00;
//		
//    TX1_Buffer[10]=(GLED1==ON)?0x01:0x00;
//    TX1_Buffer[11]=(GLED3==ON)?0x01:0x00;	//�̵�״̬
//		TX1_Buffer[12]=(GLED5==ON)?0x01:0x00;
//		
//    TX1_Buffer[13]=(Cup1==1)?0x01:0x00;
//    TX1_Buffer[14]=(Cup3==1)?0x01:0x00;	//ˮ������״̬
//		TX1_Buffer[15]=(Cup5==1)?0x01:0x00;
//    
//		TX1_Buffer[16]=(Water1Sensor==1)?0x01:0x00;
//    TX1_Buffer[17]=(Water3Sensor==1)?0x01:0x00;	//����ˮ���
//		TX1_Buffer[18]=(Water5Sensor==1)?0x01:0x00;
//   
//    TX1_Buffer[19]=(Pump1==1)?0x01:0x00;
//    TX1_Buffer[20]=(Pump3==1)?0x01:0x00;	//��״̬���
//		TX1_Buffer[21]=(Pump5==1)?0x01:0x00;
//    
//    TX1_Buffer[22]=(EXValve1==1)?0x01:0x00;
//    TX1_Buffer[23]=(EXValve3==1)?0x01:0x00;	//������״̬���
//		TX1_Buffer[24]=(EXValve5==1)?0x01:0x00;

//    TX1_Buffer[25]=(CapSensor==1)?0x01:0x00;	//5V��ѹ����
//		TX1_Buffer[26]=0x00;
//    //TX1_Buffer[26]=(ChassisLock==1)?0x01:0x00;	//������״̬
//		if(RX1_Buffer[6]==0X73)
//		{
//		TX1_Buffer[27]=RX1_Buffer[8];	//5V��ѹ����
//		}
//		else
//		{
//			TX1_Buffer[27]=0x00;
//		}
//    TX1_Buffer[28]=CheckSumTxd(TX1_Buffer,25);//У���
//		//TX1_Buffer[26]=0x00;	//��ʱΪЧ����    ����ʹ�ô�������
//		
//		TX1_Buffer[29]=0x55;
//		TX1_Buffer[30]=0xAA;	//֡β
//}
/**********************************************************************************/
/**********************************************************************************/


/**********************************************************************************/
//���ƣ�void SlaveControlEE()
//���ܣ��ϱ�EEPROMʣ�౭�ӵĴ���
//������֡ͷ+����+ʶ����+������(0x70������)+���ݶ�+Ч����+֡β
//      
//����ֵ��void
//--------------------------------
//���ߣ�XiongXiaoping 	
//�޸����ڣ�2020/05/07
//�޸����ݣ���ѯʣ�౭����
//----------------------------
/**********************************************************************************/
void SlaveControlEE()
{
	

		Buffer[0]=0xAA;  //֡ͷ
    Buffer[1]=0x55;  //֡ͷ
    
    Buffer[2]=0x06;   //���ݳ���
			
		Buffer[3]=0x20;    //�豸�����豸
    Buffer[4]=0x21;    //�����豸�����豸
			
		Buffer[5]=0x00;    //δҪ��Ӧ��
    Buffer[6]=0x70;    //����������
	
	
		Buffer[7]=EEPROM_read_n(0x0001);
		Buffer[8]=EEPROM_read_n(0x0000);	//  ��ȡ����ʣ�����
		
	//  Buffer[9]= CheckSumTxd(TX1_Buffer,0x08);
	//	TX1_Buffer[9]=0x00;	//��ʱΪЧ����   
	
		Buffer[10]=0x55;
		Buffer[11]=0xAA;	//֡β
	
}	
/**********************************************************************************/

