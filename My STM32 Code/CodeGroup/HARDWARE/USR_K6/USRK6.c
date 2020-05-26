

#include "USRK6.h"
#include "usart.h"
#include "delay.h"
#include "user.h"


//---------------------����˵��--------------------------------------------//
//�������ܣ�	��̫��ģ���ʼ���������ʼ������
//					ʹ�ô���5��RX����PD2��TX����PC12
//��ڲ�����	bound:��̫��ģ�����õĴ��ڲ�����
//����ֵ��	��

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/07
//�޸����ݣ�������ɻ�������
//----------------------------
void USRK6Init(u32 bound)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
	
	//UART5_TX   GPIOC.12
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);
   
  //UART5_RX	  GPIOD.2��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOD, &GPIO_InitStructure);  

   //UART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(UART5, &USART_InitStructure); //��ʼ������5
	
  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(UART5, ENABLE);                    //ʹ�ܴ���5
}





	//����5�жϷ������
//������̫��ģ�鷵�ص�������ʾ��Ϣ
char UART5_RX_BUF[MaxRx5Len]={0};//����5���ջ���
u16 UART5_RX_STA = 0;
//		���λΪ�洢ʹ�ܱ�־λ��Ϊ0ʱ������յ������ݴ浽���飬Ϊ1���洢
void UART5_IRQHandler(void)                
{
	u8 Res;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(UART5);	//��ȡ���յ�������
		if((UART5_RX_STA&0x8000)==0)//������ղ��洢������
		{
			UART5_RX_BUF[UART5_RX_STA&0X3FFF]=Res ;
			UART5_RX_STA++;	
			if((UART5_RX_STA  & 0X3FFF) > MaxRx5Len-1)//�쳣����̫�����ݣ����¿�ʼ����
				UART5_RX_STA = 0;
		 }		 	 
	 } 
} 




//---------------------����˵��--------------------------------------------//
//�������ܣ�	���иú�������̫��ģ�����ATģʽ
//��ڲ�������
//����ֵ��	��־ģ���Ƿ����ATģʽ������ɹ�����0��ʧ�ܷ���1

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/07
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 EnterAT(void)
{
	u8 time = 10;//������ѭ��û�յ�Ӧ��Ҳ�˳�
	ClearArray(UART5_RX_BUF,MaxRx5Len);//�����
	UART5_RX_STA=0;//�������
	USART_SendString(UART5,"+++");
	while(time--)//��ʱ�ж�
	{
		delay_ms(5);//���ʱ��Ӧ�ý�����һ���ַ��˰ɣ�
		if(UART5_RX_BUF[0]=='a' && UART5_RX_STA==1)//ֻ���յ�һ���ַ�Ϊ��a��
			break;
	}
	if(time==0)	 return 1;//����ָ��ʱ��û���յ��ظ�

	ClearArray(UART5_RX_BUF,MaxRx5Len);//�����
	UART5_RX_STA=0;//�������
	USART_SendString(UART5,"a");
	while(time--)//��ʱ�ж�
	{
		delay_ms(5);//���ʱ��Ӧ�ý�����һ���ַ��˰ɣ�
		if(UART5_RX_STA >= 2)//���յ���+ok��
		{
			if((UART5_RX_BUF[0]=='+')&&
				(UART5_RX_BUF[1]=='o')&&
				(UART5_RX_BUF[2]=='k'))		return 0;//�ɹ�����ATģʽ
			else break;
		}
	}
	return 1;//����ָ��ʱ��û���յ��ظ�
}



//---------------------����˵��--------------------------------------------//
//�������ܣ�	ͨ������5����̫��ģ��USR-K6����ATָ������ģ�����
//				δ�յ�Ӧ����յ�ģ��ķ�����ʾ��Ϣ������ֵ����������ʾ����ATָ��û�з�������
//��ڲ�����	*str:	���͵���Ϣ�ַ��������ַ
//					*ack:	����������Ϣ��ģ�鷵�ص���ʾ��Ϣ�洢��ַ
//����ֵ��	��־ģ���Ƿ��յ�Ӧ��0��ʾ�յ���1��ʾû���յ�

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/07
//�޸����ݣ�������ɻ�������
//----------------------------

u8 ATControl(char *str,char *ack)
{
	u8 time=5;//������ѭ��û�յ�Ӧ��Ҳ�˳�
	u8 i=0;
	ClearArray(UART5_RX_BUF,MaxRx5Len);//����ս�����
	UART5_RX_STA=0;//�������
	USART_SendString(UART5,str);//����ָ����Ϣ
	while(time--)//��ʱ�ж�
	{
		delay_ms(20);//���ʱ��Ӧ�ý�����Ӧ���˰ɣ�
		if(UART5_RX_STA > 1)//���յ�Ӧ����Ϣ����ʼ�жϽ�����־��
		{
			if(UART5_RX_BUF[UART5_RX_STA]=='\n' && UART5_RX_BUF[UART5_RX_STA-1]=='\r')//�յ�ĩβ�Ļ��з�
			{	
				UART5_RX_STA|= 0x8000;//�����λ��1���ȴ��������ٴ洢������������
				break;//�ɹ�����Ӧ����Ϣ����ǰ�˳�
			}
		}
	}
	if(time == 0) return 1;//�涨ʱ��δ�յ�Ӧ��
	
	for(i=0;i<UART5_RX_STA;i++)//�洢Ӧ����Ϣ����
		*(ack+i) = UART5_RX_BUF[i];
	
	return 0;
}





