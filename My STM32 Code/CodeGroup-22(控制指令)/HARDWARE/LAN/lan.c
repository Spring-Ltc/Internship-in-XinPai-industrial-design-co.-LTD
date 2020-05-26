
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


u8 UART5_RX_BUF[30];   //串口5接收缓存
u16 UART5_RX_STA = 0;               //		最高位为存储使能标志位，为0时允许把收到的数据存到数组，为1不存储

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
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD, ENABLE); //使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);//使能UART5时钟
	
	//UART5_TX   GPIOC.12
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC.12
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.12
   
  //UART5_RX	  GPIOD.2初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PD.2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOD.2

   //UART5 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(UART5, &USART_InitStructure); //初始化串口5
	
  USART_Cmd(UART5, ENABLE);  //使能串口5
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}





void UART5_IRQHandler(void)                	//串口5中断服务程序
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
			Res =USART_ReceiveData(UART5);	//读取接收到的数据
			
			if((UART5_RX_STA&0x8000)==0)//接收未完成
				{
				if(UART5_RX_STA&0x4000)//接收到了0x0d
					{
						if(Res!=0x0a)UART5_RX_STA=0;//接收错误,重新开始
						else 
							UART5_RX_STA|=0x8000;	//接收完成了 
					}
				else //还没收到0X0D
					{	
						if(Res==0x0d)UART5_RX_STA|=0x4000;
						else
							{
								UART5_RX_BUF[UART5_RX_STA&0X3FFF]=Res ;
								UART5_RX_STA++;
								if(UART5_RX_STA>(UART5_REC_LEN-1))UART5_RX_STA=0;//接收数据错误,重新开始接收	  
							}		 
					}
				}   		 
		} 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
	} 


void UserServerTask(void)
{
	int i, j;
	int m;
	
	//向用户服务器发送当前滤杯序列号的数据包
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
		for(j=0;j<17;j++)  //17位序列号
			LANtoUSTBuf[j+7] = BarCodeBuf[j];
		DataPacking(0x12, 0x20, 0x01, 0x01, 0x59, (u8 *)BarCodeBuf, (u8 *)LANtoUSTBufPacked);
		USART_SendString(UART5, LANtoUSTBufPacked);
		USTtoHMITTranspondBarCodeSuccessFlag = 1;
	}
	
	//显示屏账户登录
	if(HMITtoUSTTranspondAccountDataFlag == 1)
	{
		u8 PacketLength = 0;
		HMITtoUSTTranspondAccountDataFlag = 0;  //置零请求标志
		
		if(FunctionCode == 0x52) //管理员增加操作员
			PacketLength = DataPacking(0x04, 0x20, 0x01, 0x00, FunctionCode, (u8 *)UserIDBuf, (u8 *)DataPacket);  //数据打包成一个数据包并将数据包长度存入PacketLength
		else if(FunctionCode == 0x53) //管理员删除操作员
			PacketLength = DataPacking(0x04, 0x20, 0x01, 0x00, FunctionCode, (u8 *)UserIDBuf, (u8 *)DataPacket);  //数据打包成一个数据包并将数据包长度存入PacketLength
		else if(FunctionCode == 0x54) //操作员登陆
			PacketLength = DataPacking(0x10, 0x20, 0x01, 0x00, FunctionCode, (u8 *)EquipmentNoAndPasswordBuf, (u8 *)DataPacket);  //数据打包成一个数据包并将数据包长度存入PacketLength
		else if(FunctionCode == 0x55) //操作员登陆
			PacketLength = DataPacking(0x08, 0x20, 0x01, 0x00, FunctionCode, (u8 *)UserIDAndPasswordBuf, (u8 *)DataPacket);  //数据打包成一个数据包并将数据包长度存入PacketLength
		else if(FunctionCode == 0x56)  //管理员修改登录设备密码
			PacketLength = DataPacking(0x04, 0x20, 0x01, 0x00, FunctionCode, (u8 *)UserChangePasswordBuf, (u8 *)DataPacket);  //数据打包成一个数据包并将数据包长度存入PacketLength
		else if(FunctionCode == 0x57)  //操作员修改登录设备密码
			PacketLength = DataPacking(0x08, 0x20, 0x01, 0x00, FunctionCode, (u8 *)UserIDAndPasswordBuf, (u8 *)DataPacket);  //数据打包成一个数据包并将数据包长度存入PacketLength
		
		USART_SendLenData(UART5, (u8 *)DataPacket, PacketLength);    //串口5发送数据包
		FunctionCode = 0x00;  //置零功能码
		USTtoHMITTranspondAccountDataSuccessFlag = 1;  //发送成功
	}
	
	//生产商要求转发数据
	if(MSTtoUSTTranspondDataFlag == 1)
	{
		if(OnceSend == 0)
		{
			USART_SendLenData(UART5, (u8 *)TranspondDataBuf, sizeof(TranspondDataBuf));    //串口5发送数据包
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
			ReadLenByte(DataAddress0, SaveTest, sizeof(TranspondDataBuf));  //读取存储器测试
			GPIO_ResetBits(GPIOC, GPIO_Pin_1);
			delay_ms(50);
			GPIO_SetBits(GPIOC, GPIO_Pin_1);
			delay_ms(50);
			GPIO_ResetBits(GPIOC, GPIO_Pin_1);
			delay_ms(50);
			GPIO_SetBits(GPIOC, GPIO_Pin_1);
		}
			
	}
	
	//接收用户服务器返回的序列号比对数据包
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
		ReadLenByte(DataAddress0, SaveTest2, sizeof(TranspondDataBuf));  //读取存储器测试
		USART_SendLenData(UART5, (u8 *)SaveTest2, sizeof(TranspondDataBuf));    //串口5发送数据包
	}
}





