

#include "user.h"


//Ӧ�����ݱ���Ҫ����Ļ����� &AckPacked[3]��ʼ��ֻ���7λ�ͺ�
//����Ҫ����Ϊ�ⲿ������ֻ�ڵ�ǰc�ļ�����ʹ��
#define AckLen 12	//���ȹ̶�Ϊ12
u8 AckPacked[AckLen]={0xAA,0x55,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x55,0xAA};
#define ClearHistoryAcked  ClearArray_u8(&AckPacked[3],7) 	//�궨��һ�������������ʷӦ������֡








//---------------------����˵��--------------------------------------------//
//�������ܣ�	���һ�����飬ʵ����ʹ��0������鸲��ԭ��������
//��ڲ�����	*array:	��������
//						len:	��Ҫ��յ�ǰ���ٸ�����
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

//���u8���͵�����,��ʵû��Ҫ��ֻ�ǲ��뿴��warning
void ClearArray_u8(u8 *array,u8 len)
{
	while(len)
	{
		len--;
		*(array+len) = 0;
	}
}


#define NewLine 0		//�����ַ�����������Ҫ�Զ���ӻ������

//---------------------����˵��--------------------------------------------//
//�������ܣ�	ָ�����ڷ���һ���ַ������ַ���Ĭ���� \0 ��Ϊ��������
//��ڲ�����	*USARTx��	�õ��Ĵ���
//						 *str:	Ҫ���͵��ַ���
//����ֵ����
//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/07
//�޸����ݣ�������ɻ�������
//-------------------------------
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
//Function OK



//---------------------����˵��--------------------------------------------//
//�������ܣ�	ָ�����ڷ���ָ�����ȵ�����
//��ڲ�����	*USARTx��	�õ��Ĵ���
//						 *DataBuff:	Ҫ���͵������׵�ַ
//						len��Ҫ���͵����ݳ���
//����ֵ����
//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/07
//�޸����ݣ�������ɻ�������
//-------------------------------
void USART_SendLenData(USART_TypeDef *USARTx,u8 *DataBuff,u16 Len)
{
	u16 k=0;
	for(k=0;k<Len;k++)
	{
		USART_SendData(USARTx,*(DataBuff+k));
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
	}
	#if NewLine|1
			USART_SendData(USARTx,'\r');
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
			USART_SendData(USARTx,'\n');
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
	#endif	
}
//Function OK




//----------------------------------����˵��--------------------------------------------//
//�������ܣ�	�����ݰ���Э������һ��֡
//��ڲ�����	len:���������ȣ��������ʶ����͹�����
//					localID�����ط����ߵı���
//					destinationID��Զ�̽����ߵı���
//					IfAck���Ƿ���ҪӦ���ź�
//					FunctionCode��������
//					*Data����Ҫ������������ĵ�ַ
//					*Packed�������洢�������ĵ�ַ
//����ֵ��	�������������֡�ĳ���
//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/20
//�޸����ݣ�������ɻ�������
//----------------------------
u8 DataPacking(u8 len, u8 localID,u8 destinationID,u8 IfAck,u8 FunctionCode,u8 *Data,u8 *Packed)
{
	u8 parity=len+4;
	u16 i=0;
	*(Packed+i) = 0xAA;i++;//��Ӱ�ͷAA
	*(Packed+i) = 0x55;i++;//��Ӱ�ͷ55
	*(Packed+i) = len+4;i++;//��ӳ���len
	*(Packed+i) = localID;i++;//��ӱ���ʶ����
	*(Packed+i) = destinationID;i++;//���Զ�̽�����ʶ����
	*(Packed+i) = IfAck;i++;//��ӹ�����1���Ƿ���ҪӦ��
	*(Packed+i) = FunctionCode;i++;//��ӹ�����

	for(i=0;i<len;i++)	//�洢����	
		*(Packed+7+i) = *(Data+i);//��������
	for(i=3;i<7+len;i++)
		parity = parity ^ *(Packed+i);//����У���
	
	i=len+7;
	*(Packed+i) = parity;i++;//���У���
	*(Packed+i) = 0x55;i++;//��Ӱ�β55
	*(Packed+i) = 0xAA;i++;//��Ӱ�βAA
	return len+10;//���ش������������֡�ĳ���
}
//Function OK






//--------------------------����˵��--------------------------------------------//
//�������ܣ�	��ɶ�����֡��У�飬У��ɹ��Ļ��ڸ����յ��Ĺ������ж��Ƿ���Ӧ���ź�
//��ڲ�����	*USARTx�������Ҫ����Ӧ���źţ�ѡ������һ�����ڷ���
//						*FrameReceive:�յ��Ĵ�У�������֡��ַ
//����ֵ��	����0��ʾУ��ɹ�����������ֵ��ʾУ��ʧ��
//											��1:��ͷ����2:��β�򳤶ȴ���3:У��ʹ���
//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/20
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 FrameCheckIfSendAck(USART_TypeDef *USARTx,u8 *FrameReceive)
{
	u8 i = 0,datalen = 0,Parity = 0;
	u8 *point = FrameReceive;
	u8 IfAck=*(point+5);//������1���Ƿ���ҪӦ���ź�
	
	if( (*(point) != 0xAA) || (*(point+1) != 0x55))
		return 1;//���ݰ�ͷ����
	
	datalen = *(point+2);//��ȡ���ݶγ��ȣ������������룩
	if( (*(point+datalen+4) != 0x55) || (*(point+datalen+5) != 0xAA))
		return 2;//���ݰ�β����
	
	Parity = datalen;
	for(i=3;i<datalen+3;i++)//����У���
		Parity =Parity ^ *(point+i);//��λ�������
	if(Parity != *(point+datalen+3))
		return 3;//У��ʹ���
	
	if(IfAck)
	{
		ClearHistoryAcked;//�����ԭ��Ӧ�����ݰ��е�����
		AckPacking(0x20,FrameReceive);//�������µ�Ӧ�����ݰ�
		USART_SendLenData(USARTx,AckPacked,AckLen);
	}
	return 0;//У��ɹ�
}
//Functiona OK






//--------------------------����˵��--------------------------------------------//
//�������ܣ�	Ӧ������֡���
//��ڲ�����	LocalID���������ID���豸Ϊ0x20�������������л����̶���
//					*FrameReceive:���յ������Ѿ�У��ɹ��������֡��ַ
//����ֵ��	�����Ӧ������֡�ĳ��ȣ���ʵ���ȹ̶�Ϊ10��
//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/20
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 AckPacking(u8 LocalID,u8 *FrameReceive)
{
	u8 *point= FrameReceive;
	u8 Parity = 0x06;
	u8 AckType;
	u8 i=0;	//���ݴ��±�
	
	AckPacked[3] = LocalID;//Ӧ����ķ��ͷ�����ʵ����û��Ҫ��STM32�豸�̶�Ϊ0x20
	AckPacked[4] = *(point+3);//Ӧ�����Ŀ���豸���		
	if(LocalID == 0x00)		AckType = 0x03;
	else if(LocalID == 0x01)		AckType = 0xA7;
	else if(LocalID == 0x20)		AckType = 0x72;
	AckPacked[6] = AckType;//Ӧ������
	AckPacked[3] = *(point+2);	//�յ������ݳ���
	AckPacked[3] = *(point+3+(*(point+2)));	//�յ���У���
	
	for(i=3;i<9;i++)//����У������
		Parity =Parity^AckPacked[i];//��λ�������
	AckPacked[i]=Parity;i++;//���У������
	AckPacked[i]=0x55;i++;
	AckPacked[i]=0xAA;i++;//������ݰ�β��
//����֡������
	return i;//���س���
}
//Function OK
















