


#include "TaskUart.h"
#include "USART.h"






u8 ReceiveBuff[MaxReceiveLen];
//ʹ�õ�USART1����ʱ��Timer2��Ϊ�����ʷ�����
void TaskUart1_Init(u16 bound)
{
	COMx_InitDefine		COMx_InitStructure;					//�ṹ����
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;//ʹ�ò�����,   BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//�����ʼӱ�, ENABLE?DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//�ж�����,   ENABLE��DISABLE
	COMx_InitStructure.UART_Polity    = PolityHigh;			//�ж����ȼ�, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P36_P37;	//�л��˿�,  
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//�ڲ���·RXD��TXD, ���м�, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//��ʼ������1 
}


u8 ReceiveEnable=0;//״̬��״̬��־
u8 ReceiveCount=0;
//����״̬���ķ�ʽ
/********************* UART1�жϺ���************************/
void UART1_int (void) interrupt UART1_VECTOR
{
	u8 res;
	if(RI)//�����жϱ�־λ
	{
		res = SBUF;
		ReceiveBuff[ReceiveCount++] = res;//�浽������
			
		if(ReceiveEnable != 2)	//״̬����ʱ��δ��
		{
			ReceiveCount=0;
			switch (ReceiveEnable)
			{
				case 0://δ�յ�AA
				{
					if(res == 0xAA)
						ReceiveEnable = 1;
					break;
				}
				case 1://�յ�AA
				{
					if(res == 0x55)
					{
						ReceiveEnable =2;
						ReceiveBuff[0]=0xAA,ReceiveBuff[1]=0x55;//ȷ���յ�ͷ�����ֶ���ӵ���������
						ReceiveCount=2;//��һ�����ݴ�RX1_Buffer[2]��ʼ����
  					TR1 = 1;//������ʱ��1��
					}
					else if(res == 0xAA)	ReceiveEnable = 1;
					else ReceiveEnable = 0;
					break;
				}
				default:	break;
			}
		}
		RI=0;//���жϱ�־λ��׼��������һ���ֽ�
	}
}





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
			MasterDataBuff[MA_limit] = data_H;MasterDataBuff[MA_limit+1] = data_L;//EEPROM����������ȡ��д�룬�������
			TaskEEPROMFlag = 1;//��־λ��λ��EEPROM����д����ɺ����־λ
			break;
		}
		case 0x58://ѯ��ʣ�����ƴ���
		{
			FramePacking(0x70,&ReceiveBuff[SA_limit]);//ֱ���ϴ�
			//TaskUploadFlag = 1;//��λ���ϴ�����
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
			SlaveDataBuff[SA_time] = data_L;//���ر���ʱ����
			MasterDataBuff[MA_time] = data_L;
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


/**************************************************************************
���ܣ�STC15��Ƭ���Ĵ��ڷ����ֽڵĺ���
������dat:Ҫ���͵�һ���ֽ�   
**************************************************************************/
void UART_Send_Byte(unsigned char dat)
{
		ES=0;		 // ʹ�ò�ѯ���ͽ�����ʽ����ֹ�жϸ�Ԥ  ES �����ж� ����λ
		SBUF = dat;
		while(!TI);	    
		TI=0;	 //�˾���Բ�Ҫ����Ӱ��������ݵķ��ͣ�ֻ�������ѯ�����Ƿ������
		ES=1;
}	




void SendDataLen(u8 *DataBuff,u8 len)
{
	u8 i=0;
	u8 *point = DataBuff;
	for(i=0;i<len;i++)
	{
		UART_Send_Byte(*(point+i));
	}
}