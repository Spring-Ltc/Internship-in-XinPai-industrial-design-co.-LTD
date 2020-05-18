

#include "Slave.h"
#include "user.h"
#include "delay.h"

//---------------------函数说明--------------------------------------------//
//函数功能：	初始化从机用到的串口，这里用的串口1
//					RX引脚为PA10，TX引脚为PA9
//入口参数：	bound：和从机通信的波特率
//返回值：		无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/09
//修改内容：初次完成基本功能
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
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  //USART5_RX	  GPIOA.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //UART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口5
	
  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口5
}



	//串口1中断服务程序
//接收从控板返回的配置提示信息
u8 USART1_RX_BUF[MaxRx1Len]={0};//串口5接收缓存
u16 USART1_RX_STA = 0;
//		最高位为存储使能标志位，为0时允许把收到的数据存到数组，为1不存储
void USART1_IRQHandler(void)                
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		if((USART1_RX_STA&0x8000)==0)//允许接收并存储到数组
		{
			USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
			USART1_RX_STA++;	
			if((USART1_RX_STA  & 0X3FFF) > MaxRx1Len-1)//异常接收太多数据，重新开始接收
				USART1_RX_STA = 0;
		 }		 	 
	 } 
} 





//---------------------函数说明--------------------------------------------//
//函数功能：	设置从控板的限制次数
//入口参数：	LimitNum：设置的限制值
//						IfAck：是否需要应答(0不需要，1需要)
//返回值：		当需要应答时，表示是否设置成功，0表示成功；其他表示失败
//					不需要应答则直接返回0

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/09
//修改内容：初次完成基本功能
//--------------------------------
u8 ConfigLimit(u16 LimitNum,u8 IfAck)
{
	u8 time = 5;//如果需要应答，设置超时时间
	u8 datalen = 0x06;//该条指令的数据区长度
	u8 Data_H =(u8) (LimitNum >>8);
	u8 Data_L =LimitNum & 0x00FF;
	u8 i=0;	//数据串下标
	u8 SendStr[MaxSendStr] =		//下面只缓存到校验位之前
										{0xAA,0x55,//数据包头
										datalen,//数据段长度
										0x20,0x21,//发送和接收方都是设备，识别码为0x02
										IfAck,//是否需要应答
										0x57,//功能码，由总协议文档规定
										Data_H,Data_L,//写入的数据，限制次数
										};
	u8 Parity = datalen;
	for(i=3;i<datalen+3;i++)//计算校验数据
	{
		Parity   =Parity^SendStr[i];//按位异或运算
	}
	SendStr[i]=Parity;i++;//添加校验数据
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//添加数据包尾部

	ClearArray(USART1_RX_BUF,MaxRx1Len);//先清空
	USART1_RX_STA=0;//允许接收
	USART_SendLenData(USART1,SendStr,datalen+6);//发送出去
	if(IfAck)//如果需要应答信号，接收应答信号并判断信息的正确性
	{
		while(--time)//超时判断
		{
			delay_ms(5);
			if(USART1_RX_STA >= 4)//接收到应答信息至少4个字节
			{
				if((USART1_RX_BUF[0]==0x21)&&
					(USART1_RX_BUF[1]==0x20)&&
					(USART1_RX_BUF[2]==datalen)&&
					(USART1_RX_BUF[3]==Parity))		return 0;//收到应答验证成功
				else return 2;//收到应答但验证失败
			}
		 }
		if(time == 0)
			return 1;//超时未收到应答
	}
	return 0;		//不要要应答，返回0						
}


//---------------------函数说明--------------------------------------------//
//函数功能：	查询从控板的已经使用次数
//入口参数：无
//返回值：		查询的结果

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------
u16 QueryLimit(void)
{
	return 0;
}

//---------------------函数说明--------------------------------------------//
//函数功能：	设置从控板的工作模式
//						主动上传模式、主机询问模式
//入口参数：无
//返回值：		查询的结果

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------
u8 ConfigMode(u8 Mode)
{
	return Mode;
}

