

#include "Slave.h"
#include "user.h"
#include "delay.h"

#define ReceiveState 20	//需要查询20个设备
#define ControlState 23	//需要控制23个设备

u8 SendBuff[UART4_DMA_DataLen]={0};
u8 ReceiveBuff[UART4_DMA_DataLen]={0};//收到并需要处理的数据
u8 UART4_RX_DMA_BUF[UART4_DMA_DataLen]={0};//串口4的DMA接收缓存
u8 UART4_TX_DMA_BUF[UART4_DMA_DataLen]={0};//串口4的DMA接收缓存
u8 Flag_UART4_Solve = 0;//数据待处理标志
u16 ReceiveLen = 0;

u16 shengyu=0;

//---------------------函数说明--------------------------------------------//
//函数功能：	初始化从机用到的串口，这里用的串口4，使用DMA传输方式
//					RX引脚为PC11，TX引脚为PC10
//入口参数：	bound：和从机通信的波特率
//返回值：		无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/09
//修改内容：初次完成基本功能
//--------------------------------
void SlaveInit(u32 bound)
{
	SlaveUSART_Init(bound);
	SlaveDMA_Init();
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);//开启串口空闲中断
	USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);//开启串口DMA接收
	USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);//开启串口DMA接收
	USART_Cmd(UART4, ENABLE);//使能串口
}


//从机串口初始化
void SlaveUSART_Init(u32 bound)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	//GPIO配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//UART4_TX   GPIOC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//UART4_RX	  GPIOC.11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);  
   //UART 初始化设置
	USART_DeInit(UART4); //【先复位一下串口】
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(UART4, &USART_InitStructure); //初始化串口5
  //UART4 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;	//串口中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}

//从机DMA初始化
void SlaveDMA_Init(void)
{
	DMA_InitTypeDef TX_DMA_InitStruct;//DMA结构体变量
	DMA_InitTypeDef RX_DMA_InitStruct;//DMA结构体变量
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE);//开启DMA时钟
	
	//DMA配置TX
	DMA_DeInit(UART4_TX_DMA_Channel);//【配置前，先恢复默认值】
	TX_DMA_InitStruct.DMA_BufferSize =	UART4_DMA_DataLen;		//DMA通道DMA缓存的大小
	TX_DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;		//DMA传输的方向,数据到外设去
	TX_DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;		//内存到内存的传输使能
	TX_DMA_InitStruct.DMA_MemoryBaseAddr = (u32)DMA_SendAddress; 		//内存地址,把这里的数据发出去	
	TX_DMA_InitStruct.DMA_MemoryDataSize =	DMA_MemoryDataSize_Byte;		//内存数据宽度
	TX_DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;		//内存地址递增使能
	TX_DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//循环接收//【改为不需要循环接收】
	TX_DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)DMA_PortAddress;		//外设地址		
	TX_DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据宽度
	TX_DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//外设地址递增使能
	TX_DMA_InitStruct.DMA_Priority = DMA_Priority_High;			//DMA优先级
	DMA_Init(UART4_TX_DMA_Channel,&TX_DMA_InitStruct);
	//DMA_Cmd(UART4_TX_DMA_Channel,ENABLE);//【先不要使能，不然上电就会发送一串0x00数据出去】
	
	//DMA配置RX
	DMA_DeInit(UART4_RX_DMA_Channel);//【配置前，先恢复默认值】
	RX_DMA_InitStruct.DMA_BufferSize =	UART4_DMA_DataLen;		//DMA通道DMA缓存的大小
	RX_DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;		//DMA传输的方向，数据从外设来
	RX_DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;		//内存到内存的传输使能
	RX_DMA_InitStruct.DMA_MemoryBaseAddr = (u32)DMA_ReceiveAddress; 		//内存地址			【为什么要转成u32类型？？】
	RX_DMA_InitStruct.DMA_MemoryDataSize =	DMA_MemoryDataSize_Byte;		//内存数据宽度
	RX_DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;		//内存地址递增使能
	RX_DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//循环接收//【改为不需要循环接收】
	RX_DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)DMA_PortAddress;		//外设地址		
	RX_DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据宽度
	RX_DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//外设地址递增使能
	RX_DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;			//DMA优先级
	DMA_Init(UART4_RX_DMA_Channel,&RX_DMA_InitStruct);
	//DMA_Cmd(UART4_RX_DMA_Channel,ENABLE);//【先不要使能，不然上电就会发送一串0x00数据出去】
}



//重新使能UART4的DMA发送功能
void TX_DMA_ENABLE(void)
{
	DMA_Cmd(UART4_TX_DMA_Channel,DISABLE);
	DMA_SetCurrDataCounter(UART4_TX_DMA_Channel,UART4_DMA_DataLen);
	DMA_Cmd(UART4_TX_DMA_Channel,ENABLE);
}
//重新使能UART4的DMA接收功能
void RX_DMA_ENABLE(void)
{
	DMA_Cmd(UART4_RX_DMA_Channel,DISABLE);
	DMA_SetCurrDataCounter(UART4_RX_DMA_Channel,UART4_DMA_DataLen);
	DMA_Cmd(UART4_RX_DMA_Channel,ENABLE);
}





//串口4中断服务程序
void UART4_IRQHandler(void)                
{
	u8 i=0;
	if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)//串口空闲中断
	{
		//DMA_Cmd(UART4_RX_DMA_Channel,DISABLE);
		//USART_ClearITPendingBit(UART4,USART_IT_IDLE);//清串口空闲标志
		USART_ReceiveData(UART4);//【空读一次】
		
		if(Flag_UART4_Solve == 0)//上次的数据已经处理完毕,允许转存
		{
			//Flag_UART4_Solve=1;//置位标志，等待处理并复位
			shengyu=DMA_GetCurrDataCounter(DMA2_Channel3);
			ReceiveLen = UART4_DMA_DataLen - shengyu;//DMA_GetCurrDataCounter(DMA2_Channel3);//计算收到的字节数
			for(i=0;i<ReceiveLen;i++)
				ReceiveBuff[i]=UART4_RX_DMA_BUF[i];//把数据转存到需要处理的数组
			Flag_UART4_Solve=1;//置位标志，等待处理并复位
		}
		USART_ClearITPendingBit(UART4,USART_IT_IDLE);//清串口空闲标志
		RX_DMA_ENABLE();//再开启DMA
	} 
} 






//
//-------------------------上面为STM32相关外设配置，下面为和从控通信的功能函数----------------------------------------------------------------------------------------------------------------
//



//---------------------函数说明--------------------------------------------//
//函数功能：	设置从控板的限制次数
//入口参数：	LimitNum：设置的限制值
//返回值：	指令是否执行成功，0表示成功；其他表示失败	

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/13
//修改内容：初次完成基本功能
//--------------------------------
u8 ConfigLimit(u16 LimitNum)
{
	u8 datalen = 0x06;//该条指令的数据区长度
	u8 Data_H =(u8) (LimitNum >>8);
	u8 Data_L =LimitNum & 0x00FF;
	u8 i=0;	//数据串下标
	u8 SendStr[MaxSendStr] =		//下面只缓存到校验位之前
										{0xAA,0x55,//数据包头
										datalen,//数据段长度
										0x20,0x21,//发送和接收方都是设备，识别码为0x02
										0x00,//是否需要应答
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
//数据帧打包完毕

	USART_SendLenData(PortSlave,SendStr,datalen+6);//发送数据包
	delay_ms(500);
	if(QueryLimit() == LimitNum)//延时后判断读取的值和设定的值是否相同
		return 0;
	else return 1;		//设定的值和查询的值不一样，设置失败					
}
//Function OK 





//---------------------函数说明--------------------------------------------//
//函数功能：	查询从控板的剩余使用次数
//入口参数：无
//返回值：		查询的结果或错误，返回0xFFFF表示该条指令执行错误

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/10
//修改内容：初次完成基本功能
//--------------------------------
u16 QueryLimit(void)
{
	u8 time = 5;//设置超时时间
	u8 datalen = 0x06;//该条指令的数据区长度
	u8 i=0;	//数据串下标
	u8 SendStr[MaxSendStr] =		//下面只缓存到校验位之前
										{0xAA,0x55,//数据包头
										datalen,//数据段长度
										0x20,0x21,//发送和接收方都是设备，识别码为0x02
										0x00,//是否需要应答
										0x58,//功能码，由总协议文档规定
										0xFF,0xFF,//写入的数据，限制次数
										};
	u8 Parity = datalen;
	for(i=3;i<datalen+3;i++)//计算校验数据
	{
		Parity   =Parity^SendStr[i];//按位异或运算
	}
	SendStr[i]=Parity;i++;//添加校验数据
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//添加数据包尾部
//数据帧打包完毕

	ClearArray_u8(ReceiveBuff,UART4_DMA_DataLen);//清空接收
	USART_SendLenData(PortSlave,SendStr,datalen+6);//发送数据包
	Flag_UART4_Solve=0;//允许接收处理
	while(--time)
	{
		delay_ms(5);
		if(Flag_UART4_Solve)//接收到数据,已经存在ReceiveBuff里等待处理
		{
			//解析收到的数据
			//验证数据的合理性，合理则返回正确的值，不合理则开启下一次接收（可能对方发送的第二帧才是回应）
			while(Flag_UART4_Solve)
			{
				if(ReceiveLen != 12) break;//数据帧长度验证失败
				if(ReceiveBuff[0] != 0xAA || ReceiveBuff[1] != 0x55 || 	//包头校验
					 ReceiveBuff[ReceiveLen-1] != 0xAA || ReceiveBuff[ReceiveLen] != 0xAA)//包尾校验
					 break;//包头或包尾验证失败
				Parity = ReceiveBuff[2];
				for(i=3;i< ReceiveBuff[2]+3;i++)
					Parity   =Parity^SendStr[i];//按位异或运算
				if(Parity != ReceiveBuff[ReceiveLen-2]) break;//校验和验证失败
				
				return ((u16)ReceiveBuff[ReceiveLen-4]<<8) + ((u16)ReceiveBuff[ReceiveLen-3]);
			}
			Flag_UART4_Solve=0;//当前验证失败，允许接收处理下一帧数据，在规定时间内继续验证
			ClearArray_u8(ReceiveBuff,UART4_DMA_DataLen);//清空接收
		}
	}
	return 0xFFFF;	//超时或收到的数据验证失败				
}



//---------------------函数说明--------------------------------------------//
//函数功能：	设置从控板主动上传数据的周期
//入口参数：	Period:时间间隔等级，即最小间隔的倍数，0表示停止主动上传					
//返回值：	指令是否执行成功，0表示成功；其他表示失败		

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/10
//修改内容：初次完成基本功能
//--------------------------------
u8 ConfigUploadtime(u16 Period)
{
	u8 Period_H = (u8)(Period/256);//高字节
	u8 Period_L = (u8)(Period%256);//低字节
	u8 datalen = 0x06;//该条指令的数据区长度
	u8 i=0;	//数据串下标
	u8 SendStr[MaxSendStr] =		//下面只缓存到校验位之前
										{0xAA,0x55,//数据包头
										datalen,//数据段长度
										0x20,0x21,//发送和接收方识别码
										0x00,//是否需要应答
										0x73,//功能码，由总协议文档规定
										Period_H,Period_L,//上传时间间隔
										};
	u8 Parity = datalen;
	for(i=3;i<datalen+3;i++)//计算校验数据
	{
		Parity   =Parity^SendStr[i];//按位异或运算
	}
	SendStr[i]=Parity;i++;//添加校验数据
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//添加数据包尾部
//数据帧打包完毕
	
	//清空接收
	USART_SendLenData(PortSlave,SendStr,datalen+6);//发送数据包
	//发送查询指令，接收查询结果【调用查询函数完成，超时在函数内部实现】
	//判断查询结果
	
	
	return 0;					
}




//---------------------函数说明--------------------------------------------//
//函数功能：	查询从控板的工作状态
//入口参数：	Ackbuff：查询的结果，状态返回的存储地址
//返回值：	指令是否执行成功，0表示成功；其他表示失败

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/10
//修改内容：初次完成基本功能
//--------------------------------
u8 QueryWorkState(u8 *Ackbuff)
{
	u8 time = 5;//设置超时时间
	u8 datalen = 0x06;//该条指令的数据区长度
	u8 i=0;	//数据串下标
	u8 SendStr[MaxSendStr] =		//下面只缓存到校验位之前
										{0xAA,0x55,//数据包头
										datalen,//数据段长度
										0x20,0x21,
										0x00,//是否需要应答
										0x72,//功能码，由总协议文档规定
										0xFF,0xFF,//写入的数据，填充
										};
	u8 Parity = datalen;
	for(i=3;i<datalen+3;i++)//计算校验数据
	{
		Parity   =Parity^SendStr[i];//按位异或运算
	}
	SendStr[i]=Parity;i++;//添加校验数据
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//添加数据包尾部
//数据帧打包完毕

	ClearArray_u8(ReceiveBuff,UART4_DMA_DataLen);//清空接收
	USART_SendLenData(PortSlave,SendStr,datalen+6);//发送数据包
	Flag_UART4_Solve=0;//允许接收处理
	while(--time)
	{
		if(Flag_UART4_Solve)//接收到数据
		{
			//解析收到的数据
			//验证数据的合理性，合理则返回正确的值，不合理则开启下一次接收（可能对方发送的第二帧才是回应）
			
			
		}
	}
	return 1;	//超时或收到的数据验证失败	
}





//---------------------函数说明--------------------------------------------//
//函数功能：	主控板向从控板发送控制信息
//入口参数：	*State：控制的状态列表
//返回值：	指令是否执行成功，0表示成功；其他表示失败

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/13
//修改内容：初次完成基本功能
//--------------------------------
u8 ControlSlave(u8 *State)
{
	u8 i=0;	//数据串下标
	u8 ack[ReceiveState]={0};//设置后，返回的状态
	u8 datalen = 0x1B;//该条指令的数据区长度
	u8 Parity = datalen;
	u8 SendStr[MaxSendStr] =		//下面只缓存到校验位之前
										{0xAA,0x55,//数据包头
										datalen,//数据段长度
										0x20,0x21,
										0x00,//是否需要应答
										0x55,//功能码，由总协议文档规定
										};
	for(i=0;i<23;i++)
		SendStr[i+7] = *(State+i);//把需要控制的状态数据添加到里面
										
	for(i=3;i<datalen+3;i++)//计算校验数据
	{
		Parity   =Parity^SendStr[i];//按位异或运算
	}
	SendStr[i]=Parity;i++;//添加校验数据
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//添加数据包尾部
//数据帧打包完毕
	
	//清空接收
	USART_SendLenData(PortSlave,SendStr,datalen+6);//发送数据包
	//发送查询指令，接收查询结果【调用查询函数完成，超时在函数内部实现】
	//判断查询结果
	
	return 1;		
}
	
	

