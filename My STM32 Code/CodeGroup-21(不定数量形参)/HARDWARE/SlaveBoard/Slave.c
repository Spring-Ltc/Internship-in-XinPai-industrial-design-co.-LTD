

#include "Slave.h"
#include "user.h"
#include "delay.h"

//---------------------����˵��--------------------------------------------//
//�������ܣ�	��ʼ���ӻ��õ��Ĵ��ڣ������õĴ���4
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
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	
	//UART4_TX   GPIOC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);
   
  //UART4_RX	  GPIOC.11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);  

   //UART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(UART4, &USART_InitStructure); //��ʼ������5
	
  //UART4 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ���5
}



	//����1�жϷ������
//���մӿذ巵�ص�������ʾ��Ϣ
u8 UART4_RX_BUF[MaxRx1Len]={0};//����5���ջ���
u16 UART4_RX_STA = 0;
//		���λΪ�洢ʹ�ܱ�־λ��Ϊ0ʱ������յ������ݴ浽���飬Ϊ1���洢
void UART4_IRQHandler(void)                
{
	u8 Res;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(UART4);	//��ȡ���յ�������
		if((UART4_RX_STA&0x8000)==0)//������ղ��洢������
		{
			UART4_RX_BUF[UART4_RX_STA&0X3FFF]=Res ;
			UART4_RX_STA++;	
			if((UART4_RX_STA  & 0X3FFF) > MaxRx1Len-1)//�쳣����̫�����ݣ����¿�ʼ����
				UART4_RX_STA = 0;
		 }		 	 
	 } 
} 





//---------------------����˵��--------------------------------------------//
//�������ܣ�	���ôӿذ�����ƴ���
//��ڲ�����	LimitNum�����õ�����ֵ
//						IfAck���Ƿ���ҪӦ��(0����Ҫ��1��Ҫ)
//����ֵ��		����ҪӦ��ʱ����ʾ�Ƿ����óɹ���0��ʾ�ɹ���������ʾʧ��
//					����ҪӦ����ֱ�ӷ���0

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/09
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 ConfigLimit(u16 LimitNum,u8 IfAck)
{
	u8 time = 5;//�����ҪӦ�����ó�ʱʱ��
	u8 datalen = 0x06;//����ָ�������������
	u8 Data_H =(u8) (LimitNum >>8);
	u8 Data_L =LimitNum & 0x00FF;
	u8 i=0;	//���ݴ��±�
	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										0x20,0x21,//���ͺͽ��շ������豸��ʶ����Ϊ0x02
										IfAck,//�Ƿ���ҪӦ��
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

	ClearArray(UART4_RX_BUF,MaxRx1Len);//�����
	UART4_RX_STA=0;//�������
	USART_SendLenData(UART4,SendStr,datalen+6);//���ͳ�ȥ
	if(IfAck)//�����ҪӦ���źţ�����Ӧ���źŲ��ж���Ϣ����ȷ��
	{
		while(--time)//��ʱ�ж�
		{
			delay_ms(5);
			if(UART4_RX_STA >= 4)//���յ�Ӧ����Ϣ����4���ֽ�
			{
				if((UART4_RX_BUF[0]==0x21)&&
					(UART4_RX_BUF[1]==0x20)&&
					(UART4_RX_BUF[2]==datalen)&&
					(UART4_RX_BUF[3]==Parity))		return 0;//�յ�Ӧ����֤�ɹ�
				else return 2;//�յ�Ӧ����֤ʧ��
			}
		 }
		if(time == 0)
			return 1;//��ʱδ�յ�Ӧ��
	}
	return 0;		//��ҪҪӦ�𣬷���0						
}


//---------------------����˵��--------------------------------------------//
//�������ܣ�	��ѯ�ӿذ���Ѿ�ʹ�ô���
//��ڲ�������
//����ֵ��		��ѯ�Ľ������󣬷���0xFFFF��ʾ����ָ��ִ�д���

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/10
//�޸����ݣ�������ɻ�������
//--------------------------------
u16 QueryLimit(u8 IfAck)
{
	u8 time = 5;//�����ҪӦ�����ó�ʱʱ��
	u8 datalen = 0x06;//����ָ�������������
	u8 i=0;	//���ݴ��±�
	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										0x20,0x21,//���ͺͽ��շ������豸��ʶ����Ϊ0x02
										IfAck,//�Ƿ���ҪӦ��
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

	ClearArray(UART4_RX_BUF,MaxRx1Len);//�����
	UART4_RX_STA=0;//�������
	USART_SendLenData(UART4,SendStr,datalen+6);//���ͳ�ȥ
	if(IfAck)//�����ҪӦ���źţ�����Ӧ���źŲ��ж���Ϣ����ȷ��
	{
		while(--time)//��ʱ�ж�
		{
			delay_ms(5);
			if(UART4_RX_STA >= 4)//���յ�Ӧ����Ϣ����4���ֽ�
			{
				if((UART4_RX_BUF[0]==0x21)&&
					(UART4_RX_BUF[1]==0x20)&&
					(UART4_RX_BUF[2]==datalen)&&
					(UART4_RX_BUF[3]==Parity))		break;//�յ�Ӧ����֤�ɹ�
				else return 0xFFF;//�յ�Ӧ����֤ʧ��
			}
		 }
		if(time == 0)
			return 0xFFF;//��ʱδ�յ�Ӧ��
	}
	
	while(--time)//��ҪӦ�����ҪӦ�𣬶�Ҫ���նԷ��Ļش����ô���
	{
		delay_ms(5);
		//����������ղ������ӿذ巵���������ݡ�
		
		//����յ���������ȷ��������Ӧ�Ĵ���
	}
	return 0xFFF;		//û���յ��Է����ص�����					
}

//---------------------����˵��--------------------------------------------//
//�������ܣ�	���ôӿذ������ϴ����ݵ�����
//��ڲ�����	Period:ʱ��������λΪms
//						IfAck���Ƿ���ҪӦ��(0����Ҫ��1��Ҫ)								
//����ֵ��		

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/10
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 ConfigUploadtime(u16 Period,u8 IfAck)
{
	u8 Period_s = (u8)(Period/1000);//��sΪ��λ
	u8 Period_ms = (u8)(Period%1000);//��msΪ��λ	//Ltc20200512�޸ģ����������������
	
	u8 time = 5;//�����ҪӦ�����ó�ʱʱ��
	u8 datalen = 0x06;//����ָ�������������
	u8 i=0;	//���ݴ��±�
	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										0x20,0x21,//���ͺͽ��շ�ʶ����
										IfAck,//�Ƿ���ҪӦ��
										0x73,//�����룬����Э���ĵ��涨
										Period_s,Period_ms,//�ϴ�ʱ����
										};
	u8 Parity = datalen;
	for(i=3;i<datalen+3;i++)//����У������
	{
		Parity   =Parity^SendStr[i];//��λ�������
	}
	SendStr[i]=Parity;i++;//���У������
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//������ݰ�β��

	ClearArray(UART4_RX_BUF,MaxRx1Len);//�����
	UART4_RX_STA=0;//�������
	USART_SendLenData(UART4,SendStr,datalen+6);//���ͳ�ȥ
	if(IfAck)//�����ҪӦ���źţ�����Ӧ���źŲ��ж���Ϣ����ȷ��
	{
		while(--time)//��ʱ�ж�
		{
			delay_ms(5);
			if(UART4_RX_STA >= 4)//���յ�Ӧ����Ϣ����4���ֽ�
			{
				if((UART4_RX_BUF[0]==0x21)&&
					(UART4_RX_BUF[1]==0x20)&&
					(UART4_RX_BUF[2]==datalen)&&
					(UART4_RX_BUF[3]==Parity))		return 0;//�յ�Ӧ����֤�ɹ�
				else return 2;//�յ�Ӧ����֤ʧ��
			}
		 }
		if(time == 0)
			return 1;//��ʱδ�յ�Ӧ��
	}
	return 0;		//��ҪҪӦ�𣬷���0						
}




//---------------------����˵��--------------------------------------------//
//�������ܣ�	��ѯ�ӿذ�Ĺ���״̬
//��ڲ�����	IfAck���Ƿ���ҪӦ��(0����Ҫ��1��Ҫ)
//����ֵ��		����ҪӦ��ʱ����ʾ�Է��Ƿ��յ�ָ��ɹ���0��ʾ�ɹ���������ʾʧ��
//					����ҪӦ����ֱ�ӷ���0

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/10
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 QueryWorkState(u8 IfAck)
{
	u8 time = 5;//�����ҪӦ�����ó�ʱʱ��
	u8 datalen = 0x06;//����ָ�������������
	u8 i=0;	//���ݴ��±�
	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										0x20,0x21,//���ͺͽ��շ������豸��ʶ����Ϊ0x02
										IfAck,//�Ƿ���ҪӦ��
										0x72,//�����룬����Э���ĵ��涨
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

	ClearArray(UART4_RX_BUF,MaxRx1Len);//�����
	UART4_RX_STA=0;//�������
	USART_SendLenData(UART4,SendStr,datalen+6);//���ͳ�ȥ
	if(IfAck)//�����ҪӦ���źţ�����Ӧ���źŲ��ж���Ϣ����ȷ��
	{
		while(--time)//��ʱ�ж�
		{
			delay_ms(5);
			if(UART4_RX_STA >= 4)//���յ�Ӧ����Ϣ����4���ֽ�
			{
				if((UART4_RX_BUF[0]==0x21)&&
					(UART4_RX_BUF[1]==0x20)&&
					(UART4_RX_BUF[2]==datalen)&&
					(UART4_RX_BUF[3]==Parity))		break;//�յ�Ӧ����֤�ɹ�
				else return 2;//�յ�Ӧ����֤ʧ��
			}
		 }
		if(time == 0)
			return 1;//��ʱδ�յ�Ӧ��
	}
	return 0;		
}





//---------------------����˵��--------------------------------------------//
//�������ܣ�	���ذ���ӿذ巢�Ϳ�����Ϣ
//��ڲ�����	*State�����Ƶ�״̬�б�
//						IfAck���Ƿ���ҪӦ��(0����Ҫ��1��Ҫ)
//����ֵ��		����ҪӦ��ʱ����ʾ�Է��Ƿ��յ�ָ��ɹ���0��ʾ�ɹ���������ʾʧ��
//						����ҪӦ����ֱ�ӷ���0

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/10
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 ControlSlave(u8 *State,u8 IfAck)
{
	u8 time = 5;//�����ҪӦ�����ó�ʱʱ��
	u8 datalen = 0x1B;//����ָ�������������
	u8 i=0;	//���ݴ��±�
	u8 Parity = datalen;
	u8 SendStr[MaxSendStr] =		//����ֻ���浽У��λ֮ǰ
										{0xAA,0x55,//���ݰ�ͷ
										datalen,//���ݶγ���
										0x20,0x21,//���ͺͽ��շ������豸��ʶ����Ϊ0x02
										IfAck,//�Ƿ���ҪӦ��
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

	ClearArray(UART4_RX_BUF,MaxRx1Len);//�����
	UART4_RX_STA=0;//�������
	USART_SendLenData(UART4,SendStr,datalen+6);//���ͳ�ȥ
	if(IfAck)//�����ҪӦ���źţ�����Ӧ���źŲ��ж���Ϣ����ȷ��
	{
		while(--time)//��ʱ�ж�
		{
			delay_ms(5);
			if(UART4_RX_STA >= 4)//���յ�Ӧ����Ϣ����4���ֽ�
			{
				if((UART4_RX_BUF[0]==0x21)&&
					(UART4_RX_BUF[1]==0x20)&&
					(UART4_RX_BUF[2]==datalen)&&
					(UART4_RX_BUF[3]==Parity))		break;//�յ�Ӧ����֤�ɹ�
				else return 2;//�յ�Ӧ����֤ʧ��
			}
		 }
		if(time == 0)
			return 1;//��ʱδ�յ�Ӧ��
	}
	return 0;		
}
	
	
	



