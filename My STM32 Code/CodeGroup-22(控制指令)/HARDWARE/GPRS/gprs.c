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


char GPRSBackbuf[MaxGPRSLen] = {0};  //GPRS的AT指令返回数据储存区
u8 USART1_RX_BUF[MaxRx1Len]={0};   //串口1接收缓存
u16 USART1_RX_STA = 0;               //		最高位为存储使能标志位，为0时允许把收到的数据存到数组，为1不存储

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





u8  DMA_Rece_Buf_USART1[DMA_Rec_Len_USART1];	   //DMA接收串口数据缓冲区
u16  Usart1_Rec_Cnt=0;             //本帧数据长度	
u8  DMA_Tx_Buf_USART1[DMA_Tx_Len_USART1];	   //DMA接收串口数据缓冲区
u16  Usart1_Tx_Cnt=0;             //本帧数据长度	




////----------------------------------函数说明--------------------------------------------
////函数功能：	串口1（GPRS）的引脚初始化函数
////入口参数：	bound：波特率
////返回值：		无
////--------------------------------------------------------------------------------------
////作者：Zhou_Taoyuan	
////修改日期：2020/05/08
////修改内容：初次完成基本功能
////--------------------------------------------------------------------------------------
//void GPRSInit(u32 bound)
//{
//  //GPIO端口设置
//  GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIOA时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//使能USART1时钟
//	
//	//USART1_TX   GPIOA.9
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
//   
//  //USART1_RX	  GPIOA.10初始化
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10 

//   //USART1 初始化设置
//	USART_InitStructure.USART_BaudRate = bound;//波特率设置
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
//	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
//  USART_Init(USART1, &USART_InitStructure); //初始化串口1
//	
//  USART_Cmd(USART1, ENABLE);  //使能串口1
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
////	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启相关中断

//	//Usart1 NVIC 配置
//  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
//}




////----------------------------------函数说明--------------------------------------------
////函数功能：	串口1（GPRS）对应的中断服务程序
////入口参数：	无
////返回值：		无
////--------------------------------------------------------------------------------------
////作者：Zhou_Taoyuan	
////修改日期：2020/05/08
////修改内容：初次完成基本功能
////--------------------------------------------------------------------------------------
//void USART1_IRQHandler(void)                
//{
//	u8 Res;
//	static u8 USART1FreeFlag = 0;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
//	{
//		if(USART1FreeFlag == 1)
//		{
//			memset(USART1_RX_BUF, 0, sizeof(USART1_RX_BUF));
//			USART1_RX_STA=0;
//			USART1FreeFlag = 0;
//		}
//		Res =USART_ReceiveData(USART1);	//读取接收到的数据
//		if((USART1_RX_STA&0x8000)==0)//允许接收并存储到数组
//		{
//			USART1_RX_BUF[USART1_RX_STA&0X3FFF] = Res ;
//			USART1_RX_STA++;	
//			if((USART1_RX_STA  & 0X3FFF) > MaxRx1Len - 1)//异常接收太多数据，重新开始接收
//				USART1_RX_STA = 0;
//		 }
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//		
//	 } 
////	else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) //空闲中断
////		USART1FreeFlag = 1;
//} 






//----------------------------------函数说明--------------------------------------------
//函数功能：  串口4的DMA初始化
//入口参数：	无
//返回值：	  无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
void USART1_DMA_Init(void)
{
	//UART4_RX和DMA_UART4_TX的DMA初始化
	DMA_InitTypeDef DMA_USART1_RX;
//	DMA_InitTypeDef DMA_USART1_TX;
	
	//RX相应的DMA配置
	DMA_DeInit(DMA1_Channel5);   //将DMA的通道5寄存器重设为缺省值  串口4对应的是DMA通道13
	DMA_USART1_RX.DMA_PeripheralBaseAddr = (u32)&USART1->DR;  //DMA外设ADC基地址
	DMA_USART1_RX.DMA_MemoryBaseAddr = (u32)DMA_Rece_Buf_USART1;  //DMA内存基地址
	DMA_USART1_RX.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存
	DMA_USART1_RX.DMA_BufferSize = DMA_Rec_Len_USART1;  //DMA通道的DMA缓存的大小
	DMA_USART1_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_USART1_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_USART1_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_USART1_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_USART1_RX.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_USART1_RX.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_USART1_RX.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel5, &DMA_USART1_RX);  //根据DMA_InitStruct中指定的参数初始化DMA的通道UART4_Tx_DMA_Channe13所标识的寄存器
		
	DMA_Cmd(DMA1_Channel5, ENABLE);  //正式驱动DMA传输
	
	//TX相应的DMA配置
//	DMA_DeInit(DMA1_Channel4);   //将DMA的通道5寄存器重设为缺省值  串口4对应的是DMA通道15
//	DMA_USART1_TX.DMA_PeripheralBaseAddr = (u32)&USART1->DR;  //DMA外设ADC基地址
//	DMA_USART1_TX.DMA_MemoryBaseAddr = (u32)DMA_Tx_Buf_USART1;  //DMA内存基地址
//	DMA_USART1_TX.DMA_DIR = DMA_DIR_PeripheralDST;  //数据传输方向，从内存读取发送到外设
//	DMA_USART1_TX.DMA_BufferSize = DMA_Tx_Len;  //DMA通道的DMA缓存的大小
//	DMA_USART1_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
//	DMA_USART1_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
//	DMA_USART1_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
//	DMA_USART1_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
//	DMA_USART1_TX.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
//	DMA_USART1_TX.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
//	DMA_USART1_TX.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
//	DMA_Init(DMA1_Channel4, &DMA_USART1_TX);  //根据DMA_InitStruct中指定的参数初始化DMA的通道UART4_Tx_DMA_Channe15所标识的寄存器
		
//	DMA_Cmd(DMA1_Channel4, ENABLE);  //正式驱动DMA传输
}
	



//----------------------------------函数说明--------------------------------------------
//函数功能：  重新使能UART4_RX的DMA功能
//入口参数：	无
//返回值：	  无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
void USART1_RX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA1_Channel5, DISABLE ); //先停止DMA，暂停接收 
 	DMA_SetCurrDataCounter(DMA1_Channel5, DMA_Rec_Len_USART1); //DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA1_Channel5, ENABLE); //使能UART4 RX DMA1 所指示的通道 
}	



//----------------------------------函数说明--------------------------------------------
//函数功能：  重新使能UART4_TX的DMA功能
//入口参数：	无
//返回值：	  无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
//void USART1_TX_DMA_Enable(void)
//{ 
//	DMA_Cmd(DMA1_Channel4, DISABLE ); //先停止DMA
// 	DMA_SetCurrDataCounter(DMA1_Channel4, DMA_Tx_Len); //DMA通道的DMA缓存的大小
// 	DMA_Cmd(DMA1_Channel4, ENABLE); //使能UART4 TX DMA1 所指示的通道 
//}	





//----------------------------------函数说明--------------------------------------------
//函数功能：  主从控板通信对应的串口4初始化
//入口参数：	bound：波特率
//返回值：	  无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
void GPRSInit(u32 bound)
	{
	GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能UART4时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound; //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口4
	
	//UART4_TX   GPIOC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOC.10
   
  //UART4_RX	  GPIOC.11初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PC.11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOC.11
	
	//Uart4 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); //开启空闲中断
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); //使能串口4的DMA接收
//	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); //使能串口4的DMA发送
	USART_Cmd(USART1, ENABLE); //使能串口4 
	
	USART1_DMA_Init();  //UART4的DMA功能初始化
}




//----------------------------------函数说明--------------------------------------------
//函数功能：  串口4中断服务程序
//入口参数：	无
//返回值：	  无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
void USART1_IRQHandler(void)
	{
	//串口1空闲中断
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  
	{	
		/* 1.清除标志 */
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);	 //清除中断标志
//		memset(USART1_RX_BUF, 0, sizeof(USART1_RX_BUF));
		
		/* 2.读取DMA */
		USART_ReceiveData(USART1); 	//读取数据
		Usart1_Rec_Cnt = DMA_Rec_Len_USART1 - DMA_GetCurrDataCounter(DMA1_Channel5); 	//接收个数等于接收缓冲区总大小减已经接收的个数
		
		memcpy(USART1_RX_BUF, DMA_Rece_Buf_USART1, Usart1_Rec_Cnt);   //将串口1 DMA接收的数据转移至USART1_RX_BUF
		
		/* 4.开启新的一次DMA接收 */
		USART1_RX_DMA_Enable(); 	//重新使能DMA，等待下一次的接收
     } 
} 







//----------------------------------函数说明--------------------------------------------
//函数功能：	通过串口1向GPRS发送AT指令进入配置状态的功能函数
//入口参数：	无
//返回值：		标志模块是否进入AT模式，进入成功返回0，失败返回1
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
u8 GPRSEnterAT(void)
{
	u8 time = 10;//超过该循环没收到应答也退出
	ClearArray((char *)USART1_RX_BUF,MaxRx1Len);//先清空
	USART1_RX_STA=0;//允许接收
	USART_SendString(USART1,"+++");
	delay_ms(100);
	if(USART1_RX_BUF[0]=='a' && USART1_RX_STA==1)//接收到‘a’
	ClearArray((char *)USART1_RX_BUF,MaxRx1Len);//先清空
	USART1_RX_STA=0;//允许接收
	
	USART_SendString(USART1,"a");
	while(time--)//超时判断
	{
		delay_ms(50);//这个时间应该接收完一个字符了吧？
		if(USART1_RX_STA >= 2)//接收到‘+ok’
		{
			if((USART1_RX_BUF[0]=='+')&&
				(USART1_RX_BUF[1]=='o')&&
				(USART1_RX_BUF[2]=='k'))		
			return 0;//成功进入AT模式
			else break;
		}
	}
	return 1;//超过指定时间没有收到回复
}



//----------------------------------函数说明--------------------------------------------
//函数功能：	通过串口1向GPRS发送AT指令的功能函数
//入口参数：	*str:	发送的信息字符串保存地址
//						*ack:	发送AT指令后模块返回的提示信息存储地址
//返回值：		标志模块是否收到应答，0表示收到；1表示没有收到
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------

u8 GPRSATControl(char *str,char *ack)
{
	u8 time=5;//超过该循环没收到应答也退出
	u8 i=0;
	ClearArray((char *)USART1_RX_BUF,MaxRx1Len);//先清空接收区
	USART1_RX_STA=0;//允许接收
	USART_SendString(USART1,str);//发送指令信息
	while(--time)//超时判断
	{
		delay_ms(20);
		if(USART1_RX_STA > 1)//接收到应答信息，开始判断结束标志符
		{
			if(USART1_RX_BUF[USART1_RX_STA]=='0' && USART1_RX_BUF[USART1_RX_STA-1]=='\r')//收到末尾的换行符
			{	
				USART1_RX_STA|= 0x8000;//把最高位置1，等待处理完再存储接收其他数据
				break;//成功收完应答信息，提前退出
			}
		}
	}
	if(time == 0) return 1;//规定时间未收到应答
	
	for(i=0;i<USART1_RX_STA;i++)//存储应答信息数据
		*(ack+i) = USART1_RX_BUF[i];
	
	return 0;
}



//----------------------------------函数说明--------------------------------------------
//函数功能：  配置GPRS的参数
//入口参数：	*Protocol: 通信协议（TCP）
//						*IP：生产商服务器IP地址
//            *Port：生产商服务器端口号
//返回值：	  无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
void GPRSConfig(char *Protocol, char *IP, char *Port)
{
	char IPPortAT[50]="AT+SOCKB=";
	u8 count=0;//统计整条指令字符数
	int i=0;//单项参数循环
	while(IPPortAT[count] != '\0')
		count++;
	IPPortAT[count] = '\"';count++;
	for(i=0;*(Protocol+i) != '\0';i++)//添加通信协议类型到指令字符串
	{
		IPPortAT[count] = Protocol[i];
		count++;
	}
	IPPortAT[count] = '\"';count++;
	IPPortAT[count] = ',';count++;
	IPPortAT[count] = '\"';count++;
	for(i=0;*(IP+i) != '\0';i++)//添加IP到指令字符串
	{
		IPPortAT[count] = IP[i];
		count++;
	}
	IPPortAT[count] = '\"';count++;
	IPPortAT[count] = ',';count++;
	for(i=0;*(Port+i) != '\0';i++)//添加端口号到指令字符串
	{
		IPPortAT[count] = Port[i];
		count++;
	}
	IPPortAT[count] = '\r';count++;IPPortAT[count] = '\n';//添加换行符

	
	
	GPRSEnterAT();  //进入AT指令
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


//---------------------函数说明--------------------------------------------//
//函数功能：	通过AT指令查询相关配置参数
//入口参数：	*Command:	发送的查询命令
//						*Respond:	模块传回的信息
//返回值：		标志模块是否收到应答，0表示收到；1表示没有收到

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/07
//修改内容：初次完成基本功能
//----------------------------
u8 ATQuery(char *Command,char *Respond)
{
	char command[MaxGPRSCommandLen]="AT+";
	u8 count=0;//统计整条指令字符数
	u8 i=0;//单项参数循环
	u8 status = 0;//返回指令运行状态
	
	while(command[count] != '\0')
		count++;
	for(i=0;*(Command+i) != '\0';i++)//添加查询指令类容到指令字符串
	{
		command[count] = Command[i];
		count++;
	}
	command[count] = '?';count++;//添加问号
	command[count] = '\r';count++;command[count] = '\n';	//添加换行符
	
	status = GPRSATControl(command,Respond);//发送AT指令，并接收模块的应答信息
	if(status) 
		return 1;//没有收到应答
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
	
	//MST收到HMIT发送的请求同步序列号的命令
	if(HMITtoMSTRequestSynSeriesNumFlag==1)
	{
		u8 PacketLength = 0;
		u8 DataPacket[30];
//		u8 ReceiveBuf[20];
//		HMITtoMSTRequestSynSeriesNumFlag=0;
		if(OnceSendGPRS == 0)
		{
			PacketLength = DataPacking(0x0C, 0x20, 0x00, 0x01, FunctionCode, (u8 *)EquipmentNo, (u8 *)DataPacket);  //数据打包成一个数据包并将数据包长度存入PacketLength
			USART_SendLenData(USART1, (u8 *)DataPacket, PacketLength);    //串口1发送请求同步序列号的命令数据包
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


