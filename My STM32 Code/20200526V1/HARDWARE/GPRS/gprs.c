#include "gprs.h"
#include "usart.h"
#include "delay.h"
#include "user.h"


char GPRSBackbuf[MaxGPRSLen] = {0};  //GPRS的AT指令返回数据储存区
char USART1_RX_BUF[MaxRx1Len]={0};   //串口1接收缓存
u16 USART1_RX_STA = 0;               //		最高位为存储使能标志位，为0时允许把收到的数据存到数组，为1不存储





//----------------------------------函数说明--------------------------------------------
//函数功能：	串口1（GPRS）的引脚初始化函数
//入口参数：	bound：波特率
//返回值：		无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
void GPRSInit(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//使能USART1时钟
	
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10 

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
}




//----------------------------------函数说明--------------------------------------------
//函数功能：	串口1（GPRS）对应的中断服务程序
//入口参数：	无
//返回值：		无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
void USART1_IRQHandler(void)                
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		if((USART1_RX_STA&0x8000)==0)//允许接收并存储到数组
		{
			USART1_RX_BUF[USART1_RX_STA&0X3FFF] = Res ;
			USART1_RX_STA++;	
			if((USART1_RX_STA  & 0X3FFF) > MaxRx1Len - 1)//异常接收太多数据，重新开始接收
				USART1_RX_STA = 0;
		 }		 	 
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
	ClearArray(USART1_RX_BUF,MaxRx1Len);//先清空
	USART1_RX_STA=0;//允许接收
	USART_SendString(USART1,"+++");
	delay_ms(100);
	if(USART1_RX_BUF[0]=='a' && USART1_RX_STA==1)//接收到‘a’
	ClearArray(USART1_RX_BUF,MaxRx1Len);//先清空
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
	ClearArray(USART1_RX_BUF,MaxRx1Len);//先清空接收区
	USART1_RX_STA=0;//允许接收
	USART_SendString(USART1,str);//发送指令信息
	while(time--)//超时判断
	{
		delay_ms(20);
		if(USART1_RX_STA > 1)//接收到应答信息，开始判断结束标志符
		{
			if(USART1_RX_BUF[USART1_RX_STA]=='\n' && USART1_RX_BUF[USART1_RX_STA-1]=='\r')//收到末尾的换行符
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









