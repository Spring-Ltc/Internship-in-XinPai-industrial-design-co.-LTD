#include "sys.h"
#include "string.h"	
#include "slave.h"
#include "user.h"
#include "hmi_handle.h"
#include "delay.h"



//#define FunctionCodeOfConfigLimit 0x58
//#define FunctionCodeOfConfigUploadtime  0x59
//#define FunctionCodeOf  0x59
//#define FunctionCodeOf  0x59
//#define FunctionCodeOf  0x59


int BSTtoHMITGetSensorStateSuccessFlag = 0;


u8  DMA_Rece_Buf[DMA_Rec_Len];	   //DMA���մ������ݻ�����
u16  Uart4_Rec_Cnt=0;             //��֡���ݳ���	
u8  DMA_Tx_Buf[DMA_Tx_Len];	   //DMA���մ������ݻ�����
u16  Uart4_Tx_Cnt=0;             //��֡���ݳ���	

u8 BoradSlaveBuf[35];   //ʵ��33

//={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x00, 0x00, 0x00, 0x00};

u8 RLED1State, RLED3State, RLED5State, GLED1State, GLED3State, GLED5State;
u8 Cup1State, Cup3State, Cup5State;
u8 Water1SensorState, Water3SensorState, Water5SensorState, WaterSensorPowerState;
u8 PUMP1State, PUMP3State, PUMP5State;
u8 VALVE1State, VALVE3State, VALVE5State;
u8 LockState;
u8 SetUploadTimeInterval;
u16 ReportRemainLimits;



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
void SlaveInit(u32 bound)
	{
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
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	
	//����1�����ж�
	if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)  
	{	
		/* 1.�����־ */
		USART_ClearITPendingBit(UART4, USART_IT_IDLE);	 //����жϱ�־
		
		/* 2.��ȡDMA */
		USART_ReceiveData(UART4); 	//��ȡ����
		Uart4_Rec_Cnt = DMA_Rec_Len - DMA_GetCurrDataCounter(DMA2_Channel3); 	//���ո������ڽ��ջ������ܴ�С���Ѿ����յĸ���
		
		/* 3.�������ݽ����������� */
//		memcpy(DMA_Tx_Buf, DMA_Rece_Buf, Uart4_Rec_Cnt); 	//������ת��ͨ������4��DMA��ʽ���ͳ�ȥ����
//		UART4_TX_DMA_Enable(); 	//����һ��DMA���ͣ�ʵ��ת��

		
		
		memcpy(BoradSlaveBuf, DMA_Rece_Buf, Uart4_Rec_Cnt);   //������5 DMA���յ�����ת����BoradSlaveBuf
		
		
		/* 4.�����µ�һ��DMA���� */
		UART4_RX_DMA_Enable(); 	//����ʹ��DMA���ȴ���һ�εĽ���
     } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
	





void BoradSlaveTask(void)
{
	if(HMITtoBSTGetSensorStateFlag ==1)  //HMI����������־
	{
		if(FrameCheckIfSendAck(UART4, (u8 *)BoradSlaveBuf)==0)  //У��ϸ�
		{
			if(BoradSlaveBuf[6]==0x71)   //���ݰ�������Ϊ0x71
			{
				BoradSlaveBufDeal();   //���ദ������
				BSTtoHMITGetSensorStateSuccessFlag = 1;  //���ദ����ɱ�־
			}
		}
	}
}




void BoradSlaveBufDeal(void)
{
	RLED1State=BoradSlaveBuf[7];
	RLED3State=BoradSlaveBuf[8];
	RLED5State=BoradSlaveBuf[9];
	
	GLED1State=BoradSlaveBuf[10];
	GLED3State=BoradSlaveBuf[11];
	GLED5State=BoradSlaveBuf[12];
	
	Cup1State=BoradSlaveBuf[13];
	Cup3State=BoradSlaveBuf[14];
	Cup5State=BoradSlaveBuf[15];
	
	Water1SensorState=BoradSlaveBuf[16];
	Water3SensorState=BoradSlaveBuf[17];
	Water5SensorState=BoradSlaveBuf[18];
	
	PUMP1State=BoradSlaveBuf[19];
	PUMP3State=BoradSlaveBuf[20];
	PUMP5State=BoradSlaveBuf[21];
	
	VALVE1State=BoradSlaveBuf[22];
	VALVE3State=BoradSlaveBuf[23];
	VALVE5State=BoradSlaveBuf[24];
	
	WaterSensorPowerState=BoradSlaveBuf[25];
	
	LockState=BoradSlaveBuf[26];
	
	SetUploadTimeInterval=BoradSlaveBuf[27];
	
	ReportRemainLimits|=BoradSlaveBuf[28]<<8;
	ReportRemainLimits|=BoradSlaveBuf[29];
}






//---------------------����˵��--------------------------------------------//
//�������ܣ�	���ôӿذ�����ƴ���
//��ڲ�����	*LimitNum�����õ�����ֵ����Դ��ʼ��ַ���̶�2byte��
//����ֵ��	��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/25
//�޸����ݣ��޸ĺ���ԭ�ͣ�����Ҫ����ָ���ִ��״̬
//--------------------------------
void ConfigLimit(u8 *LimitNum)
{
	u8 datalen = 0x06;//����ָ�������������
	u8 Data_H =*LimitNum;
	u8 Data_L =*(LimitNum+1);
	u8 i=0;	//���ݴ��±�
	u8 SendStr[DMA_Tx_Len] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										0x20,0x21,//���ͺͽ��շ������豸��ʶ����Ϊ0x02
										0x00,//�Ƿ���ҪӦ��
										0x5C,//�����룬����Э���ĵ��涨
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

	USART_SendLenData(UART4,SendStr,datalen+6);//�������ݰ�				
}
//Function OK 


//---------------------����˵��--------------------------------------------//
//�������ܣ�	���ôӿذ������ϴ����ݵ�����
//��ڲ�����	Period:ʱ�����ȼ�������С����ı�����0��ʾֹͣ�����ϴ�					
//����ֵ��	��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/23
//�޸����ݣ������µ�Э�飬�޸Ĳ������ͺ͹�����
//--------------------------------
void ConfigUploadtime(u8 Period)
{
	u8 datalen = 0x06;//����ָ�������������
	u8 i=0;	//���ݴ��±�
	u8 SendStr[DMA_Tx_Len] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										0x20,0x21,//���ͺͽ��շ�ʶ����
										0x00,//�Ƿ���ҪӦ��
										0x60,//�����룬����Э���ĵ��涨
										0x00,Period,//�ϴ�ʱ����
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

	USART_SendLenData(UART4,SendStr,datalen+6);//�������ݰ�
}




//---------------------����˵��--------------------------------------------//
//�������ܣ�	��ѯ�ӿذ�Ĺ���״̬
//��ڲ�����	��
//����ֵ��	��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/23
//�޸����ݣ�����ǰ�İ汾�ϣ������µ�Э��Ҫ���޸��˺���ԭ���Լ������루��ģ�
//--------------------------------
void QueryWorkState(void)
{
	u8 datalen = 0x06;//����ָ�������������
	u8 i=0;	//���ݴ��±�
	u8 SendStr[DMA_Tx_Len] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										0x20,0x21,
										0x00,//�Ƿ���ҪӦ��
										0x5E,//�����룬����Э���ĵ��涨
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
	USART_SendLenData(UART4,SendStr,datalen+6);//�������ݰ�
}



//---------------------����˵��--------------------------------------------//
//�������ܣ�	���ذ���ӿذ巢�Ϳ�����Ϣ
//��ڲ�����	CUPx��������һ������
//						CupDisEnable�����Ʊ��׵�ʹ�ܣ�0x00ʧ�ܣ�0x01ʹ�ܣ�0x02ά��ԭ״̬
//						PumpDisEnable�����Ʊ�����Ӧ�ıõ�ʹ�ܣ�0x00������0x01�رգ�0x02ά��ԭ״̬
//����ֵ��		��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/25
//�޸����ݣ�������ɻ�������
//--------------------------------
void ControlSlave(u8 CUPx,u8 CupDisEnable,u8 PumpDisEnable)
{
	u8 i=0;	//���ݴ��±�
	u8 datalen = 0x1E;//����ָ�������������
	u8 Parity = datalen;
	u8 SendStr[DMA_Tx_Len] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										0x1E,//���ݶγ���
										0x20,0x21,
										0x00,//�Ƿ���ҪӦ��
										0x5B,//�����룬����Э���ĵ��涨
										0x02,0x02,0x02,0x02,0x02,0x02, 0x02,0x02,0x02,0x02,0x02,0x02,//���ܿصĶ���6��LED��6��������״̬
										0x02,0x02,0x02,//�����ö�Ӧ�ļ̵���
										0x02,0x02,0x02,//��������Ӧ�ļ̵���
										0x02,0x02,0x02,0x02,0x02,//�����ɿض�����ʱû��ʹ��
										0x02,0x02,0x02//�������ӵ�ʹ�ܿ���
										};
				
		SendStr[18+CUPx] = PumpDisEnable;	
		SendStr[28+CUPx] = CupDisEnable;
		//����д״̬�������
								
	for(i=3;i<32;i++)//����У������//3+ 26+4 = 33
		Parity   =Parity^SendStr[i];//��λ�������
										
	SendStr[i]=Parity;i++;//���У������
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//������ݰ�β��
//����֡������

	USART_SendLenData(UART4,SendStr,datalen+6);//�������ݰ�//������Ҫ��
}
	

