#include "sys.h"
#include "string.h"	
#include "slave.h"


u8  DMA_Rece_Buf[DMA_Rec_Len];	   //DMA���մ������ݻ�����
u16  Uart4_Rec_Cnt=0;             //��֡���ݳ���	
u8  DMA_Tx_Buf[DMA_Tx_Len];	   //DMA���մ������ݻ�����
u16  Uart4_Tx_Cnt=0;             //��֡���ݳ���	


//���������������ȡ�ӿذ��״̬���ݣ��ӿذ帺�������������ݡ�
u8 BoradSlaveBuf[29]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
											0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
											0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};



//----------------------------------����˵��--------------------------------------------
//�������ܣ�  ����4��DMA��ʼ��
//��ڲ�����	��
//����ֵ��	  ��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void UART4_DMA_Init(void)
{
	//UART4_RX��DMA_UART4_TX��DMA��ʼ��
	DMA_InitTypeDef DMA_UART4_RX;
	DMA_InitTypeDef DMA_UART4_TX;
	
	//RX��Ӧ��DMA����
	DMA_DeInit(DMA2_Channel3);   //��DMA��ͨ��5�Ĵ�������Ϊȱʡֵ  ����4��Ӧ����DMAͨ��13
	DMA_UART4_RX.DMA_PeripheralBaseAddr = (u32)&UART4->DR;  //DMA����ADC����ַ
	DMA_UART4_RX.DMA_MemoryBaseAddr = (u32)DMA_Rece_Buf;  //DMA�ڴ����ַ
	DMA_UART4_RX.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴������ȡ���͵��ڴ�
	DMA_UART4_RX.DMA_BufferSize = DMA_Rec_Len;  //DMAͨ����DMA����Ĵ�С
	DMA_UART4_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_UART4_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_UART4_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_UART4_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_UART4_RX.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_UART4_RX.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
	DMA_UART4_RX.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA2_Channel3, &DMA_UART4_RX);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��UART4_Tx_DMA_Channe13����ʶ�ļĴ���
		
	DMA_Cmd(DMA2_Channel3, ENABLE);  //��ʽ����DMA����
	
	//TX��Ӧ��DMA����
	DMA_DeInit(DMA2_Channel5);   //��DMA��ͨ��5�Ĵ�������Ϊȱʡֵ  ����4��Ӧ����DMAͨ��15
	DMA_UART4_TX.DMA_PeripheralBaseAddr = (u32)&UART4->DR;  //DMA����ADC����ַ
	DMA_UART4_TX.DMA_MemoryBaseAddr = (u32)DMA_Tx_Buf;  //DMA�ڴ����ַ
	DMA_UART4_TX.DMA_DIR = DMA_DIR_PeripheralDST;  //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_UART4_TX.DMA_BufferSize = DMA_Tx_Len;  //DMAͨ����DMA����Ĵ�С
	DMA_UART4_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_UART4_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_UART4_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_UART4_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_UART4_TX.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_UART4_TX.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
	DMA_UART4_TX.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA2_Channel5, &DMA_UART4_TX);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��UART4_Tx_DMA_Channe15����ʶ�ļĴ���
		
	DMA_Cmd(DMA2_Channel5, ENABLE);  //��ʽ����DMA����
}
	



//----------------------------------����˵��--------------------------------------------
//�������ܣ�  ����ʹ��UART4_RX��DMA����
//��ڲ�����	��
//����ֵ��	  ��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void UART4_RX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA2_Channel3, DISABLE ); //��ֹͣDMA����ͣ���� 
 	DMA_SetCurrDataCounter(DMA2_Channel3, DMA_Rec_Len); //DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA2_Channel3, ENABLE); //ʹ��UART4 RX DMA1 ��ָʾ��ͨ�� 
}	



//----------------------------------����˵��--------------------------------------------
//�������ܣ�  ����ʹ��UART4_TX��DMA����
//��ڲ�����	��
//����ֵ��	  ��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void UART4_TX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA2_Channel5, DISABLE ); //��ֹͣDMA
 	DMA_SetCurrDataCounter(DMA2_Channel5, DMA_Tx_Len); //DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA2_Channel5, ENABLE); //ʹ��UART4 TX DMA1 ��ָʾ��ͨ�� 
}	





//----------------------------------����˵��--------------------------------------------
//�������ܣ�  ���ӿذ�ͨ�Ŷ�Ӧ�Ĵ���4��ʼ��
//��ڲ�����	bound��������
//����ֵ��	  ��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void SlaveInit(u32 bound){
	GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//ʹ��UART4ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	//ʹ��DMA����
	
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound; //���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No; //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(UART4, &USART_InitStructure); //��ʼ������4
	
	//UART4_TX   GPIOC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.10
   
  //UART4_RX	  GPIOC.11��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC.11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.11
	
	//Uart4 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); //���������ж�
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE); //ʹ�ܴ���4��DMA����
	USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE); //ʹ�ܴ���4��DMA����
	USART_Cmd(UART4, ENABLE); //ʹ�ܴ���4 
	
	UART4_DMA_Init();  //UART4��DMA���ܳ�ʼ��
}




//----------------------------------����˵��--------------------------------------------
//�������ܣ�  ����4�жϷ������
//��ڲ�����	��
//����ֵ��	  ��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void UART4_IRQHandler(void)
{
	//����1�����ж�
	if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)  
	{	
		/* 1.�����־ */
		USART_ClearITPendingBit(UART4, USART_IT_IDLE);	 //����жϱ�־
		
		/* 2.��ȡDMA */
		USART_ReceiveData(UART4); 	//��ȡ����
		Uart4_Rec_Cnt = DMA_Rec_Len - DMA_GetCurrDataCounter(DMA2_Channel3); 	//���ո������ڽ��ջ������ܴ�С���Ѿ����յĸ���
		
		/* 3.�������ݽ����������� */
		memcpy(DMA_Tx_Buf, DMA_Rece_Buf, Uart4_Rec_Cnt); 	//������ת��ͨ������4��DMA��ʽ���ͳ�ȥ����
		UART4_TX_DMA_Enable(); 	//����һ��DMA���ͣ�ʵ��ת��

		/* 4.�����µ�һ��DMA���� */
		UART4_RX_DMA_Enable(); 	//����ʹ��DMA���ȴ���һ�εĽ���
   } 
} 
	





void BoradSlaveTask(void)
{
	
//	u8 BoradSlaveBuf[29]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x00, 0x00, 0x00, 0x00};
	////////////
}










//
//-------------------------����ΪSTM32����������ã�����Ϊ�ʹӿ�ͨ�ŵĹ��ܺ���----------------------------------------------------------------------------------------------------------------
//



////---------------------����˵��--------------------------------------------//
////�������ܣ�	���ôӿذ�����ƴ���
////��ڲ�����	LimitNum�����õ�����ֵ
////����ֵ��	ָ���Ƿ�ִ�гɹ���0��ʾ�ɹ���������ʾʧ��	

////--------------------------------
////���ߣ�Liao_tingchun		
////�޸����ڣ�2020/05/13
////�޸����ݣ�������ɻ�������
////--------------------------------
//u8 ConfigLimit(u16 LimitNum)
//{
//	u8 datalen = 0x06;//����ָ�������������
//	u8 Data_H =(u8) (LimitNum >>8);
//	u8 Data_L =LimitNum & 0x00FF;
//	u8 i=0;	//���ݴ��±�
//	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
//										{0xAA,0x55,//���ݰ�ͷ
//										datalen,//���ݶγ���
//										0x20,0x21,//���ͺͽ��շ������豸��ʶ����Ϊ0x02
//										0x00,//�Ƿ���ҪӦ��
//										0x57,//�����룬����Э���ĵ��涨
//										Data_H,Data_L,//д������ݣ����ƴ���
//										};
//	u8 Parity = datalen;
//	for(i=3;i<datalen+3;i++)//����У������
//	{
//		Parity   =Parity^SendStr[i];//��λ�������
//	}
//	SendStr[i]=Parity;i++;//���У������
//	SendStr[i]=0x55;i++;
//	SendStr[i]=0xAA;i++;//������ݰ�β��
////����֡������

//	USART_SendLenData(PortSlave,SendStr,datalen+6);//�������ݰ�
//	delay_ms(500);
//	if(QueryLimit() == LimitNum)//��ʱ���ж϶�ȡ��ֵ���趨��ֵ�Ƿ���ͬ
//		return 0;
//	else return 1;		//�趨��ֵ�Ͳ�ѯ��ֵ��һ��������ʧ��					
//}
////Function OK 





////---------------------����˵��--------------------------------------------//
////�������ܣ�	��ѯ�ӿذ��ʣ��ʹ�ô���
////��ڲ�������
////����ֵ��		��ѯ�Ľ������󣬷���0xFFFF��ʾ����ָ��ִ�д���

////--------------------------------
////���ߣ�Liao_tingchun		
////�޸����ڣ�2020/05/10
////�޸����ݣ�������ɻ�������
////--------------------------------
//u16 QueryLimit(void)
//{
//	u8 time = 5;//���ó�ʱʱ��
//	u8 datalen = 0x06;//����ָ�������������
//	u8 i=0;	//���ݴ��±�
//	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
//										{0xAA,0x55,//���ݰ�ͷ
//										datalen,//���ݶγ���
//										0x20,0x21,//���ͺͽ��շ������豸��ʶ����Ϊ0x02
//										0x00,//�Ƿ���ҪӦ��
//										0x58,//�����룬����Э���ĵ��涨
//										0xFF,0xFF,//д������ݣ����ƴ���
//										};
//	u8 Parity = datalen;
//	for(i=3;i<datalen+3;i++)//����У������
//	{
//		Parity   =Parity^SendStr[i];//��λ�������
//	}
//	SendStr[i]=Parity;i++;//���У������
//	SendStr[i]=0x55;i++;
//	SendStr[i]=0xAA;i++;//������ݰ�β��
////����֡������

//	ClearArray_u8(ReceiveBuff,UART4_DMA_DataLen);//��ս���
//	USART_SendLenData(PortSlave,SendStr,datalen+6);//�������ݰ�
//	Flag_UART4_Solve=0;//������մ���
//	while(--time)
//	{
//		delay_ms(5);
//		if(Flag_UART4_Solve)//���յ�����,�Ѿ�����ReceiveBuff��ȴ�����
//		{
//			//�����յ�������
//			//��֤���ݵĺ����ԣ������򷵻���ȷ��ֵ��������������һ�ν��գ����ܶԷ����͵ĵڶ�֡���ǻ�Ӧ��
//			while(Flag_UART4_Solve)
//			{
//				if(ReceiveLen != 12) break;//����֡������֤ʧ��
//				if(ReceiveBuff[0] != 0xAA || ReceiveBuff[1] != 0x55 || 	//��ͷУ��
//					 ReceiveBuff[ReceiveLen-1] != 0xAA || ReceiveBuff[ReceiveLen] != 0xAA)//��βУ��
//					 break;//��ͷ���β��֤ʧ��
//				Parity = ReceiveBuff[2];
//				for(i=3;i< ReceiveBuff[2]+3;i++)
//					Parity   =Parity^SendStr[i];//��λ�������
//				if(Parity != ReceiveBuff[ReceiveLen-2]) break;//У�����֤ʧ��
//				
//				return ((u16)ReceiveBuff[ReceiveLen-4]<<8) + ((u16)ReceiveBuff[ReceiveLen-3]);
//			}
//			Flag_UART4_Solve=0;//��ǰ��֤ʧ�ܣ�������մ�����һ֡���ݣ��ڹ涨ʱ���ڼ�����֤
//			ClearArray_u8(ReceiveBuff,UART4_DMA_DataLen);//��ս���
//		}
//	}
//	return 0xFFFF;	//��ʱ���յ���������֤ʧ��				
//}



////---------------------����˵��--------------------------------------------//
////�������ܣ�	���ôӿذ������ϴ����ݵ�����
////��ڲ�����	Period:ʱ�����ȼ�������С����ı�����0��ʾֹͣ�����ϴ�					
////����ֵ��	ָ���Ƿ�ִ�гɹ���0��ʾ�ɹ���������ʾʧ��		

////--------------------------------
////���ߣ�Liao_tingchun		
////�޸����ڣ�2020/05/10
////�޸����ݣ�������ɻ�������
////--------------------------------
//u8 ConfigUploadtime(u16 Period)
//{
//	u8 Period_H = (u8)(Period/256);//���ֽ�
//	u8 Period_L = (u8)(Period%256);//���ֽ�
//	u8 datalen = 0x06;//����ָ�������������
//	u8 i=0;	//���ݴ��±�
//	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
//										{0xAA,0x55,//���ݰ�ͷ
//										datalen,//���ݶγ���
//										0x20,0x21,//���ͺͽ��շ�ʶ����
//										0x00,//�Ƿ���ҪӦ��
//										0x73,//�����룬����Э���ĵ��涨
//										Period_H,Period_L,//�ϴ�ʱ����
//										};
//	u8 Parity = datalen;
//	for(i=3;i<datalen+3;i++)//����У������
//	{
//		Parity   =Parity^SendStr[i];//��λ�������
//	}
//	SendStr[i]=Parity;i++;//���У������
//	SendStr[i]=0x55;i++;
//	SendStr[i]=0xAA;i++;//������ݰ�β��
////����֡������
//	
//	//��ս���
//	USART_SendLenData(PortSlave,SendStr,datalen+6);//�������ݰ�
//	//���Ͳ�ѯָ����ղ�ѯ��������ò�ѯ������ɣ���ʱ�ں����ڲ�ʵ�֡�
//	//�жϲ�ѯ���
//	return 0;					
//}




////---------------------����˵��--------------------------------------------//
////�������ܣ�	��ѯ�ӿذ�Ĺ���״̬
////��ڲ�����	Ackbuff����ѯ�Ľ����״̬���صĴ洢��ַ
////����ֵ��	ָ���Ƿ�ִ�гɹ���0��ʾ�ɹ���������ʾʧ��

////--------------------------------
////���ߣ�Liao_tingchun		
////�޸����ڣ�2020/05/10
////�޸����ݣ�������ɻ�������
////--------------------------------
//u8 QueryWorkState(u8 *Ackbuff)
//{
//	u8 time = 5;//���ó�ʱʱ��
//	u8 datalen = 0x06;//����ָ�������������
//	u8 i=0;	//���ݴ��±�
//	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
//										{0xAA,0x55,//���ݰ�ͷ
//										datalen,//���ݶγ���
//										0x20,0x21,
//										0x00,//�Ƿ���ҪӦ��
//										0x72,//�����룬����Э���ĵ��涨
//										0xFF,0xFF,//д������ݣ����
//										};.
//	u8 Parity = datalen;
//	for(i=3;i<datalen+3;i++)//����У������
//	{
//		Parity   =Parity^SendStr[i];//��λ�������
//	}
//	SendStr[i]=Parity;i++;//���У������
//	SendStr[i]=0x55;i++;
//	SendStr[i]=0xAA;i++;//������ݰ�β��
////����֡������

//	ClearArray_u8(ReceiveBuff,UART4_DMA_DataLen);//��ս���
//	USART_SendLenData(PortSlave,SendStr,datalen+6);//�������ݰ�
//	Flag_UART4_Solve=0;//������մ���
//	while(--time)
//	{
//		if(Flag_UART4_Solve)//���յ�����
//		{
//			//�����յ�������
//			//��֤���ݵĺ����ԣ������򷵻���ȷ��ֵ��������������һ�ν��գ����ܶԷ����͵ĵڶ�֡���ǻ�Ӧ��
//			
//			
//		}
//	}
//	return 1;	//��ʱ���յ���������֤ʧ��	
//}





////---------------------����˵��--------------------------------------------//
////�������ܣ�	���ذ���ӿذ巢�Ϳ�����Ϣ
////��ڲ�����	*State�����Ƶ�״̬�б�
////����ֵ��	ָ���Ƿ�ִ�гɹ���0��ʾ�ɹ���������ʾʧ��

////--------------------------------
////���ߣ�Liao_tingchun		
////�޸����ڣ�2020/05/13
////�޸����ݣ�������ɻ�������
////--------------------------------
//u8 ControlSlave(u8 *State)
//{
//	u8 i=0;	//���ݴ��±�
//	u8 ack[ReceiveState]={0};//���ú󣬷��ص�״̬
//	u8 datalen = 0x1B;//����ָ�������������
//	u8 Parity = datalen;
//	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
//										{0xAA,0x55,//���ݰ�ͷ
//										datalen,//���ݶγ���
//										0x20,0x21,
//										0x00,//�Ƿ���ҪӦ��
//										0x55,//�����룬����Э���ĵ��涨
//										};
//	for(i=0;i<23;i++)
//		SendStr[i+7] = *(State+i);//����Ҫ���Ƶ�״̬������ӵ�����
//										
//	for(i=3;i<datalen+3;i++)//����У������
//	{
//		Parity   =Parity^SendStr[i];//��λ�������
//	}
//	SendStr[i]=Parity;i++;//���У������
//	SendStr[i]=0x55;i++;
//	SendStr[i]=0xAA;i++;//������ݰ�β��
////����֡������
//	
//	//��ս���
//	USART_SendLenData(PortSlave,SendStr,datalen+6);//�������ݰ�
//	//���Ͳ�ѯָ����ղ�ѯ��������ò�ѯ������ɣ���ʱ�ں����ڲ�ʵ�֡�
//	//�жϲ�ѯ���
//	
//	return 1;		
//}
//	
//	

