

#include "Slave.h"
#include "user.h"
#include "delay.h"

//---------------------����˵��--------------------------------------------//
//�������ܣ�	��ʼ���ӻ��õ��Ĵ��ڣ������õĴ���1
//					RX����ΪPA10��TX����ΪPA9
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
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE);	
	
	//USART5_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  //USART5_RX	  GPIOA.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //UART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������5
	
  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���5
}



	//����1�жϷ������
//���մӿذ巵�ص�������ʾ��Ϣ
u8 USART1_RX_BUF[MaxRx1Len]={0};//����5���ջ���
u16 USART1_RX_STA = 0;
//		���λΪ�洢ʹ�ܱ�־λ��Ϊ0ʱ������յ������ݴ浽���飬Ϊ1���洢
void USART1_IRQHandler(void)                
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		if((USART1_RX_STA&0x8000)==0)//������ղ��洢������
		{
			USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
			USART1_RX_STA++;	
			if((USART1_RX_STA  & 0X3FFF) > MaxRx1Len-1)//�쳣����̫�����ݣ����¿�ʼ����
				USART1_RX_STA = 0;
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

	ClearArray(USART1_RX_BUF,MaxRx1Len);//�����
	USART1_RX_STA=0;//�������
	USART_SendLenData(USART1,SendStr,datalen+6);//���ͳ�ȥ
	if(IfAck)//�����ҪӦ���źţ�����Ӧ���źŲ��ж���Ϣ����ȷ��
	{
		while(--time)//��ʱ�ж�
		{
			delay_ms(5);
			if(USART1_RX_STA >= 4)//���յ�Ӧ����Ϣ����4���ֽ�
			{
				if((USART1_RX_BUF[0]==0x21)&&
					(USART1_RX_BUF[1]==0x20)&&
					(USART1_RX_BUF[2]==datalen)&&
					(USART1_RX_BUF[3]==Parity))		return 0;//�յ�Ӧ����֤�ɹ�
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
//����ֵ��		��ѯ�Ľ��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------
u16 QueryLimit(void)
{
	return 0;
}

//---------------------����˵��--------------------------------------------//
//�������ܣ�	���ôӿذ�Ĺ���ģʽ
//						�����ϴ�ģʽ������ѯ��ģʽ
//��ڲ�������
//����ֵ��		��ѯ�Ľ��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 ConfigMode(u8 Mode)
{
	return Mode;
}

