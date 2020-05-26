#include "gprs.h"
#include "usart.h"
#include "delay.h"
#include "user.h"
#include "string.h"
#include "hmi_handle.h"
#include "lan.h"
#include "AT24C256.h"
#include "timer.h"
#include "led.h"


char GPRSBackbuf[MaxGPRSLen] = {0};  //GPRS��ATָ������ݴ�����
u8 USART1_RX_BUF[MaxRx1Len]={0};   //����1���ջ���
u16 USART1_RX_STA = 0;               //		���λΪ�洢ʹ�ܱ�־λ��Ϊ0ʱ������յ������ݴ浽���飬Ϊ1���洢

int MSTtoHMITGetGprsStateSuccessFlag = 0;
int MSTtoUSTTranspondDataFlag = 0;
int i;
int OnceSendGPRS = 0;

extern int TimeStart;
extern int TimeOut;
extern int time;

extern u8 ReceiveBuf[20];

char GPRSStateBuf[16];
u8 TranspondDataBuf[240];
u16 AckPackedLenth;





u8  DMA_Rece_Buf_USART1[DMA_Rec_Len_USART1];	   //DMA���մ������ݻ�����
u16  Usart1_Rec_Cnt=0;             //��֡���ݳ���	
u8  DMA_Tx_Buf_USART1[DMA_Tx_Len_USART1];	   //DMA���մ������ݻ�����
u16  Usart1_Tx_Cnt=0;             //��֡���ݳ���	




////----------------------------------����˵��--------------------------------------------
////�������ܣ�	����1��GPRS�������ų�ʼ������
////��ڲ�����	bound��������
////����ֵ��		��
////--------------------------------------------------------------------------------------
////���ߣ�Zhou_Taoyuan	
////�޸����ڣ�2020/05/08
////�޸����ݣ�������ɻ�������
////--------------------------------------------------------------------------------------
//void GPRSInit(u32 bound)
//{
//  //GPIO�˿�����
//  GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��GPIOAʱ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//ʹ��USART1ʱ��
//	
//	//USART1_TX   GPIOA.9
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
//   
//  //USART1_RX	  GPIOA.10��ʼ��
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10 

//   //USART1 ��ʼ������
//	USART_InitStructure.USART_BaudRate = bound;//����������
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
//	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
//  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
//	
//  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�
////	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//��������ж�

//	//Usart1 NVIC ����
//  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
//}




////----------------------------------����˵��--------------------------------------------
////�������ܣ�	����1��GPRS����Ӧ���жϷ������
////��ڲ�����	��
////����ֵ��		��
////--------------------------------------------------------------------------------------
////���ߣ�Zhou_Taoyuan	
////�޸����ڣ�2020/05/08
////�޸����ݣ�������ɻ�������
////--------------------------------------------------------------------------------------
//void USART1_IRQHandler(void)                
//{
//	u8 Res;
//	static u8 USART1FreeFlag = 0;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
//	{
//		if(USART1FreeFlag == 1)
//		{
//			memset(USART1_RX_BUF, 0, sizeof(USART1_RX_BUF));
//			USART1_RX_STA=0;
//			USART1FreeFlag = 0;
//		}
//		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
//		if((USART1_RX_STA&0x8000)==0)//������ղ��洢������
//		{
//			USART1_RX_BUF[USART1_RX_STA&0X3FFF] = Res ;
//			USART1_RX_STA++;	
//			if((USART1_RX_STA  & 0X3FFF) > MaxRx1Len - 1)//�쳣����̫�����ݣ����¿�ʼ����
//				USART1_RX_STA = 0;
//		 }
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//		
//	 } 
////	else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) //�����ж�
////		USART1FreeFlag = 1;
//} 






//----------------------------------����˵��--------------------------------------------
//�������ܣ�  ����4��DMA��ʼ��
//��ڲ�����	��
//����ֵ��	  ��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void USART1_DMA_Init(void)
{
	//UART4_RX��DMA_UART4_TX��DMA��ʼ��
	DMA_InitTypeDef DMA_USART1_RX;
//	DMA_InitTypeDef DMA_USART1_TX;
	
	//RX��Ӧ��DMA����
	DMA_DeInit(DMA1_Channel5);   //��DMA��ͨ��5�Ĵ�������Ϊȱʡֵ  ����4��Ӧ����DMAͨ��13
	DMA_USART1_RX.DMA_PeripheralBaseAddr = (u32)&USART1->DR;  //DMA����ADC����ַ
	DMA_USART1_RX.DMA_MemoryBaseAddr = (u32)DMA_Rece_Buf_USART1;  //DMA�ڴ����ַ
	DMA_USART1_RX.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴������ȡ���͵��ڴ�
	DMA_USART1_RX.DMA_BufferSize = DMA_Rec_Len_USART1;  //DMAͨ����DMA����Ĵ�С
	DMA_USART1_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_USART1_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_USART1_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_USART1_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_USART1_RX.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_USART1_RX.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
	DMA_USART1_RX.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel5, &DMA_USART1_RX);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��UART4_Tx_DMA_Channe13����ʶ�ļĴ���
		
	DMA_Cmd(DMA1_Channel5, ENABLE);  //��ʽ����DMA����
	
	//TX��Ӧ��DMA����
//	DMA_DeInit(DMA1_Channel4);   //��DMA��ͨ��5�Ĵ�������Ϊȱʡֵ  ����4��Ӧ����DMAͨ��15
//	DMA_USART1_TX.DMA_PeripheralBaseAddr = (u32)&USART1->DR;  //DMA����ADC����ַ
//	DMA_USART1_TX.DMA_MemoryBaseAddr = (u32)DMA_Tx_Buf_USART1;  //DMA�ڴ����ַ
//	DMA_USART1_TX.DMA_DIR = DMA_DIR_PeripheralDST;  //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
//	DMA_USART1_TX.DMA_BufferSize = DMA_Tx_Len;  //DMAͨ����DMA����Ĵ�С
//	DMA_USART1_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
//	DMA_USART1_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
//	DMA_USART1_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
//	DMA_USART1_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
//	DMA_USART1_TX.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
//	DMA_USART1_TX.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
//	DMA_USART1_TX.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
//	DMA_Init(DMA1_Channel4, &DMA_USART1_TX);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��UART4_Tx_DMA_Channe15����ʶ�ļĴ���
		
//	DMA_Cmd(DMA1_Channel4, ENABLE);  //��ʽ����DMA����
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
void USART1_RX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA1_Channel5, DISABLE ); //��ֹͣDMA����ͣ���� 
 	DMA_SetCurrDataCounter(DMA1_Channel5, DMA_Rec_Len_USART1); //DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA1_Channel5, ENABLE); //ʹ��UART4 RX DMA1 ��ָʾ��ͨ�� 
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
//void USART1_TX_DMA_Enable(void)
//{ 
//	DMA_Cmd(DMA1_Channel4, DISABLE ); //��ֹͣDMA
// 	DMA_SetCurrDataCounter(DMA1_Channel4, DMA_Tx_Len); //DMAͨ����DMA����Ĵ�С
// 	DMA_Cmd(DMA1_Channel4, ENABLE); //ʹ��UART4 TX DMA1 ��ָʾ��ͨ�� 
//}	





//----------------------------------����˵��--------------------------------------------
//�������ܣ�  ���ӿذ�ͨ�Ŷ�Ӧ�Ĵ���4��ʼ��
//��ڲ�����	bound��������
//����ֵ��	  ��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void GPRSInit(u32 bound)
	{
	GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//ʹ��UART4ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
	
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound; //���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No; //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������4
	
	//UART4_TX   GPIOC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOC.10
   
  //UART4_RX	  GPIOC.11��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PC.11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOC.11
	
	//Uart4 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); //���������ж�
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); //ʹ�ܴ���4��DMA����
//	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); //ʹ�ܴ���4��DMA����
	USART_Cmd(USART1, ENABLE); //ʹ�ܴ���4 
	
	USART1_DMA_Init();  //UART4��DMA���ܳ�ʼ��
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
void USART1_IRQHandler(void)
	{
	//����1�����ж�
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  
	{	
		/* 1.�����־ */
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);	 //����жϱ�־
//		memset(USART1_RX_BUF, 0, sizeof(USART1_RX_BUF));
		
		/* 2.��ȡDMA */
		USART_ReceiveData(USART1); 	//��ȡ����
		Usart1_Rec_Cnt = DMA_Rec_Len_USART1 - DMA_GetCurrDataCounter(DMA1_Channel5); 	//���ո������ڽ��ջ������ܴ�С���Ѿ����յĸ���
		
		memcpy(USART1_RX_BUF, DMA_Rece_Buf_USART1, Usart1_Rec_Cnt);   //������1 DMA���յ�����ת����USART1_RX_BUF
		
		/* 4.�����µ�һ��DMA���� */
		USART1_RX_DMA_Enable(); 	//����ʹ��DMA���ȴ���һ�εĽ���
     } 
} 







//----------------------------------����˵��--------------------------------------------
//�������ܣ�	ͨ������1��GPRS����ATָ���������״̬�Ĺ��ܺ���
//��ڲ�����	��
//����ֵ��		��־ģ���Ƿ����ATģʽ������ɹ�����0��ʧ�ܷ���1
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
u8 GPRSEnterAT(void)
{
	u8 time = 10;//������ѭ��û�յ�Ӧ��Ҳ�˳�
	ClearArray((char *)USART1_RX_BUF,MaxRx1Len);//�����
	USART1_RX_STA=0;//�������
	USART_SendString(USART1,"+++");
	delay_ms(100);
	if(USART1_RX_BUF[0]=='a' && USART1_RX_STA==1)//���յ���a��
	ClearArray((char *)USART1_RX_BUF,MaxRx1Len);//�����
	USART1_RX_STA=0;//�������
	
	USART_SendString(USART1,"a");
	while(time--)//��ʱ�ж�
	{
		delay_ms(50);//���ʱ��Ӧ�ý�����һ���ַ��˰ɣ�
		if(USART1_RX_STA >= 2)//���յ���+ok��
		{
			if((USART1_RX_BUF[0]=='+')&&
				(USART1_RX_BUF[1]=='o')&&
				(USART1_RX_BUF[2]=='k'))		
			return 0;//�ɹ�����ATģʽ
			else break;
		}
	}
	return 1;//����ָ��ʱ��û���յ��ظ�
}



//----------------------------------����˵��--------------------------------------------
//�������ܣ�	ͨ������1��GPRS����ATָ��Ĺ��ܺ���
//��ڲ�����	*str:	���͵���Ϣ�ַ��������ַ
//						*ack:	����ATָ���ģ�鷵�ص���ʾ��Ϣ�洢��ַ
//����ֵ��		��־ģ���Ƿ��յ�Ӧ��0��ʾ�յ���1��ʾû���յ�
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------

u8 GPRSATControl(char *str,char *ack)
{
	u8 time=5;//������ѭ��û�յ�Ӧ��Ҳ�˳�
	u8 i=0;
	ClearArray((char *)USART1_RX_BUF,MaxRx1Len);//����ս�����
	USART1_RX_STA=0;//�������
	USART_SendString(USART1,str);//����ָ����Ϣ
	while(--time)//��ʱ�ж�
	{
		delay_ms(20);
		if(USART1_RX_STA > 1)//���յ�Ӧ����Ϣ����ʼ�жϽ�����־��
		{
			if(USART1_RX_BUF[USART1_RX_STA]=='0' && USART1_RX_BUF[USART1_RX_STA-1]=='\r')//�յ�ĩβ�Ļ��з�
			{	
				USART1_RX_STA|= 0x8000;//�����λ��1���ȴ��������ٴ洢������������
				break;//�ɹ�����Ӧ����Ϣ����ǰ�˳�
			}
		}
	}
	if(time == 0) return 1;//�涨ʱ��δ�յ�Ӧ��
	
	for(i=0;i<USART1_RX_STA;i++)//�洢Ӧ����Ϣ����
		*(ack+i) = USART1_RX_BUF[i];
	
	return 0;
}



//----------------------------------����˵��--------------------------------------------
//�������ܣ�  ����GPRS�Ĳ���
//��ڲ�����	*Protocol: ͨ��Э�飨TCP��
//						*IP�������̷�����IP��ַ
//            *Port�������̷������˿ں�
//����ֵ��	  ��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void GPRSConfig(char *Protocol, char *IP, char *Port)
{
	char IPPortAT[50]="AT+SOCKB=";
	u8 count=0;//ͳ������ָ���ַ���
	int i=0;//�������ѭ��
	while(IPPortAT[count] != '\0')
		count++;
	IPPortAT[count] = '\"';count++;
	for(i=0;*(Protocol+i) != '\0';i++)//���ͨ��Э�����͵�ָ���ַ���
	{
		IPPortAT[count] = Protocol[i];
		count++;
	}
	IPPortAT[count] = '\"';count++;
	IPPortAT[count] = ',';count++;
	IPPortAT[count] = '\"';count++;
	for(i=0;*(IP+i) != '\0';i++)//���IP��ָ���ַ���
	{
		IPPortAT[count] = IP[i];
		count++;
	}
	IPPortAT[count] = '\"';count++;
	IPPortAT[count] = ',';count++;
	for(i=0;*(Port+i) != '\0';i++)//��Ӷ˿ںŵ�ָ���ַ���
	{
		IPPortAT[count] = Port[i];
		count++;
	}
	IPPortAT[count] = '\r';count++;IPPortAT[count] = '\n';//��ӻ��з�

	
	
	GPRSEnterAT();  //����ATָ��
//	delay_ms(20);
//	USART_SendString(USART1,"AT+WKMOD=\"NET\"\r\n");
//	delay_ms(20);
//	USART_SendString(USART1,"AT+SOCKBEN=\"on\"\r\n");
//	delay_ms(20);
//	USART_SendString(USART1,"AT+SOCKAEN=\"off\"\r\n");
//	delay_ms(20);
//	USART_SendString(USART1,IPPortAT);
//	delay_ms(20);
//	USART_SendString(USART1,"AT+SOCKBSL=\"long\"\r\n");
//	delay_ms(20);
//	USART_SendString(USART1,"AT+SOCKRSTIM=60\r\n");
//	delay_ms(20);
//	USART_SendString(USART1,"AT+HEARTEN=\"on\"\r\n");
//	delay_ms(20);
//	USART_SendString(USART1,"AT+HEARTTM=10\r\n");
//	delay_ms(20);
//	USART_SendString(USART1,"AT+HEARTDT=\"414243\"\r\n");
//	delay_ms(20);
//	USART_SendString(USART1,"AT+HEARTTP=\"NET\"\r\n");
//	delay_ms(20);
//	USART_SendString(USART1,"AT+UART=115200,\"NONE\",8,1,\"RS485\"\r\n");
//	delay_ms(20);
//	USART_SendString(USART1,"AT+APN=\"CMNET\",\"\",\"\"\r\n");
//	delay_ms(20);
//	USART_SendString(USART1,"AT+E=\"on\"\r\n");
//	delay_ms(20);
//	USART_SendString(USART1,"AT+S\r\n");
//	delay_ms(20);
	
	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl("AT+WKMOD=\"NET\"\r\n",GPRSBackbuf);

	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl("AT+SOCKBEN=\"on\"\r\n",GPRSBackbuf);
	
	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl("AT+SOCKAEN=\"off\"\r\n",GPRSBackbuf);
		
	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl(IPPortAT,GPRSBackbuf);
	
	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl("AT+SOCKBSL=\"long\"\r\n",GPRSBackbuf);
	
	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl("AT+SOCKRSTIM=60\r\n",GPRSBackbuf);
	
	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl("AT+HEARTEN=\"off\"\r\n",GPRSBackbuf);
	
//	ClearArray(GPRSBackbuf,MaxGPRSLen);
//	GPRSATControl("AT+HEARTTM=10\r\n",GPRSBackbuf);
//	
//	ClearArray(GPRSBackbuf,MaxGPRSLen);
//	GPRSATControl("AT+HEARTDT=\"414243\"\r\n",GPRSBackbuf);
	
	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl("AT+HEARTTP=\"NET\"\r\n",GPRSBackbuf);
	
	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl("AT+UART=115200,\"NONE\",8,1,\"RS485\"\r\n",GPRSBackbuf);
	
	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl("AT+APN=\"CMNET\",\"\",\"\"\r\n",GPRSBackbuf);
	
	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl("AT+E=\"on\"\r\n",GPRSBackbuf);
	
	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl("AT+S\r\n",GPRSBackbuf);
}


//---------------------����˵��--------------------------------------------//
//�������ܣ�	ͨ��ATָ���ѯ������ò���
//��ڲ�����	*Command:	���͵Ĳ�ѯ����
//						*Respond:	ģ�鴫�ص���Ϣ
//����ֵ��		��־ģ���Ƿ��յ�Ӧ��0��ʾ�յ���1��ʾû���յ�

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/07
//�޸����ݣ�������ɻ�������
//----------------------------
u8 ATQuery(char *Command,char *Respond)
{
	char command[MaxGPRSCommandLen]="AT+";
	u8 count=0;//ͳ������ָ���ַ���
	u8 i=0;//�������ѭ��
	u8 status = 0;//����ָ������״̬
	
	while(command[count] != '\0')
		count++;
	for(i=0;*(Command+i) != '\0';i++)//��Ӳ�ѯָ�����ݵ�ָ���ַ���
	{
		command[count] = Command[i];
		count++;
	}
	command[count] = '?';count++;//����ʺ�
	command[count] = '\r';count++;command[count] = '\n';	//��ӻ��з�
	
	status = GPRSATControl(command,Respond);//����ATָ�������ģ���Ӧ����Ϣ
	if(status) 
		return 1;//û���յ�Ӧ��
	return 0;
}





void ManufacturerServerTask(void)
{
	
	if(HMITtoMSTGetGprsStateFlag == 1)
	{
		memset(USART1_RX_BUF,0,15);
		USART_SendString(USART1, "usr.cn#AT+CSQ\r\n");
		delay_ms(100);
		for(i=0;i<15+1;i++)
				{
					GPRSStateBuf[i]=USART1_RX_BUF[i];
				}
				if(i==15+1)
				{
					i = 0;
					memset(USART1_RX_BUF,0,15);
					USART1_RX_STA=0;
					MSTtoHMITGetGprsStateSuccessFlag = 1;
				}
	}
	
	if(HMITtoMSTSetGprsPramFlag==1)
	{
		HMITtoMSTSetGprsPramFlag=0;
	}
	
	//MST�յ�HMIT���͵�����ͬ�����кŵ�����
	if(HMITtoMSTRequestSynSeriesNumFlag==1)
	{
		u8 PacketLength = 0;
		u8 DataPacket[30];
//		u8 ReceiveBuf[20];
//		HMITtoMSTRequestSynSeriesNumFlag=0;
		if(OnceSendGPRS == 0)
		{
			PacketLength = DataPacking(0x0C, 0x20, 0x00, 0x01, FunctionCode, (u8 *)EquipmentNo, (u8 *)DataPacket);  //���ݴ����һ�����ݰ��������ݰ����ȴ���PacketLength
			USART_SendLenData(USART1, (u8 *)DataPacket, PacketLength);    //����1��������ͬ�����кŵ��������ݰ�
			OnceSendGPRS++;
		}
		TimeStart = 1; 
		USARTtoBuf(USART1_RX_BUF, ReceiveBuf, sizeof(ReceiveBuf));
		if(ReceiveBuf[0]==0x01&&TimeOut == 0)
		{
			HMITtoMSTRequestSynSeriesNumFlag=0;
			TimeStart = 0;
			TimeOut = 0;
			time = 0;
			OnceSendGPRS = 0;
			USART1_RX_STA = 0;
		}
		
		OnceSendGPRS = 0;
	}
	
	
	if((USART1_RX_BUF[1]==0x55)&&(USART1_RX_BUF[0]==0xAA)&&(USART1_RX_BUF[6]==0x01)&&USTtoMSTTranspondDataSuccessFlag==0)
	{
		memset(TranspondDataBuf, 0, sizeof(TranspondDataBuf));
		for(i=0;i<sizeof(USART1_RX_BUF);i++)
		{
			TranspondDataBuf[i] = USART1_RX_BUF[i];
		}
		if(i == sizeof(USART1_RX_BUF))
		{
			i = 0;
			memset(USART1_RX_BUF, 0, sizeof(USART1_RX_BUF));
			AckPackedLenth = AckPacking(0x20,(u8 *)TranspondDataBuf);
			USART_SendLenData(USART1,(u8 *)AckPacked, AckPackedLenth);
			if(TranspondDataBuf[6] == 0x01)
			{
				MSTtoUSTTranspondDataFlag = 1;
			}
		}
	}
	
	if(USTtoMSTTranspondDataSuccessFlag == 1)
	{
		USTtoMSTTranspondDataSuccessFlag = 0;
	}
	
	
}


