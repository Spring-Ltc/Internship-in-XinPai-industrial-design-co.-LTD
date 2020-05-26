

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
		
//		if((UART5_RX_STA&0x8000)==0)//������ղ��洢������
//		{
			UART5_RX_BUF[UART5_RX_STA&0X3FFF]=Res ;
			UART5_RX_STA++;	
//			if((UART5_RX_STA  & 0X3FFF) > MaxRx5Len-1)//�쳣����̫�����ݣ����¿�ʼ����
//				UART5_RX_STA = 0;
//		 }		 	 
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
	while(--time)//��ʱ�ж�
	{
		delay_ms(5);//���ʱ��Ӧ�ý�����һ���ַ��˰ɣ�
		if(UART5_RX_BUF[0]=='a' && UART5_RX_STA==1)//ֻ���յ�һ���ַ�Ϊ��a��
			break;
	}
	if(time==0)	 return 1;//����ָ��ʱ��û���յ��ظ�

	ClearArray(UART5_RX_BUF,MaxRx5Len);//�����
	UART5_RX_STA=0;//�������
	USART_SendString(UART5,"a");
	while(--time)//��ʱ�ж�
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
//�������ܣ�	���иú�������̫��ģ���˳�ATģʽ
//��ڲ�������
//����ֵ��	�Ƿ��˳�ATģʽ�ɹ���0��ʾ�ɹ���������ʾʧ�ܣ�������Ӧ��������

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 ExitAT(void )
{
	char AckBuff[15]="0";
	u8 status=0;
	ClearArray(AckBuff,15);
	status = ATControl("AT+ENTM\r\n",AckBuff);
	if(status) return 6;//��������6��û���յ�Ӧ��

	if((AckBuff[2]=='+')&&
		(AckBuff[3]=='O')&&
		(AckBuff[4]=='K'))		return 0;//�ɹ��˳�ATģʽ

	else return AckBuff[8];//�����ֲ��϶���Ĵ�������
}

//---------------------����˵��--------------------------------------------//
//�������ܣ�	������̫��ģ��
//��ڲ�������
//����ֵ��	�Ƿ������ɹ���0��ʾ�ɹ���������ʾʧ�ܣ�������Ӧ��������

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 Reboot(void)
{
	char AckBuff[15]="0";
	u8 status=0;
	status = ATControl("AT+Z\r\n",AckBuff);
	if(status) return 6;//��������6��û���յ�Ӧ��

	if((AckBuff[2]=='+')&&
		(AckBuff[3]=='O')&&
		(AckBuff[4]=='K'))		return 0;//�ɹ�

	else return AckBuff[8];//�����ֲ��϶���Ĵ�������
}



//---------------------����˵��--------------------------------------------//
//�������ܣ�	��̫��ģ��ָ���������
//��ڲ�������
//����ֵ��	�Ƿ�ɹ���0��ʾ�ɹ���������ʾʧ�ܣ�������Ӧ��������

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//--------------------------------
u8 ManufacturerReset(void)
{
	char AckBuff[15]="0";
	u8 status=0;
	status = ATControl("AT+RELD\r\n",AckBuff);
	if(status) return 6;//��������6��û���յ�Ӧ��

	if((AckBuff[2]=='+')&&
		(AckBuff[3]=='O')&&
		(AckBuff[4]=='K'))		return 0;//�ɹ�

	else return AckBuff[8];//�����ֲ��϶���Ĵ�������
}




	
//u8 ConfigWEBU(char *UserName,char *Password)
//u8 ConfigDNS(char *Address)
//u8 ConfigWEBPORT(char *Port)
//u8 ConfigTCPSE(char *Status)
//u8 ConfigSOCKPORT(char *Sta)
//u8 ConfigRFCEN(char *Status)
//u8 ConfigREGEN(char *Status)
//u8 ConfigREGTCP(char *Status)
//u8 ConfigREGCLOUD(char *Name,char *Password)
//u8 ConfigREGUSR(char *Data)
//u8 ConfigREGUSRHEX(char *Data)
//u8 ConfigHTPTP(char *Status)
//u8 ConfigHTPURL(char *URL)
//u8 ConfigHTPHEAD(char *Data)
//u8 ConfigHTPEAD(char *Data)
//u8 ConfigHTPCHD(char *Sta)
//u8 ConfigHEARTEN(char *Status)
//u8 ConfigHEARTTP(char *Type)
//u8 ConfigHEARTTM(char *Time)
//u8 ConfigHEARTDT(char *Data)
//u8 ConfigHEARTDTHEX(char *Data)
//u8 ConfigSCSLINK(char *Status)
//u8 ConfigCLIENTRST(char *Sta)
//u8 ConfigINDEXEN(char *Sta)
//u8 ConfigSOCKSL(char *Sta)
//u8 ConfigSHORTO(char *Sta)
//u8 ConfigUARTCLBUF(char *Sta)
//u8 ConfigRSTIM(char *Time)
//u8 ConfigMAXSK(char *Num)
//u8 ConfigMID(char *Name)




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
	u8 time=10;//������ѭ��û�յ�Ӧ��Ҳ�˳�,���ʱ�䲻Ҫ̫С��Ŷ
	u8 i=0;
	ClearArray(UART5_RX_BUF,MaxRx5Len);//����ս�����
	UART5_RX_STA=0;//�������
	USART_SendString(UART5,str);//����ָ����Ϣ
	while(--time)//��ʱ�ж�
	{
		delay_ms(10);//���ʱ��Ӧ�ý�����Ӧ���˰ɣ�
		if(UART5_RX_STA > 4)//���յ�Ӧ����Ϣ����ʼ�жϽ�����־��
		{
			break;
//			if(UART5_RX_BUF[UART5_RX_STA]=='\n' && UART5_RX_BUF[UART5_RX_STA-1]=='\r')//�յ�ĩβ�Ļ��з�
//			{	
//				UART5_RX_STA|= 0x8000;//�����λ��1���ȴ��������ٴ洢������������
//				break;//�ɹ�����Ӧ����Ϣ����ǰ�˳�
//			}
		}
	}
	if(time == 0) return 1;//�涨ʱ��δ�յ�Ӧ��
	
	for(i=0;i<UART5_RX_STA;i++)//�洢Ӧ����Ϣ����
		*(ack+i) = UART5_RX_BUF[i];
	
	return 0;
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
	char command[MaxCommandLen]="AT+";
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
	
	status = ATControl(command,Respond);//����ATָ�������ģ���Ӧ����Ϣ
	if(status) 
		return 1;//û���յ�Ӧ��
	return 0;
}



//---------------------����˵��--------------------------------------------//
//�������ܣ�	����WAN�ڲ���
//��ע��������ڲ����������ַ�������ʽ��Ҫ��'\0'��Ϊ������־��
//��ڲ�����		*Mode:	����IPģʽ��STATIC:��̬IP��DHCP:��̬IP(������������ʡ��)
//						*Address:	��̬IP��ַ������192.168.1.117��
//						*Mask:�������루����255.255.255.0��
//						*Gateway:���أ�����192.168.0.1��
//����ֵ��	�Ƿ����óɹ���0��ʾ���óɹ���������ʾʧ�ܣ�������Ӧ��������

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//----------------------------



u8 ConfigWANN(char *Mode,char *Address,char *Mask,char *Gateway)
{
	char command[MaxCommandLen]="AT+WANN=";
	char AckBuff[15]="0";
	u8 count=0;//ͳ������ָ���ַ���
	u8 i=0;//�������ѭ��
	u8 status = 0;//����ָ������״̬
	
	u8 len=0;//����ָ���ַ���
	len = ATComPacking(command,"WANN",Mode,Address,Mask,Gateway,"#");
	
	status = ATControl(command,AckBuff);//����ATָ�������ģ���Ӧ����Ϣ
	if(status) return 6;//��������6��û���յ�Ӧ��

	if((AckBuff[2]=='+')&&
		(AckBuff[3]=='O')&&
		(AckBuff[4]=='K'))		return 0;//���óɹ�

	else return AckBuff[8];//�����ֲ��϶���Ĵ�������
}
//Function OK


//---------------------����˵��--------------------------------------------//
//�������ܣ�	������̫��ģ��Ĵ��ڲ���
//��ڲ�����	*BaudRate:	�����ʣ�600��460800�Զ��壩
//						*DataBits:	����λ��ֻ������Ϊ7��8��
//						*StopBits:ֹͣλ��ֻ������Ϊ1��2��
//						*Parity:У��λ��NONE:�޼���λ��EVEN:żУ�飻ODD:��У�飻MASK:1У�飻SPACE:0У��
//						*Flowctrl���������ã�NFC:��Ӳ�����أ�FCH����Ӳ�����أ�FCR:������ء�
//											���豸ģ������ֻ������ΪNFC��
//����ֵ��	�Ƿ����óɹ���0��ʾ���óɹ���������ʾʧ�ܣ�������Ӧ��������

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//----------------------------
u8 ConfigUART(char *BaudRate,char *DataBits,char *StopBits,char *Parity,char *Flowctrl)
{
	char command[MaxCommandLen]="AT+UART=";
	char AckBuff[15]="0";
	u8 count=0;//ͳ������ָ���ַ���
	u8 i=0;//�������ѭ��
	u8 status = 0;//����ָ������״̬
	
	u8 len=0;//����ָ���ַ���
	len = ATComPacking(command,"UART",BaudRate,DataBits,StopBits,Parity,Flowctrl,"#");
	
	status = ATControl(command,AckBuff);//����ATָ�������ģ���Ӧ����Ϣ
	if(status) return 6;//��������6��û���յ�Ӧ��

	if((AckBuff[2]=='+')&&
		(AckBuff[3]=='O')&&
		(AckBuff[4]=='K'))		return 0;//���óɹ�

	else return AckBuff[8];//�����ֲ��϶���Ĵ�������
}
//Function OK


//---------------------����˵��--------------------------------------------//
//�������ܣ�	����SOCK����
//��ڲ�����	*Protocol:	ģ��Э������(TCPC��TCPS��UDPC��UDPS��HTPC��ѡһ)
//						*IP:	��ģ�鱻����Ϊ��CLIENT��ʱ��Ŀ��������� IP ��ַ������������192.168.1.117��
//						*Port:ʽ��Ϊ���ض˿ںţ�Client ģʽ��ΪԶ�̶˿ںţ�10 ��������С�� 6535
//����ֵ��	�Ƿ����óɹ���0��ʾ���óɹ���������ʾʧ�ܣ�������Ӧ��������

//--------------------------------
//���ߣ�Liao_tingchun		
//�޸����ڣ�2020/05/08
//�޸����ݣ�������ɻ�������
//----------------------------
u8 ConfigSOCK(char *Protocol,char *IP,char *Port)
{
	char command[MaxCommandLen]={0};//����һ����Ҫ���͵�ATָ���ַ���
	char AckBuff[15]="0";//����ָ���ģ�鷵�ص�����

	u8 status = 0;//����ָ������״̬
	
	u8 len=0;//����ָ���ַ���
	len = ATComPacking(command,"SOCK",Protocol,IP,Port,"#");
	
	status = ATControl(command,AckBuff);//����ATָ�������ģ���Ӧ����Ϣ
	if(status) return 6;//��������6��û���յ�Ӧ��

	if((AckBuff[2]=='+')&&
		(AckBuff[3]=='O')&&
		(AckBuff[4]=='K'))		return 0;//���óɹ�

	else return AckBuff[8];//�����ֲ��϶���Ĵ�������
}
//Function OK


