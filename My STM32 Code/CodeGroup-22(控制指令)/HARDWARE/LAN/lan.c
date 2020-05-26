
#include "lan.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "string.h"
#include "sys.h"
#include "user.h"
#include "code.h"
#include "hmi_handle.h"
#include "user.h"
#include "gprs.h"
#include "AT24C256.h"


u8 UART5_RX_BUF[30];   //����5���ջ���
u16 UART5_RX_STA = 0;               //		���λΪ�洢ʹ�ܱ�־λ��Ϊ0ʱ������յ������ݴ浽���飬Ϊ1���洢

char LanBuf[18];

u8 SaveTest[256];
u8 SaveTest2[256];

int USTtoHMITTranspondBarCodeSuccessFlag = 0;
int USTtoHMITComparisonBarCodeSuccessFlag = 0;
int USTtoHMITTranspondAccountDataSuccessFlag = 0;
int USTtoMSTTranspondDataSuccessFlag = 0;

int OnceSend = 0;

extern int TimeStart;
extern int TimeOut;
extern int time;

char LANtoUSTBuf[30]={0xAA, 0x55, 0x12, 0x20, 0x01, 0x01, 0x59, 
											0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
											0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
											0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 
											0x11, 0x55, 0xAA};
u8 USTtoLANBuf[28];
char LANtoUSTBufPacked[30];
char DataPacket[20];
extern char BarCodeBuf[18];


void LanInit(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD, ENABLE); //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);//ʹ��UART5ʱ��
	
	//UART5_TX   GPIOC.12
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC.12
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.12
   
  //UART5_RX	  GPIOD.2��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PD.2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOD.2

   //UART5 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART5, &USART_InitStructure); //��ʼ������5
	
  USART_Cmd(UART5, ENABLE);  //ʹ�ܴ���5
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}





void UART5_IRQHandler(void)                	//����5�жϷ������
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
			Res =USART_ReceiveData(UART5);	//��ȡ���յ�������
			
			if((UART5_RX_STA&0x8000)==0)//����δ���
				{
				if(UART5_RX_STA&0x4000)//���յ���0x0d
					{
						if(Res!=0x0a)UART5_RX_STA=0;//���մ���,���¿�ʼ
						else 
							UART5_RX_STA|=0x8000;	//��������� 
					}
				else //��û�յ�0X0D
					{	
						if(Res==0x0d)UART5_RX_STA|=0x4000;
						else
							{
								UART5_RX_BUF[UART5_RX_STA&0X3FFF]=Res ;
								UART5_RX_STA++;
								if(UART5_RX_STA>(UART5_REC_LEN-1))UART5_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
							}		 
					}
				}   		 
		} 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
	} 


void UserServerTask(void)
{
	int i, j;
	int m;
	
	//���û����������͵�ǰ�˱����кŵ����ݰ�
	if(HMITtoUSTTranspondBarCodeFlag == 1)
	{
		HMITtoUSTTranspondBarCodeFlag = 0;
		
		if(UART5_RX_STA!=0)
			{
				for(i=0;i<18+1;i++)
				{
					LanBuf[i]=UART5_RX_BUF[i];
				}
				if(i==18+1)
				{
					memset(UART5_RX_BUF,0,18);
					UART5_RX_STA=0;
				}
			}
		for(j=0;j<17;j++)  //17λ���к�
			LANtoUSTBuf[j+7] = BarCodeBuf[j];
		DataPacking(0x12, 0x20, 0x01, 0x01, 0x59, (u8 *)BarCodeBuf, (u8 *)LANtoUSTBufPacked);
		USART_SendString(UART5, LANtoUSTBufPacked);
		USTtoHMITTranspondBarCodeSuccessFlag = 1;
	}
	
	//��ʾ���˻���¼
	if(HMITtoUSTTranspondAccountDataFlag == 1)
	{
		u8 PacketLength = 0;
		HMITtoUSTTranspondAccountDataFlag = 0;  //���������־
		
		if(FunctionCode == 0x52) //����Ա���Ӳ���Ա
			PacketLength = DataPacking(0x04, 0x20, 0x01, 0x00, FunctionCode, (u8 *)UserIDBuf, (u8 *)DataPacket);  //���ݴ����һ�����ݰ��������ݰ����ȴ���PacketLength
		else if(FunctionCode == 0x53) //����Աɾ������Ա
			PacketLength = DataPacking(0x04, 0x20, 0x01, 0x00, FunctionCode, (u8 *)UserIDBuf, (u8 *)DataPacket);  //���ݴ����һ�����ݰ��������ݰ����ȴ���PacketLength
		else if(FunctionCode == 0x54) //����Ա��½
			PacketLength = DataPacking(0x10, 0x20, 0x01, 0x00, FunctionCode, (u8 *)EquipmentNoAndPasswordBuf, (u8 *)DataPacket);  //���ݴ����һ�����ݰ��������ݰ����ȴ���PacketLength
		else if(FunctionCode == 0x55) //����Ա��½
			PacketLength = DataPacking(0x08, 0x20, 0x01, 0x00, FunctionCode, (u8 *)UserIDAndPasswordBuf, (u8 *)DataPacket);  //���ݴ����һ�����ݰ��������ݰ����ȴ���PacketLength
		else if(FunctionCode == 0x56)  //����Ա�޸ĵ�¼�豸����
			PacketLength = DataPacking(0x04, 0x20, 0x01, 0x00, FunctionCode, (u8 *)UserChangePasswordBuf, (u8 *)DataPacket);  //���ݴ����һ�����ݰ��������ݰ����ȴ���PacketLength
		else if(FunctionCode == 0x57)  //����Ա�޸ĵ�¼�豸����
			PacketLength = DataPacking(0x08, 0x20, 0x01, 0x00, FunctionCode, (u8 *)UserIDAndPasswordBuf, (u8 *)DataPacket);  //���ݴ����һ�����ݰ��������ݰ����ȴ���PacketLength
		
		USART_SendLenData(UART5, (u8 *)DataPacket, PacketLength);    //����5�������ݰ�
		FunctionCode = 0x00;  //���㹦����
		USTtoHMITTranspondAccountDataSuccessFlag = 1;  //���ͳɹ�
	}
	
	//������Ҫ��ת������
	if(MSTtoUSTTranspondDataFlag == 1)
	{
		if(OnceSend == 0)
		{
			USART_SendLenData(UART5, (u8 *)TranspondDataBuf, sizeof(TranspondDataBuf));    //����5�������ݰ�
			OnceSend++;
		}
		
		TimeStart = 1; 
		if(UART5_RX_BUF[0]==0x01&&TimeOut == 0)
		{
			MSTtoUSTTranspondDataFlag = 0;
			TimeStart = 0;
			TimeOut = 0;
			time = 0;
			OnceSend = 0;
			memset(UART5_RX_BUF,0, sizeof(UART5_RX_BUF));
			UART5_RX_STA = 0;
			USTtoMSTTranspondDataSuccessFlag = 1;
			
			GPIO_ResetBits(GPIOC, GPIO_Pin_1);
			delay_ms(50);
			GPIO_SetBits(GPIOC, GPIO_Pin_1);
		}
			
		else if(TimeOut == 1)
		{
			MSTtoUSTTranspondDataFlag = 0;
			TimeStart = 0;
			TimeOut = 0;
			time = 0;
			OnceSend = 0;
			memset(UART5_RX_BUF,0, sizeof(UART5_RX_BUF));
			UART5_RX_STA = 0;
			WriteLenByte(DataAddress0, TranspondDataBuf, sizeof(TranspondDataBuf));
			ReadLenByte(DataAddress0, SaveTest, sizeof(TranspondDataBuf));  //��ȡ�洢������
			GPIO_ResetBits(GPIOC, GPIO_Pin_1);
			delay_ms(50);
			GPIO_SetBits(GPIOC, GPIO_Pin_1);
			delay_ms(50);
			GPIO_ResetBits(GPIOC, GPIO_Pin_1);
			delay_ms(50);
			GPIO_SetBits(GPIOC, GPIO_Pin_1);
		}
			
	}
	
	//�����û����������ص����кűȶ����ݰ�
//	if(UART5_RX_STA!=0)
//			{
//				for(m=0;m<50;m++)
//				{
//					USTtoLANBuf[m]=UART5_RX_BUF[m];
//				}
//				if(m==50+1)
//				{
//					memset(UART5_RX_BUF, 0, sizeof(UART5_RX_BUF));
//					UART5_RX_STA=0;
//				}
//				if(USTtoLANBuf[6] == 0xA0||FrameCheckIfSendAck(UART5, (u8 *)USTtoLANBuf)==0)
//				USTtoHMITComparisonBarCodeSuccessFlag = 0;
//				if(FrameCheckIfSendAck(UART5, (u8 *)USTtoLANBuf)==0)
//					GPIO_ResetBits(GPIOC, GPIO_Pin_1);
//			}
			
	if((UART5_RX_BUF[1]==0x55)&&(UART5_RX_BUF[0]==0xAA)&&(UART5_RX_BUF[6]==0xA1))
	{
		memset(UART5_RX_BUF, 0, sizeof(UART5_RX_BUF));
		UART5_RX_STA=0;
		ReadLenByte(DataAddress0, SaveTest2, sizeof(TranspondDataBuf));  //��ȡ�洢������
		USART_SendLenData(UART5, (u8 *)SaveTest2, sizeof(TranspondDataBuf));    //����5�������ݰ�
	}
}





