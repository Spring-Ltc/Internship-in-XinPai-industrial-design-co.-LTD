#include "gprs.h"
#include "usart.h"
#include "delay.h"
#include "user.h"


char GPRSBackbuf[MaxGPRSLen] = {0};  //GPRS��ATָ������ݴ�����
char USART1_RX_BUF[MaxRx1Len]={0};   //����1���ջ���
u16 USART1_RX_STA = 0;               //		���λΪ�洢ʹ�ܱ�־λ��Ϊ0ʱ������յ������ݴ浽���飬Ϊ1���洢





//----------------------------------����˵��--------------------------------------------
//�������ܣ�	����1��GPRS�������ų�ʼ������
//��ڲ�����	bound��������
//����ֵ��		��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void GPRSInit(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//ʹ��USART1ʱ��
	
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10 

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
}




//----------------------------------����˵��--------------------------------------------
//�������ܣ�	����1��GPRS����Ӧ���жϷ������
//��ڲ�����	��
//����ֵ��		��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
void USART1_IRQHandler(void)                
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		if((USART1_RX_STA&0x8000)==0)//������ղ��洢������
		{
			USART1_RX_BUF[USART1_RX_STA&0X3FFF] = Res ;
			USART1_RX_STA++;	
			if((USART1_RX_STA  & 0X3FFF) > MaxRx1Len - 1)//�쳣����̫�����ݣ����¿�ʼ����
				USART1_RX_STA = 0;
		 }		 	 
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
	ClearArray(USART1_RX_BUF,MaxRx1Len);//�����
	USART1_RX_STA=0;//�������
	USART_SendString(USART1,"+++");
	delay_ms(100);
	if(USART1_RX_BUF[0]=='a' && USART1_RX_STA==1)//���յ���a��
	ClearArray(USART1_RX_BUF,MaxRx1Len);//�����
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
	ClearArray(USART1_RX_BUF,MaxRx1Len);//����ս�����
	USART1_RX_STA=0;//�������
	USART_SendString(USART1,str);//����ָ����Ϣ
	while(time--)//��ʱ�ж�
	{
		delay_ms(20);
		if(USART1_RX_STA > 1)//���յ�Ӧ����Ϣ����ʼ�жϽ�����־��
		{
			if(USART1_RX_BUF[USART1_RX_STA]=='\n' && USART1_RX_BUF[USART1_RX_STA-1]=='\r')//�յ�ĩβ�Ļ��з�
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
	GPRSATControl("AT+HEARTEN=\"on\"\r\n",GPRSBackbuf);
	
	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl("AT+HEARTTM=10\r\n",GPRSBackbuf);
	
	ClearArray(GPRSBackbuf,MaxGPRSLen);
	GPRSATControl("AT+HEARTDT=\"414243\"\r\n",GPRSBackbuf);
	
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





void ManufacturerServerTask(void)
{
	////////
}









