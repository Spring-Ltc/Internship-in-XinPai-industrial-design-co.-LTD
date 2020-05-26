#include "sys.h"
#include "string.h"	
#include "slave.h"


u8  DMA_Rece_Buf[DMA_Rec_Len];	   //DMA接收串口数据缓冲区
u16  Uart4_Rec_Cnt=0;             //本帧数据长度	
u8  DMA_Tx_Buf[DMA_Tx_Len];	   //DMA接收串口数据缓冲区
u16  Uart4_Tx_Cnt=0;             //本帧数据长度	


//【串口屏从这里读取从控板的状态数据，从控板负责更新这里的数据】
u8 BoradSlaveBuf[29]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
											0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
											0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};



//----------------------------------函数说明--------------------------------------------
//函数功能：  串口4的DMA初始化
//入口参数：	无
//返回值：	  无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
void UART4_DMA_Init(void)
{
	//UART4_RX和DMA_UART4_TX的DMA初始化
	DMA_InitTypeDef DMA_UART4_RX;
	DMA_InitTypeDef DMA_UART4_TX;
	
	//RX相应的DMA配置
	DMA_DeInit(DMA2_Channel3);   //将DMA的通道5寄存器重设为缺省值  串口4对应的是DMA通道13
	DMA_UART4_RX.DMA_PeripheralBaseAddr = (u32)&UART4->DR;  //DMA外设ADC基地址
	DMA_UART4_RX.DMA_MemoryBaseAddr = (u32)DMA_Rece_Buf;  //DMA内存基地址
	DMA_UART4_RX.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存
	DMA_UART4_RX.DMA_BufferSize = DMA_Rec_Len;  //DMA通道的DMA缓存的大小
	DMA_UART4_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_UART4_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_UART4_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_UART4_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_UART4_RX.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_UART4_RX.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_UART4_RX.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA2_Channel3, &DMA_UART4_RX);  //根据DMA_InitStruct中指定的参数初始化DMA的通道UART4_Tx_DMA_Channe13所标识的寄存器
		
	DMA_Cmd(DMA2_Channel3, ENABLE);  //正式驱动DMA传输
	
	//TX相应的DMA配置
	DMA_DeInit(DMA2_Channel5);   //将DMA的通道5寄存器重设为缺省值  串口4对应的是DMA通道15
	DMA_UART4_TX.DMA_PeripheralBaseAddr = (u32)&UART4->DR;  //DMA外设ADC基地址
	DMA_UART4_TX.DMA_MemoryBaseAddr = (u32)DMA_Tx_Buf;  //DMA内存基地址
	DMA_UART4_TX.DMA_DIR = DMA_DIR_PeripheralDST;  //数据传输方向，从内存读取发送到外设
	DMA_UART4_TX.DMA_BufferSize = DMA_Tx_Len;  //DMA通道的DMA缓存的大小
	DMA_UART4_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_UART4_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_UART4_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_UART4_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_UART4_TX.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_UART4_TX.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_UART4_TX.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA2_Channel5, &DMA_UART4_TX);  //根据DMA_InitStruct中指定的参数初始化DMA的通道UART4_Tx_DMA_Channe15所标识的寄存器
		
	DMA_Cmd(DMA2_Channel5, ENABLE);  //正式驱动DMA传输
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
void UART4_RX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA2_Channel3, DISABLE ); //先停止DMA，暂停接收 
 	DMA_SetCurrDataCounter(DMA2_Channel3, DMA_Rec_Len); //DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA2_Channel3, ENABLE); //使能UART4 RX DMA1 所指示的通道 
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
void UART4_TX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA2_Channel5, DISABLE ); //先停止DMA
 	DMA_SetCurrDataCounter(DMA2_Channel5, DMA_Tx_Len); //DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA2_Channel5, ENABLE); //使能UART4 TX DMA1 所指示的通道 
}	





//----------------------------------函数说明--------------------------------------------
//函数功能：  主从控板通信对应的串口4初始化
//入口参数：	bound：波特率
//返回值：	  无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
void SlaveInit(u32 bound){
	GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//使能UART4时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	//使能DMA传输
	
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound; //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(UART4, &USART_InitStructure); //初始化串口4
	
	//UART4_TX   GPIOC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.10
   
  //UART4_RX	  GPIOC.11初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC.11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.11
	
	//Uart4 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); //开启空闲中断
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE); //使能串口4的DMA接收
	USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE); //使能串口4的DMA发送
	USART_Cmd(UART4, ENABLE); //使能串口4 
	
	UART4_DMA_Init();  //UART4的DMA功能初始化
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
void UART4_IRQHandler(void)
{
	//串口1空闲中断
	if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)  
	{	
		/* 1.清除标志 */
		USART_ClearITPendingBit(UART4, USART_IT_IDLE);	 //清除中断标志
		
		/* 2.读取DMA */
		USART_ReceiveData(UART4); 	//读取数据
		Uart4_Rec_Cnt = DMA_Rec_Len - DMA_GetCurrDataCounter(DMA2_Channel3); 	//接收个数等于接收缓冲区总大小减已经接收的个数
		
		/* 3.搬移数据进行其他处理 */
		memcpy(DMA_Tx_Buf, DMA_Rece_Buf, Uart4_Rec_Cnt); 	//将接收转移通过串口4的DMA方式发送出去测试
		UART4_TX_DMA_Enable(); 	//开启一次DMA发送，实现转发

		/* 4.开启新的一次DMA接收 */
		UART4_RX_DMA_Enable(); 	//重新使能DMA，等待下一次的接收
   } 
} 
	





void BoradSlaveTask(void)
{
	
//	u8 BoradSlaveBuf[29]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x00, 0x00, 0x00, 0x00};
	////////////
}










//
//-------------------------上面为STM32相关外设配置，下面为和从控通信的功能函数----------------------------------------------------------------------------------------------------------------
//



////---------------------函数说明--------------------------------------------//
////函数功能：	设置从控板的限制次数
////入口参数：	LimitNum：设置的限制值
////返回值：	指令是否执行成功，0表示成功；其他表示失败	

////--------------------------------
////作者：Liao_tingchun		
////修改日期：2020/05/13
////修改内容：初次完成基本功能
////--------------------------------
//u8 ConfigLimit(u16 LimitNum)
//{
//	u8 datalen = 0x06;//该条指令的数据区长度
//	u8 Data_H =(u8) (LimitNum >>8);
//	u8 Data_L =LimitNum & 0x00FF;
//	u8 i=0;	//数据串下标
//	u8 SendStr[MaxSendStr] =		//下面只缓存到校验位之前
//										{0xAA,0x55,//数据包头
//										datalen,//数据段长度
//										0x20,0x21,//发送和接收方都是设备，识别码为0x02
//										0x00,//是否需要应答
//										0x57,//功能码，由总协议文档规定
//										Data_H,Data_L,//写入的数据，限制次数
//										};
//	u8 Parity = datalen;
//	for(i=3;i<datalen+3;i++)//计算校验数据
//	{
//		Parity   =Parity^SendStr[i];//按位异或运算
//	}
//	SendStr[i]=Parity;i++;//添加校验数据
//	SendStr[i]=0x55;i++;
//	SendStr[i]=0xAA;i++;//添加数据包尾部
////数据帧打包完毕

//	USART_SendLenData(PortSlave,SendStr,datalen+6);//发送数据包
//	delay_ms(500);
//	if(QueryLimit() == LimitNum)//延时后判断读取的值和设定的值是否相同
//		return 0;
//	else return 1;		//设定的值和查询的值不一样，设置失败					
//}
////Function OK 





////---------------------函数说明--------------------------------------------//
////函数功能：	查询从控板的剩余使用次数
////入口参数：无
////返回值：		查询的结果或错误，返回0xFFFF表示该条指令执行错误

////--------------------------------
////作者：Liao_tingchun		
////修改日期：2020/05/10
////修改内容：初次完成基本功能
////--------------------------------
//u16 QueryLimit(void)
//{
//	u8 time = 5;//设置超时时间
//	u8 datalen = 0x06;//该条指令的数据区长度
//	u8 i=0;	//数据串下标
//	u8 SendStr[MaxSendStr] =		//下面只缓存到校验位之前
//										{0xAA,0x55,//数据包头
//										datalen,//数据段长度
//										0x20,0x21,//发送和接收方都是设备，识别码为0x02
//										0x00,//是否需要应答
//										0x58,//功能码，由总协议文档规定
//										0xFF,0xFF,//写入的数据，限制次数
//										};
//	u8 Parity = datalen;
//	for(i=3;i<datalen+3;i++)//计算校验数据
//	{
//		Parity   =Parity^SendStr[i];//按位异或运算
//	}
//	SendStr[i]=Parity;i++;//添加校验数据
//	SendStr[i]=0x55;i++;
//	SendStr[i]=0xAA;i++;//添加数据包尾部
////数据帧打包完毕

//	ClearArray_u8(ReceiveBuff,UART4_DMA_DataLen);//清空接收
//	USART_SendLenData(PortSlave,SendStr,datalen+6);//发送数据包
//	Flag_UART4_Solve=0;//允许接收处理
//	while(--time)
//	{
//		delay_ms(5);
//		if(Flag_UART4_Solve)//接收到数据,已经存在ReceiveBuff里等待处理
//		{
//			//解析收到的数据
//			//验证数据的合理性，合理则返回正确的值，不合理则开启下一次接收（可能对方发送的第二帧才是回应）
//			while(Flag_UART4_Solve)
//			{
//				if(ReceiveLen != 12) break;//数据帧长度验证失败
//				if(ReceiveBuff[0] != 0xAA || ReceiveBuff[1] != 0x55 || 	//包头校验
//					 ReceiveBuff[ReceiveLen-1] != 0xAA || ReceiveBuff[ReceiveLen] != 0xAA)//包尾校验
//					 break;//包头或包尾验证失败
//				Parity = ReceiveBuff[2];
//				for(i=3;i< ReceiveBuff[2]+3;i++)
//					Parity   =Parity^SendStr[i];//按位异或运算
//				if(Parity != ReceiveBuff[ReceiveLen-2]) break;//校验和验证失败
//				
//				return ((u16)ReceiveBuff[ReceiveLen-4]<<8) + ((u16)ReceiveBuff[ReceiveLen-3]);
//			}
//			Flag_UART4_Solve=0;//当前验证失败，允许接收处理下一帧数据，在规定时间内继续验证
//			ClearArray_u8(ReceiveBuff,UART4_DMA_DataLen);//清空接收
//		}
//	}
//	return 0xFFFF;	//超时或收到的数据验证失败				
//}



////---------------------函数说明--------------------------------------------//
////函数功能：	设置从控板主动上传数据的周期
////入口参数：	Period:时间间隔等级，即最小间隔的倍数，0表示停止主动上传					
////返回值：	指令是否执行成功，0表示成功；其他表示失败		

////--------------------------------
////作者：Liao_tingchun		
////修改日期：2020/05/10
////修改内容：初次完成基本功能
////--------------------------------
//u8 ConfigUploadtime(u16 Period)
//{
//	u8 Period_H = (u8)(Period/256);//高字节
//	u8 Period_L = (u8)(Period%256);//低字节
//	u8 datalen = 0x06;//该条指令的数据区长度
//	u8 i=0;	//数据串下标
//	u8 SendStr[MaxSendStr] =		//下面只缓存到校验位之前
//										{0xAA,0x55,//数据包头
//										datalen,//数据段长度
//										0x20,0x21,//发送和接收方识别码
//										0x00,//是否需要应答
//										0x73,//功能码，由总协议文档规定
//										Period_H,Period_L,//上传时间间隔
//										};
//	u8 Parity = datalen;
//	for(i=3;i<datalen+3;i++)//计算校验数据
//	{
//		Parity   =Parity^SendStr[i];//按位异或运算
//	}
//	SendStr[i]=Parity;i++;//添加校验数据
//	SendStr[i]=0x55;i++;
//	SendStr[i]=0xAA;i++;//添加数据包尾部
////数据帧打包完毕
//	
//	//清空接收
//	USART_SendLenData(PortSlave,SendStr,datalen+6);//发送数据包
//	//发送查询指令，接收查询结果【调用查询函数完成，超时在函数内部实现】
//	//判断查询结果
//	return 0;					
//}




////---------------------函数说明--------------------------------------------//
////函数功能：	查询从控板的工作状态
////入口参数：	Ackbuff：查询的结果，状态返回的存储地址
////返回值：	指令是否执行成功，0表示成功；其他表示失败

////--------------------------------
////作者：Liao_tingchun		
////修改日期：2020/05/10
////修改内容：初次完成基本功能
////--------------------------------
//u8 QueryWorkState(u8 *Ackbuff)
//{
//	u8 time = 5;//设置超时时间
//	u8 datalen = 0x06;//该条指令的数据区长度
//	u8 i=0;	//数据串下标
//	u8 SendStr[MaxSendStr] =		//下面只缓存到校验位之前
//										{0xAA,0x55,//数据包头
//										datalen,//数据段长度
//										0x20,0x21,
//										0x00,//是否需要应答
//										0x72,//功能码，由总协议文档规定
//										0xFF,0xFF,//写入的数据，填充
//										};.
//	u8 Parity = datalen;
//	for(i=3;i<datalen+3;i++)//计算校验数据
//	{
//		Parity   =Parity^SendStr[i];//按位异或运算
//	}
//	SendStr[i]=Parity;i++;//添加校验数据
//	SendStr[i]=0x55;i++;
//	SendStr[i]=0xAA;i++;//添加数据包尾部
////数据帧打包完毕

//	ClearArray_u8(ReceiveBuff,UART4_DMA_DataLen);//清空接收
//	USART_SendLenData(PortSlave,SendStr,datalen+6);//发送数据包
//	Flag_UART4_Solve=0;//允许接收处理
//	while(--time)
//	{
//		if(Flag_UART4_Solve)//接收到数据
//		{
//			//解析收到的数据
//			//验证数据的合理性，合理则返回正确的值，不合理则开启下一次接收（可能对方发送的第二帧才是回应）
//			
//			
//		}
//	}
//	return 1;	//超时或收到的数据验证失败	
//}





////---------------------函数说明--------------------------------------------//
////函数功能：	主控板向从控板发送控制信息
////入口参数：	*State：控制的状态列表
////返回值：	指令是否执行成功，0表示成功；其他表示失败

////--------------------------------
////作者：Liao_tingchun		
////修改日期：2020/05/13
////修改内容：初次完成基本功能
////--------------------------------
//u8 ControlSlave(u8 *State)
//{
//	u8 i=0;	//数据串下标
//	u8 ack[ReceiveState]={0};//设置后，返回的状态
//	u8 datalen = 0x1B;//该条指令的数据区长度
//	u8 Parity = datalen;
//	u8 SendStr[MaxSendStr] =		//下面只缓存到校验位之前
//										{0xAA,0x55,//数据包头
//										datalen,//数据段长度
//										0x20,0x21,
//										0x00,//是否需要应答
//										0x55,//功能码，由总协议文档规定
//										};
//	for(i=0;i<23;i++)
//		SendStr[i+7] = *(State+i);//把需要控制的状态数据添加到里面
//										
//	for(i=3;i<datalen+3;i++)//计算校验数据
//	{
//		Parity   =Parity^SendStr[i];//按位异或运算
//	}
//	SendStr[i]=Parity;i++;//添加校验数据
//	SendStr[i]=0x55;i++;
//	SendStr[i]=0xAA;i++;//添加数据包尾部
////数据帧打包完毕
//	
//	//清空接收
//	USART_SendLenData(PortSlave,SendStr,datalen+6);//发送数据包
//	//发送查询指令，接收查询结果【调用查询函数完成，超时在函数内部实现】
//	//判断查询结果
//	
//	return 1;		
//}
//	
//	

