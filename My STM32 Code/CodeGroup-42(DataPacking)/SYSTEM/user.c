

#include "user.h"


//---------------------����˵��--------------------------------------------//
//�������ܣ�	���һ�����飬ʵ����ʹ��0������鸲��ԭ��������
//��ڲ�����	*array:	��������
//					len:	��Ҫ��յ�ǰ���ٸ�����
//����ֵ����

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/07
//�޸����ݣ�������ɻ�������
//-------------------------------
void ClearArray(char *array,u8 len)
{
	while(len)
	{
		len--;
		*(array+len) = 0;
	}
}

//���u8���͵�����
void ClearArray_u8(u8 *array,u8 len)
{
	while(len)
	{
		len--;
		*(array+len) = 0;
	}
}

#define NewLine 0		//�����ַ�����������Ҫ�Զ����ӻ������

//���ڷ����ַ�������
void USART_SendString(USART_TypeDef *USARTx,char *str)
{
	u16 k=0;
	while((*(str+k)) != '\0')
	{
		USART_SendData(USARTx,*(str+k));
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
		k++;
	}
	#if NewLine
			USART_SendData(USARTx,'\r');
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
			USART_SendData(USARTx,'\n');
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
	#endif	
}

//���ڷ���ָ���ֽ�u8������
void USART_SendLenData(USART_TypeDef *USARTx,u8 *DataBuff,u16 Len)
{
	u16 k=0;
	for(k=0;k<Len;k++)
	{
		USART_SendData(USARTx,*(DataBuff+k));
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
	}
	#if NewLine
			USART_SendData(USARTx,'\r');
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
			USART_SendData(USARTx,'\n');
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
	#endif	
}



//---------------------------------����˵��--------------------------------------------//
//�������ܣ�	����һ��Ӧ���ź�(У��ɹ���ŷ���Ӧ���ź�)
//��ڲ�����	*USARTx:����һ�����ڷ���
//					AckSource��ָʾӦ����ķ��ͷ�������˭����Ӧ����
//					*DATaFrame:�յ��ĵ�����֡��ַ��Ӧ���ź���Ҫ��ȡ�յ�������֡��������ݣ�
//����ֵ��	��
//---------------------------------��������---------------------------------------------
//text1:�����̷������յ����ݷ���Ӧ��
//		�յ����ݣ�AA 55 07 20 00 01 58 FF FF FF 81 55 AA			�������յ��豸����������
//		����Ӧ��AA 55 06 00 20 00 03 07 81 A3 55 AA 				�����̷���Ӧ����豸��Ӧ������Ϊ0x03
//text2���û��������յ����ݷ���Ӧ��
//		�յ����ݣ�AA 55 07 20 01 01 58 FF FF FF 80 55 AA			�û��յ��豸����������
//		����Ӧ��AA 55 06 01 20 00 A7 07 80 07 55 AA 				�û�����Ӧ����豸��Ӧ������Ϊ0xA7
//text3���豸�յ����ݷ���Ӧ��
//		�յ�����1��AA 55 07 00 20 01 58 FF FF FF 81 55 AA		�豸�յ������̷���������
//		����Ӧ��1��AA 55 06 20 00 00 72 07 81 D2 55 AA 			�豸����Ӧ��������̣�Ӧ������Ϊ0x72
//		�յ�����2��AA 55 07 01 20 01 58 FF FF FF 80 55 AA		�豸�յ��û�����������
//		����Ӧ��2��AA 55 06 20 01 00 72 07 80 D2 55 AA 			�豸����Ӧ����û���Ӧ������Ϊ0x72
//-------------------------------------------------------------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/16
//�޸����ݣ�������ɻ�������
//----------------------------
void SendACK(USART_TypeDef *USARTx,u8 AckSource,u8 *DataFrame)
{
	u8 datalen = 0x06;//����ָ�������������
	u8 AckType;
	u8 *point= DataFrame;
	u8 i=0;	//���ݴ��±�
	if(AckSource == 0x00)		AckType = 0x03;
	else if(AckSource == 0x01)		AckType = 0xA7;
	else if(AckSource == 0x20)		AckType = 0x72;
	u8 SendStr[14] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										AckSource,*(point+3),//���ͺͽ��շ�ʶ���룬
										0x00,//�Ƿ���ҪӦ�𡾹̶�����ҪӦ��
										AckType,//Ӧ�����룬����Э���ĵ��涨
										*(point+2),*(point+(*(point+2))+3)
										};
	u8 Parity = datalen;
	for(i=3;i<datalen+3;i++)//����У������
		Parity   =Parity^SendStr[i];//��λ�������
	SendStr[i]=Parity;i++;//����У������
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//�������ݰ�β��
//����֡������
	USART_SendLenData(USARTx,SendStr,datalen+6);//�������ݰ�
}





u8 AckFrame[14] = {0};//����һ��ȫ�ֱ����洢Ӧ�����ݰ�
void ACKPacking(u8 AckSource,u8 *DataFrame)
{
	u8 datalen = 0x06;//����ָ�������������
	u8 AckType;
	u8 *point= DataFrame;
	u8 i=0;	//���ݴ��±�
	if(AckSource == 0x00)		AckType = 0x03;
	else if(AckSource == 0x01)		AckType = 0xA7;
	else if(AckSource == 0x20)		AckType = 0x72;
	u8 AckFrame[14] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										AckSource,*(point+3),//���ͺͽ��շ�ʶ���룬
										0x00,//�Ƿ���ҪӦ�𡾹̶�����ҪӦ��
										AckType,//Ӧ�����룬����Э���ĵ��涨
										*(point+2),*(point+(*(point+2))+3)
										};
	u8 Parity = datalen;
	for(i=3;i<datalen+3;i++)//����У������
		Parity   =Parity^AckFrame[i];//��λ�������
	AckFrame[i]=Parity;i++;//����У������
	AckFrame[i]=0x55;i++;
	AckFrame[i]=0xAA;i++;//�������ݰ�β��
//����֡������
}


//--------------------------����˵��--------------------------------------------//
//�������ܣ�	��ɶ�����֡��У��
//��ڲ�����	*DATaFrame:��У�������֡��ַ
//����ֵ��	����0��ʾУ��ɹ�����������ֵ��ʾУ��ʧ��
//											��1:��ͷ����2:��β�򳤶ȴ���3:У��ʹ���

//-------------------------��������------------------------------------------------------//
//text1�����룺AA 55 06 20 21 00 58 FF FF 5F 55 AA    ����ֵ��0		˵��������֡У��ɹ�
//text2�����룺AA 55 06 20 21 00 58 FF FF 11 55 AA    ����ֵ��3		˵����У��ʹ���
//text3�����룺AA 06 20 21 00 58 FF FF 5F 55 AA       ����ֵ��1		˵������ͷ����
//text4�����룺AA 55 06 20 21 00 58 FF FF 5F 55       ����ֵ��2		˵������β����
//text5�����룺AA 55 04 20 21 00 58 FF FF 5F 55 AA    ����ֵ��2		˵�������ݳ��ȴ���
//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/15
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 DataValidityCheck(u8 *DataFrame)
{
	u8 i = 0,datalen = 0,Parity = 0;
	u8 *point = DataFrame;
	
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
//�������ܣ�	����У��ɹ��󣬵��øú�����������
//��ڲ�����	*DataBuff:��Ҫ����������֡��ַ
//����ֵ��		��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/16
//�޸����ݣ�������ɻ�������
//----------------------------
void DataAnalyze(u8 *DataBuff)
{
	u8 *point = DataBuff;
	u8 FunctionCode = *(point+6);//������
	
	//Ҫ��Ҫ���ж��ǲ��Ƿ����Լ��ģ�������˭���ģ�������
	
	switch(FunctionCode)//���ݹ����룬�������־λ����Ӧ���������ж�д����
	{
		case 0x55:
			break;
		case 0x57:
			break;
		case 0x58:
			break;
		case 0x59:
			break;
		case 0x5A:
			break;
		default:break;
	}
}









//----------------------------------����˵��--------------------------------------------//
//�������ܣ�	�����ݰ���Э������һ��֡
//��ڲ�����	len:���������ȣ���������ʶ����͹�����
//				localID�����ط����ߵı���
//				destinationID��Զ�̽����ߵı���
//				IfAck���Ƿ���ҪӦ���ź�
//				FunctionCode��������
//				*Data����Ҫ������������ĵ�ַ
//				*Packed�������洢�������ĵ�ַ
//����ֵ��	��
//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/19
//�޸����ݣ�������ɻ�������
//----------------------------
void DataPacking(u8 len, u8 localID,u8 destinationID,u8 IfAck,u8 FunctionCode,u8 *Data,u8 *Packed)
{
	u8 parity=len;
	u16 i=0;
	*(Packed+i) = 0xAA;i++;//���Ӱ�ͷAA
	*(Packed+i) = 0x55;i++;//���Ӱ�ͷ55
	*(Packed+i) = len;i++;//���ӳ���len
	*(Packed+i) = localID;i++;//���ӱ���ʶ����
	*(Packed+i) = destinationID;i++;//����Զ�̽�����ʶ����
	*(Packed+i) = IfAck;i++;//���ӹ�����1���Ƿ���ҪӦ��
	*(Packed+i) = FunctionCode;i++;//���ӹ�����

	for(i=0;i<len;i++)	//�洢����	
		*(Packed+7+i) = *(Data+i);//��������
	for(i=3;i<7+len;i++)
		parity = parity ^ *(Packed+i);//����У���
	
	i=len+7;
	*(Packed+i) = parity;i++;//����У���
	*(Packed+i) = 0x55;i++;//���Ӱ�β55
	*(Packed+i) = 0xAA;i++;//���Ӱ�βAA
}








