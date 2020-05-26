

#include "Slave.h"
#include "user.h"
#include "delay.h"

#define ReceiveState 20	//��Ҫ��ѯ20���豸
#define ControlState 23	//��Ҫ����23���豸

u8 SendBuff[UART4_DMA_DataLen]={0};
u8 ReceiveBuff[UART4_DMA_DataLen]={0};//�յ�����Ҫ���������
u8 UART4_RX_DMA_BUF[UART4_DMA_DataLen]={0};//����4��DMA���ջ���
u8 UART4_TX_DMA_BUF[UART4_DMA_DataLen]={0};//����4��DMA���ջ���
u8 Flag_UART4_Solve = 0;//���ݴ������־
u16 ReceiveLen = 0;

u16 shengyu=0;

//---------------------����˵��--------------------------------------------//
//�������ܣ�	��ʼ���ӻ��õ��Ĵ��ڣ������õĴ���4��ʹ��DMA���䷽ʽ
//					RX����ΪPC11��TX����ΪPC10
//��ڲ�����	bound���ʹӻ�ͨ�ŵĲ�����
//����ֵ��		��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/09
//�޸����ݣ�������ɻ�������
//--------------------------------
void SlaveInit(u32 bound)
{
	SlaveUSART_Init(bound);
	SlaveDMA_Init();
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);//�������ڿ����ж�
	USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);//��������DMA����
	USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);//��������DMA����
	USART_Cmd(UART4, ENABLE);//ʹ�ܴ���
}


//�ӻ����ڳ�ʼ��
void SlaveUSART_Init(u32 bound)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	//GPIO����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//UART4_TX   GPIOC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//UART4_RX	  GPIOC.11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);  
   //UART ��ʼ������
	USART_DeInit(UART4); //���ȸ�λһ�´��ڡ�
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(UART4, &USART_InitStructure); //��ʼ������5
  //UART4 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;	//�����ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

//�ӻ�DMA��ʼ��
void SlaveDMA_Init(void)
{
	DMA_InitTypeDef TX_DMA_InitStruct;//DMA�ṹ�����
	DMA_InitTypeDef RX_DMA_InitStruct;//DMA�ṹ�����
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE);//����DMAʱ��
	
	//DMA����TX
	DMA_DeInit(UART4_TX_DMA_Channel);//������ǰ���Ȼָ�Ĭ��ֵ��
	TX_DMA_InitStruct.DMA_BufferSize =	UART4_DMA_DataLen;		//DMAͨ��DMA����Ĵ�С
	TX_DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;		//DMA����ķ���,���ݵ�����ȥ
	TX_DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;		//�ڴ浽�ڴ�Ĵ���ʹ��
	TX_DMA_InitStruct.DMA_MemoryBaseAddr = (u32)DMA_SendAddress; 		//�ڴ��ַ,����������ݷ���ȥ	
	TX_DMA_InitStruct.DMA_MemoryDataSize =	DMA_MemoryDataSize_Byte;		//�ڴ����ݿ��
	TX_DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;		//�ڴ��ַ����ʹ��
	TX_DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//ѭ������//����Ϊ����Ҫѭ�����ա�
	TX_DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)DMA_PortAddress;		//�����ַ		
	TX_DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݿ��
	TX_DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//�����ַ����ʹ��
	TX_DMA_InitStruct.DMA_Priority = DMA_Priority_High;			//DMA���ȼ�
	DMA_Init(UART4_TX_DMA_Channel,&TX_DMA_InitStruct);
	//DMA_Cmd(UART4_TX_DMA_Channel,ENABLE);//���Ȳ�Ҫʹ�ܣ���Ȼ�ϵ�ͻᷢ��һ��0x00���ݳ�ȥ��
	
	//DMA����RX
	DMA_DeInit(UART4_RX_DMA_Channel);//������ǰ���Ȼָ�Ĭ��ֵ��
	RX_DMA_InitStruct.DMA_BufferSize =	UART4_DMA_DataLen;		//DMAͨ��DMA����Ĵ�С
	RX_DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;		//DMA����ķ������ݴ�������
	RX_DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;		//�ڴ浽�ڴ�Ĵ���ʹ��
	RX_DMA_InitStruct.DMA_MemoryBaseAddr = (u32)DMA_ReceiveAddress; 		//�ڴ��ַ			��ΪʲôҪת��u32���ͣ�����
	RX_DMA_InitStruct.DMA_MemoryDataSize =	DMA_MemoryDataSize_Byte;		//�ڴ����ݿ��
	RX_DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;		//�ڴ��ַ����ʹ��
	RX_DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//ѭ������//����Ϊ����Ҫѭ�����ա�
	RX_DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)DMA_PortAddress;		//�����ַ		
	RX_DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݿ��
	RX_DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//�����ַ����ʹ��
	RX_DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;			//DMA���ȼ�
	DMA_Init(UART4_RX_DMA_Channel,&RX_DMA_InitStruct);
	//DMA_Cmd(UART4_RX_DMA_Channel,ENABLE);//���Ȳ�Ҫʹ�ܣ���Ȼ�ϵ�ͻᷢ��һ��0x00���ݳ�ȥ��
}



//����ʹ��UART4��DMA���͹���
void TX_DMA_ENABLE(void)
{
	DMA_Cmd(UART4_TX_DMA_Channel,DISABLE);
	DMA_SetCurrDataCounter(UART4_TX_DMA_Channel,UART4_DMA_DataLen);
	DMA_Cmd(UART4_TX_DMA_Channel,ENABLE);
}
//����ʹ��UART4��DMA���չ���
void RX_DMA_ENABLE(void)
{
	DMA_Cmd(UART4_RX_DMA_Channel,DISABLE);
	DMA_SetCurrDataCounter(UART4_RX_DMA_Channel,UART4_DMA_DataLen);
	DMA_Cmd(UART4_RX_DMA_Channel,ENABLE);
}





//����4�жϷ������
void UART4_IRQHandler(void)                
{
	u8 i=0;
	if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)//���ڿ����ж�
	{
		//DMA_Cmd(UART4_RX_DMA_Channel,DISABLE);
		//USART_ClearITPendingBit(UART4,USART_IT_IDLE);//�崮�ڿ��б�־
		USART_ReceiveData(UART4);//���ն�һ�Ρ�
		
		if(Flag_UART4_Solve == 0)//�ϴε������Ѿ��������,����ת��
		{
			//Flag_UART4_Solve=1;//��λ��־���ȴ�������λ
			shengyu=DMA_GetCurrDataCounter(DMA2_Channel3);
			ReceiveLen = UART4_DMA_DataLen - shengyu;//DMA_GetCurrDataCounter(DMA2_Channel3);//�����յ����ֽ���
			for(i=0;i<ReceiveLen;i++)
				ReceiveBuff[i]=UART4_RX_DMA_BUF[i];//������ת�浽��Ҫ���������
			Flag_UART4_Solve=1;//��λ��־���ȴ�������λ
		}
		USART_ClearITPendingBit(UART4,USART_IT_IDLE);//�崮�ڿ��б�־
		RX_DMA_ENABLE();//�ٿ���DMA
	} 
} 






//
//-------------------------����ΪSTM32����������ã�����Ϊ�ʹӿ�ͨ�ŵĹ��ܺ���----------------------------------------------------------------------------------------------------------------
//



//---------------------����˵��--------------------------------------------//
//�������ܣ�	���ôӿذ�����ƴ���
//��ڲ�����	LimitNum�����õ�����ֵ
//����ֵ��	ָ���Ƿ�ִ�гɹ���0��ʾ�ɹ���������ʾʧ��	

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/13
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 ConfigLimit(u16 LimitNum)
{
	u8 datalen = 0x06;//����ָ�������������
	u8 Data_H =(u8) (LimitNum >>8);
	u8 Data_L =LimitNum & 0x00FF;
	u8 i=0;	//���ݴ��±�
	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										0x20,0x21,//���ͺͽ��շ������豸��ʶ����Ϊ0x02
										0x00,//�Ƿ���ҪӦ��
										0x57,//�����룬����Э���ĵ��涨
										Data_H,Data_L,//д������ݣ����ƴ���
										};
	u8 Parity = datalen;
	for(i=3;i<datalen+3;i++)//����У������
	{
		Parity   =Parity^SendStr[i];//��λ�������
	}
	SendStr[i]=Parity;i++;//���У������
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//������ݰ�β��
//����֡������

	USART_SendLenData(PortSlave,SendStr,datalen+6);//�������ݰ�
	delay_ms(500);
	if(QueryLimit() == LimitNum)//��ʱ���ж϶�ȡ��ֵ���趨��ֵ�Ƿ���ͬ
		return 0;
	else return 1;		//�趨��ֵ�Ͳ�ѯ��ֵ��һ��������ʧ��					
}
//Function OK 





//---------------------����˵��--------------------------------------------//
//�������ܣ�	��ѯ�ӿذ��ʣ��ʹ�ô���
//��ڲ�������
//����ֵ��		��ѯ�Ľ������󣬷���0xFFFF��ʾ����ָ��ִ�д���

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/10
//�޸����ݣ�������ɻ�������
//--------------------------------
u16 QueryLimit(void)
{
	u8 time = 5;//���ó�ʱʱ��
	u8 datalen = 0x06;//����ָ�������������
	u8 i=0;	//���ݴ��±�
	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										0x20,0x21,//���ͺͽ��շ������豸��ʶ����Ϊ0x02
										0x00,//�Ƿ���ҪӦ��
										0x58,//�����룬����Э���ĵ��涨
										0xFF,0xFF,//д������ݣ����ƴ���
										};
	u8 Parity = datalen;
	for(i=3;i<datalen+3;i++)//����У������
	{
		Parity   =Parity^SendStr[i];//��λ�������
	}
	SendStr[i]=Parity;i++;//���У������
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//������ݰ�β��
//����֡������

	ClearArray_u8(ReceiveBuff,UART4_DMA_DataLen);//��ս���
	USART_SendLenData(PortSlave,SendStr,datalen+6);//�������ݰ�
	Flag_UART4_Solve=0;//������մ���
	while(--time)
	{
		delay_ms(5);
		if(Flag_UART4_Solve)//���յ�����,�Ѿ�����ReceiveBuff��ȴ�����
		{
			//�����յ�������
			//��֤���ݵĺ����ԣ������򷵻���ȷ��ֵ��������������һ�ν��գ����ܶԷ����͵ĵڶ�֡���ǻ�Ӧ��
			while(Flag_UART4_Solve)
			{
				if(ReceiveLen != 12) break;//����֡������֤ʧ��
				if(ReceiveBuff[0] != 0xAA || ReceiveBuff[1] != 0x55 || 	//��ͷУ��
					 ReceiveBuff[ReceiveLen-1] != 0xAA || ReceiveBuff[ReceiveLen] != 0xAA)//��βУ��
					 break;//��ͷ���β��֤ʧ��
				Parity = ReceiveBuff[2];
				for(i=3;i< ReceiveBuff[2]+3;i++)
					Parity   =Parity^SendStr[i];//��λ�������
				if(Parity != ReceiveBuff[ReceiveLen-2]) break;//У�����֤ʧ��
				
				return ((u16)ReceiveBuff[ReceiveLen-4]<<8) + ((u16)ReceiveBuff[ReceiveLen-3]);
			}
			Flag_UART4_Solve=0;//��ǰ��֤ʧ�ܣ�������մ�����һ֡���ݣ��ڹ涨ʱ���ڼ�����֤
			ClearArray_u8(ReceiveBuff,UART4_DMA_DataLen);//��ս���
		}
	}
	return 0xFFFF;	//��ʱ���յ���������֤ʧ��				
}



//---------------------����˵��--------------------------------------------//
//�������ܣ�	���ôӿذ������ϴ����ݵ�����
//��ڲ�����	Period:ʱ�����ȼ�������С����ı�����0��ʾֹͣ�����ϴ�					
//����ֵ��	ָ���Ƿ�ִ�гɹ���0��ʾ�ɹ���������ʾʧ��		

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/10
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 ConfigUploadtime(u16 Period)
{
	u8 Period_H = (u8)(Period/256);//���ֽ�
	u8 Period_L = (u8)(Period%256);//���ֽ�
	u8 datalen = 0x06;//����ָ�������������
	u8 i=0;	//���ݴ��±�
	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										0x20,0x21,//���ͺͽ��շ�ʶ����
										0x00,//�Ƿ���ҪӦ��
										0x73,//�����룬����Э���ĵ��涨
										Period_H,Period_L,//�ϴ�ʱ����
										};
	u8 Parity = datalen;
	for(i=3;i<datalen+3;i++)//����У������
	{
		Parity   =Parity^SendStr[i];//��λ�������
	}
	SendStr[i]=Parity;i++;//���У������
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//������ݰ�β��
//����֡������
	
	//��ս���
	USART_SendLenData(PortSlave,SendStr,datalen+6);//�������ݰ�
	//���Ͳ�ѯָ����ղ�ѯ��������ò�ѯ������ɣ���ʱ�ں����ڲ�ʵ�֡�
	//�жϲ�ѯ���
	
	
	return 0;					
}




//---------------------����˵��--------------------------------------------//
//�������ܣ�	��ѯ�ӿذ�Ĺ���״̬
//��ڲ�����	Ackbuff����ѯ�Ľ����״̬���صĴ洢��ַ
//����ֵ��	ָ���Ƿ�ִ�гɹ���0��ʾ�ɹ���������ʾʧ��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/10
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 QueryWorkState(u8 *Ackbuff)
{
	u8 time = 5;//���ó�ʱʱ��
	u8 datalen = 0x06;//����ָ�������������
	u8 i=0;	//���ݴ��±�
	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										0x20,0x21,
										0x00,//�Ƿ���ҪӦ��
										0x72,//�����룬����Э���ĵ��涨
										0xFF,0xFF,//д������ݣ����
										};
	u8 Parity = datalen;
	for(i=3;i<datalen+3;i++)//����У������
	{
		Parity   =Parity^SendStr[i];//��λ�������
	}
	SendStr[i]=Parity;i++;//���У������
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//������ݰ�β��
//����֡������

	ClearArray_u8(ReceiveBuff,UART4_DMA_DataLen);//��ս���
	USART_SendLenData(PortSlave,SendStr,datalen+6);//�������ݰ�
	Flag_UART4_Solve=0;//������մ���
	while(--time)
	{
		if(Flag_UART4_Solve)//���յ�����
		{
			//�����յ�������
			//��֤���ݵĺ����ԣ������򷵻���ȷ��ֵ��������������һ�ν��գ����ܶԷ����͵ĵڶ�֡���ǻ�Ӧ��
			
			
		}
	}
	return 1;	//��ʱ���յ���������֤ʧ��	
}





//---------------------����˵��--------------------------------------------//
//�������ܣ�	���ذ���ӿذ巢�Ϳ�����Ϣ
//��ڲ�����	*State�����Ƶ�״̬�б�
//����ֵ��	ָ���Ƿ�ִ�гɹ���0��ʾ�ɹ���������ʾʧ��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/13
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 ControlSlave(u8 *State)
{
	u8 i=0;	//���ݴ��±�
	u8 ack[ReceiveState]={0};//���ú󣬷��ص�״̬
	u8 datalen = 0x1B;//����ָ�������������
	u8 Parity = datalen;
	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										0x20,0x21,
										0x00,//�Ƿ���ҪӦ��
										0x55,//�����룬����Э���ĵ��涨
										};
	for(i=0;i<23;i++)
		SendStr[i+7] = *(State+i);//����Ҫ���Ƶ�״̬������ӵ�����
										
	for(i=3;i<datalen+3;i++)//����У������
	{
		Parity   =Parity^SendStr[i];//��λ�������
	}
	SendStr[i]=Parity;i++;//���У������
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//������ݰ�β��
//����֡������
	
	//��ս���
	USART_SendLenData(PortSlave,SendStr,datalen+6);//�������ݰ�
	//���Ͳ�ѯָ����ղ�ѯ��������ò�ѯ������ɣ���ʱ�ں����ڲ�ʵ�֡�
	//�жϲ�ѯ���
	
	return 1;		
}
	
	

