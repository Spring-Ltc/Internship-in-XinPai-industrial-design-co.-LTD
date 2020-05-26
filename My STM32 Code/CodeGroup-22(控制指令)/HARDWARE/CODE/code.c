#include "delay.h"
#include "user.h"
#include "code.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "string.h"
#include "sys.h"
#include "hmi_handle.h"
#include "slave.h"


char USART3_RX_BUF[18]={0};   //����3���ջ���
u16 USART3_RX_STA = 0;               //		���λΪ�洢ʹ�ܱ�־λ��Ϊ0ʱ������յ������ݴ浽���飬Ϊ1���洢

int BCTtoHMITFlag = 0;

char BarCodeBuf[18];




//----------------------------------����˵��--------------------------------------------
//�������ܣ�	Code�����ų�ʼ������
//��ڲ�����	bound��������
//����ֵ��		��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/09
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void CodeInit(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PC�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //PC.4 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC.4
	GPIO_SetBits(GPIOC,GPIO_Pin_4);						 //PC.4 �����
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);//ʹ��USART3ʱ��
	
	//USART1_TX   GPIOB.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB.10
   
  //USART1_RX	  GPIOB.11��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB.11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB.11

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure); //��ʼ������3
	
  USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���3
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	
  
}




//----------------------------------����˵��--------------------------------------------
//�������ܣ�	����3��Code����Ӧ���жϷ������
//��ڲ�����	��
//����ֵ��		��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/09
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void USART3_IRQHandler(void)                	//����3�жϷ������
	{
	u8 Res;
		int t = 0;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
		
		if((USART3_RX_STA&0x8000)==0)//����δ���
			{
			if(USART3_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART3_RX_STA=0;//���մ���,���¿�ʼ
				else 
					USART3_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else
					{
						USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
						USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
	} 

	
	
//----------------------------------����˵��--------------------------------------------
//�������ܣ�	��ȡ������ĺ�����������3�����ݴ��䵽����������
//��ڲ�����	��
//����ֵ��		��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/13
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void BarCodeTask(void)
{
	int i;
	if(HMITtoBCTFlag == 1)
	{
		if(BoradSlaveBuf[7]==0x01)
		{
//			BoradSlaveBuf[7]=0x00;
			GPIO_ResetBits(GPIOC, GPIO_Pin_4);
			if(USART3_RX_STA!=0)
			{
				for(i=0;i<17+1;i++)
				{
					BarCodeBuf[i]=USART3_RX_BUF[i];
				}
				BarCodeBuf[17]='0';
				
				if(i==17+1)
				{
					memset(USART3_RX_BUF,0,18);
					USART3_RX_STA=0;
					GPIO_SetBits(GPIOC, GPIO_Pin_4);
					BCTtoHMITFlag = 1;
					
					memset(BoradSlaveBuf,0,sizeof (BoradSlaveBuf));
				}
			}
		}
	}
	else if(HMITtoBCTFlag == 0)
//	else
			GPIO_SetBits(GPIOC, GPIO_Pin_4);
}



		








