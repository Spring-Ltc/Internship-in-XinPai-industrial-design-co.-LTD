

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
//�޸����ڣ�2020/05/15
//�޸����ݣ�������ɻ�������
//----------------------------
//void DataAnalyze(u8 *DataBuff)
//{
//	u8 *point;
//	u8 i;
//	u8 data_H,data_L;
//	u8 FunctionCode = *(point+6);//������
//	point = DataBuff;
//	switch(FunctionCode)
//	{
//		case 0x55:                                //���ذ����ӿذ�Ŀ�����Ϣ
//			for(i=0;i<26;i++)
//			MasterDataBuff[i]=*(point+6+i);//���������������ݶ�Ӧ�洢��MsterDataBuff
//			TaskControlFlag = 1;			
//			break;
//		case 0x57:  //�������Ƴ�Һ����
//			data_H = *(point+7);
//			data_L = *(point+8);
//			MasterDataBuff[21] = data_H;MasterDataBuff[22] = data_L; //EEPROM����������ȡ��д�룬�������
//			TaskEEPROMFlag = 1;  
//     // MasterControlEE();		//��־λ��λ��EEPROM����д����ɺ����־λ
//			break;
//		case 0x58:                                                  //ѯ��ʣ�����ƴ���
//			TaskUploadFlag=1;                                     //������ɺ�ֱ�ӷ���״̬���������������ݣ����ȡ״̬������ֻ���ʹ�������
//			//FramePacking(0x70,&SlaveDataBuff[TimH]);
//			break;
//		case 0x59:
//			TaskUploadFlag=2;//��ѯ����״̬
//			//FramePacking(0x71,SlaveDataBuff);
//			break;
//		case 0x5A:
//			//data_H = *(point+7);
//			data_L = *(point+8);	//ֻ�õ��˵�8λ����
//			SlaveDataBuff[20] = data_L;//���ر���ʱ�������������ز�ѯ�Ƿ����óɹ�
//			MasterDataBuff[20] = data_L;
//			TaskUploadFlag=3;//��ѯ����״̬//�����ϴ�ʱ����//TaskUploadFlag=1;//��ѯ����״̬//ֱ���޸������ϴ����ݼ���������ֵ,�����������ֵ��ȡMasterDataBuff��ֵ
//			break;
//		default:break;
//	}
//}

void DataAnalyze(u8 *DataBuff)
{
	u8 i=0;
	u8 *point;
	u8 data_H,data_L;
	u8 FunctionCode = *(point+6);//������
	point = DataBuff;
	switch(FunctionCode)
	{
		case 0x5B://���ذ����ӿذ�Ŀ�����Ϣ
		{
			for(i=0;i<6;i++)
			{
				if(( *(point+7+i))!= 0x02  )
					MasterDataBuff[i] = *(point+7+i);//д��6��LED�Ŀ���״̬
			}
			for(i=0;i<7;i++)
			{
				if(( *(point+19+i))!= 0x02  )
					MasterDataBuff[i+12] = *(point+19+i);//д��7���̵����Ŀ���״̬
			}
			for(i=0;i<3;i++)
			{
				if(( *(point+30+i))!= 0x02  )
					MasterDataBuff[i+23] = *(point+30+i);//ʹ��3����λ		
			}				
			TaskControlFlag = 1;
			break;
		}
		case 0x5C://�������Ƴ�Һ����
		{
			data_H = *(point+7);
			data_L = *(point+8);
			MasterDataBuff[21] = data_H;
			MasterDataBuff[22] = data_L;//EEPROM����������ȡ��д�룬�������
			TaskEEPROMFlag = 1;//��־λ��λ��EEPROM����д����ɺ����־λ
			break;
		}
		case 0x5D://ѯ��ʣ�����ƴ���
		{
			FramePacking(0x70,&SlaveDataBuff [21]);//ֱ���ϴ�
			//TaskUploadFlag = 1;//��λ���ϴ�����
			break;
		}
		case 0x5E://��ѯ����״̬
		{
			TaskUploadFlag = 1;//��λ���ϴ�����
			break;
		}
		case 0x60://�����ϴ�ʱ����
		{
			data_H = *(point+7);
			data_L = *(point+8);	//ֻ�õ��˵�8λ����
			SlaveDataBuff[20] = data_L;//���ر���ʱ����
			MasterDataBuff[20] = data_L;
			//TaskUploadFlag=3;
			break;
		}
		default:break;
	}	
}

//--------------------------����˵��--------------------------------------------//
//�������ܣ�	�����ݰ���Э������֡������
//��ڲ�����	FunctionCode:������
//						*DataBuff:��Ҫ���͵�������������
//����ֵ��		��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/16
//�޸����ݣ�������ɻ�������
//----------------------------
void FramePacking(u8 FunctionCode,u8 *DataBuff)
{

	u8 DataLen;//����������,ָ��Ч�����ݳ��ȣ��������豸����͹�����
	u8 *point = DataBuff;
	u8 Parity;
	u8 i=0;	//���ݴ��±�
	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										0xFF,//���ݶγ��ȡ�MCU�������𣿳�ʼ�������ñ�����ֵ��������дһ�ΰɡ�
										0x21,0x20,//���ͺͽ��շ�ʶ����
										0x00,//�Ƿ���ҪӦ��
										0xFF//�����룬����Э���ĵ��涨��MCU�������𣿳�ʼ�������ñ�����ֵ��������дһ�ΰɡ�
										};
	if(FunctionCode == 0x70)	DataLen =2;
	else if(FunctionCode == 0x71)	DataLen =23;	
	SendStr[2] = DataLen+4;//���°ѳ���д������֡���棬+4
	SendStr[6] = FunctionCode;//���°ѹ�����д������֡����								
	for(i=0;i<DataLen;i++)							
				SendStr[i+7] = *(point+i);	//���������	
										
	Parity = SendStr[2];
	for(i=3;i<SendStr[2]+3;i++)//����У������
		Parity = Parity^SendStr[i];//��λ�������
										
	SendStr[i]=Parity;i++;//���У������
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//������ݰ�β��
//����֡������

	SendDataLen(SendStr,DataLen+10);//�������ݰ�
}


