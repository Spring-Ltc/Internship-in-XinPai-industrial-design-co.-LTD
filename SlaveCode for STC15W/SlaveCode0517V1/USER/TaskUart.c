

#include "TaskUart.h"



//--------------------------����˵��--------------------------------------------//
//�������ܣ�	��ɶ�����֡��У��
//��ڲ�����	*DATaFrame:��У�������֡��ַ
//����ֵ��	����0��ʾУ��ɹ�����������ֵ��ʾУ��ʧ��
//											��1:��ͷ����2:��β�򳤶ȴ���3:У��ʹ���

//-------------------------��������----------------------------------------//
//text1�����룺AA 55 06 20 21 00 58 FF FF 5F 55 AA    ����ֵ��0		˵��������֡У��ɹ�
//text2�����룺AA 55 06 20 21 00 58 FF FF 11 55 AA    ����ֵ��3		˵����У��ʹ���
//text3�����룺AA 06 20 21 00 58 FF FF 5F 55 AA       ����ֵ��1		˵������ͷ����
//text4�����룺AA 55 06 20 21 00 58 FF FF 5F 55       ����ֵ��2		˵������β����
//text5�����룺AA 55 04 20 21 00 58 FF FF 5F 55 AA    ����ֵ��2		˵�������ݳ��ȴ���
//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/15
//�޸����ݣ�������ɻ�������
//----------------------------
u8 DataValidityCheck(u8 *DataFrame)
{
	u8 i = 0,datalen = 0,Parity = 0;
	u8 *point;
	point = DataFrame;
	if( (*(point) != 0xAA) || (*(point+1) != 0x55))
		return 1;//���ݰ�ͷ����
	
	datalen = *(point+2);//��ȡ���ݲ��ֳ���
	if( (*(point+datalen+4) != 0x55) || (*(point+datalen+5) != 0xAA))
		return 2;//���ݰ�β����
	
	Parity = datalen;
	for(i=3;i<datalen+3;i++)//����У���
		Parity =Parity ^ *(point+i);//��λ�������
	if(Parity != *(point+datalen+3))
		return 3;//У��ʹ���
	
	return 0;//У��ɹ�
}







//--------------------------����˵��--------------------------------------------//
//�������ܣ�	����У��ɹ��󣬴ӿذ���øú�����������
//��ڲ�����	*DataBuff:��Ҫ����������֡��ַ
//����ֵ��		��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/16
//�޸����ݣ�������ɻ�������
//----------------------------
void DataAnalyze(u8 *DataBuff)
{
	u8 i=0;
	u8 *point;
	u8 data_H,data_L;
	u8 FunctionCode = *(point+6);//������
	point = DataBuff;
	switch(FunctionCode)
	{
		case 0x55://���ذ����ӿذ�Ŀ�����Ϣ
		{
			for(i=0;i<6;i++)
				MasterDataBuff[i] = *(point+7+i);//д��6��LED�Ŀ���״̬
			for(i=0;i<8;i++)
				MasterDataBuff[i+12] = *(point+19+i);//д��8���̵����Ŀ���״̬
			for(i=0;i<3;i++)
				MasterDataBuff[i+23] = *(point+30+i);//ʹ��3����λ			
			TaskControlFlag = 1;
			break;
		}
		case 0x57://�������Ƴ�Һ����
		{
			data_H = *(point+7);
			data_L = *(point+8);
			MasterDataBuff[MAlimit] = data_H;MasterDataBuff[MAlimit+1] = data_L;//EEPROM����������ȡ��д�룬�������
			TaskEEPROMFlag = 1;//��־λ��λ��EEPROM����д����ɺ����־λ
			break;
		}
		case 0x58://ѯ��ʣ�����ƴ���
		{
			//������ɺ�ֱ�ӷ���״̬���������������ݣ����ȡ״̬������ֻ���ʹ�������,��TaskUploadFlag��λ��While(1)�����������
			//Send(SlaveDataBuff) �� Send(LimitNumber)
			TaskUploadFlag = 1;//��λ���ϴ�����
			break;
		}
		case 0x59://��ѯ����״̬
		{
			TaskUploadFlag = 1;//��λ���ϴ�����
			break;
		}
		case 0x5A://�����ϴ�ʱ����
		{
			data_H = *(point+7);
			data_L = *(point+8);	//ֻ�õ��˵�8λ����
			SlaveDataBuff[SAuploadtime] = data_L;//���ر���ʱ����
			MasterDataBuff[MAuploadtime] = data_L;
			//ֱ���޸������ϴ����ݼ���������ֵ,�����������ֵ��ȡMasterDataBuff��ֵ
			break;
		}
		default:break;
	}	
}




//--------------------------����˵��--------------------------------------------//
//�������ܣ�	�����ݰ���Э������֡������
//��ڲ�����	FunctionCode:������
//					*DataBuff:��Ҫ���͵�������������
//����ֵ��		��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/16
//�޸����ݣ�������ɻ�������
//----------------------------
//void FramePacking(u8 FunctionCode,u8 *DataBuff)
//{

//	u8 datalen;//����ָ�������������
//	u8 *point = DataBuff;
//	u8 Funcode = FunctionCode;
////	u8 SendStr[MaxSendStr];
//	u8 Parity;
//	u8 i=0;	//���ݴ��±�
//	if(FunctionCode == 0x70)	datalen = 4+2;
//	else if(FunctionCode == 0x71)	datalen = 4+23;
////	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
////										{0xAA,0x55,//���ݰ�ͷ
////									//	datalen,//���ݶγ���
////										0x21,0x20,//���ͺͽ��շ�ʶ����
////										0x00,//�Ƿ���ҪӦ��
////										Funcode//�����룬����Э���ĵ��涨
////										};
////	for(i=0;i<datalen-4;i++)	//���������							
////				SendStr[i+7] = *(point+i);	
////										
////	Parity = datalen;
////	for(i=3;i<datalen+3;i++)//����У������
////	{
////		Parity   =Parity^SendStr[i];//��λ�������
////	}
////	SendStr[i]=Parity;i++;//���У������
////	SendStr[i]=0x55;i++;
////	SendStr[i]=0xAA;i++;//������ݰ�β��
//////����֡������

////	USART_SendLenData(PortSlave,SendStr,datalen+6);//�������ݰ�
//}







