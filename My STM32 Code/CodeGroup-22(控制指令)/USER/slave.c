#include "sys.h"
#include "string.h"	
#include "slave.h"
#include "user.h"
#include "hmi_handle.h"
#include "delay.h"



//#define FunctionCodeOfConfigLimit 0x58
//#define FunctionCodeOfConfigUploadtime  0x59
//#define FunctionCodeOf  0x59
//#define FunctionCodeOf  0x59
//#define FunctionCodeOf  0x59


int BSTtoHMITGetSensorStateSuccessFlag = 0;


u8  DMA_Rece_Buf[DMA_Rec_Len];	   //DMA接收串口数据缓冲区
u16  Uart4_Rec_Cnt=0;             //本帧数据长度	
u8  DMA_Tx_Buf[DMA_Tx_Len];	   //DMA接收串口数据缓冲区
u16  Uart4_Tx_Cnt=0;             //本帧数据长度	

u8 BoradSlaveBuf[35];   //实际33

//={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//												0x00, 0x00, 0x00, 0x00, 0x00};

u8 RLED1State, RLED3State, RLED5State, GLED1State, GLED3State, GLED5State;
u8 Cup1State, Cup3State, Cup5State;
u8 Water1SensorState, Water3SensorState, Water5SensorState, WaterSensorPowerState;
u8 PUMP1State, PUMP3State, PUMP5State;
u8 VALVE1State, VALVE3State, VALVE5State;
u8 LockState;
u8 SetUploadTimeInterval;
u16 ReportRemainLimits;



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
void SlaveInit(u32 bound)
	{
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
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	
	//串口1空闲中断
	if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)  
	{	
		/* 1.清除标志 */
		USART_ClearITPendingBit(UART4, USART_IT_IDLE);	 //清除中断标志
		
		/* 2.读取DMA */
		USART_ReceiveData(UART4); 	//读取数据
		Uart4_Rec_Cnt = DMA_Rec_Len - DMA_GetCurrDataCounter(DMA2_Channel3); 	//接收个数等于接收缓冲区总大小减已经接收的个数
		
		/* 3.搬移数据进行其他处理 */
//		memcpy(DMA_Tx_Buf, DMA_Rece_Buf, Uart4_Rec_Cnt); 	//将接收转移通过串口4的DMA方式发送出去测试
//		UART4_TX_DMA_Enable(); 	//开启一次DMA发送，实现转发

		
		
		memcpy(BoradSlaveBuf, DMA_Rece_Buf, Uart4_Rec_Cnt);   //将串口5 DMA接收的数据转移至BoradSlaveBuf
		
		
		/* 4.开启新的一次DMA接收 */
		UART4_RX_DMA_Enable(); 	//重新使能DMA，等待下一次的接收
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
	





void BoradSlaveTask(void)
{
	if(HMITtoBSTGetSensorStateFlag ==1)  //HMI发送启动标志
	{
		if(FrameCheckIfSendAck(UART4, (u8 *)BoradSlaveBuf)==0)  //校验合格
		{
			if(BoradSlaveBuf[6]==0x71)   //数据包功能码为0x71
			{
				BoradSlaveBufDeal();   //分类处理数据
				BSTtoHMITGetSensorStateSuccessFlag = 1;  //分类处理完成标志
			}
		}
	}
}




void BoradSlaveBufDeal(void)
{
	RLED1State=BoradSlaveBuf[7];
	RLED3State=BoradSlaveBuf[8];
	RLED5State=BoradSlaveBuf[9];
	
	GLED1State=BoradSlaveBuf[10];
	GLED3State=BoradSlaveBuf[11];
	GLED5State=BoradSlaveBuf[12];
	
	Cup1State=BoradSlaveBuf[13];
	Cup3State=BoradSlaveBuf[14];
	Cup5State=BoradSlaveBuf[15];
	
	Water1SensorState=BoradSlaveBuf[16];
	Water3SensorState=BoradSlaveBuf[17];
	Water5SensorState=BoradSlaveBuf[18];
	
	PUMP1State=BoradSlaveBuf[19];
	PUMP3State=BoradSlaveBuf[20];
	PUMP5State=BoradSlaveBuf[21];
	
	VALVE1State=BoradSlaveBuf[22];
	VALVE3State=BoradSlaveBuf[23];
	VALVE5State=BoradSlaveBuf[24];
	
	WaterSensorPowerState=BoradSlaveBuf[25];
	
	LockState=BoradSlaveBuf[26];
	
	SetUploadTimeInterval=BoradSlaveBuf[27];
	
	ReportRemainLimits|=BoradSlaveBuf[28]<<8;
	ReportRemainLimits|=BoradSlaveBuf[29];
}






//---------------------函数说明--------------------------------------------//
//函数功能：	设置从控板的限制次数
//入口参数：	*LimitNum：设置的限制值的来源起始地址（固定2byte）
//返回值：	无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/25
//修改内容：修改函数原型，不需要返回指令的执行状态
//--------------------------------
void ConfigLimit(u8 *LimitNum)
{
	u8 datalen = 0x06;//该条指令的数据区长度
	u8 Data_H =*LimitNum;
	u8 Data_L =*(LimitNum+1);
	u8 i=0;	//数据串下标
	u8 SendStr[DMA_Tx_Len] =		//下面只缓存到校验位之前
										{0xAA,0x55,//数据包头
										datalen,//数据段长度
										0x20,0x21,//发送和接收方都是设备，识别码为0x02
										0x00,//是否需要应答
										0x5C,//功能码，由总协议文档规定
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

	USART_SendLenData(UART4,SendStr,datalen+6);//发送数据包				
}
//Function OK 


//---------------------函数说明--------------------------------------------//
//函数功能：	设置从控板主动上传数据的周期
//入口参数：	Period:时间间隔等级，即最小间隔的倍数，0表示停止主动上传					
//返回值：	无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/23
//修改内容：根据新的协议，修改参数类型和功能码
//--------------------------------
void ConfigUploadtime(u8 Period)
{
	u8 datalen = 0x06;//该条指令的数据区长度
	u8 i=0;	//数据串下标
	u8 SendStr[DMA_Tx_Len] =		//下面只缓存到校验位之前
										{0xAA,0x55,//数据包头
										datalen,//数据段长度
										0x20,0x21,//发送和接收方识别码
										0x00,//是否需要应答
										0x60,//功能码，由总协议文档规定
										0x00,Period,//上传时间间隔
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

	USART_SendLenData(UART4,SendStr,datalen+6);//发送数据包
}




//---------------------函数说明--------------------------------------------//
//函数功能：	查询从控板的工作状态
//入口参数：	无
//返回值：	无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/23
//修改内容：在以前的版本上，根据新的协议要求，修改了函数原型以及功能码（大改）
//--------------------------------
void QueryWorkState(void)
{
	u8 datalen = 0x06;//该条指令的数据区长度
	u8 i=0;	//数据串下标
	u8 SendStr[DMA_Tx_Len] =		//下面只缓存到校验位之前
										{0xAA,0x55,//数据包头
										datalen,//数据段长度
										0x20,0x21,
										0x00,//是否需要应答
										0x5E,//功能码，由总协议文档规定
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
	USART_SendLenData(UART4,SendStr,datalen+6);//发送数据包
}



//---------------------函数说明--------------------------------------------//
//函数功能：	主控板向从控板发送控制信息
//入口参数：	CUPx：控制哪一个杯座
//						CupDisEnable：控制杯孔的使能，0x00失能，0x01使能，0x02维持原状态
//						PumpDisEnable：控制杯座对应的泵的使能，0x00开启，0x01关闭，0x02维持原状态
//返回值：		无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/25
//修改内容：初次完成基本功能
//--------------------------------
void ControlSlave(u8 CUPx,u8 CupDisEnable,u8 PumpDisEnable)
{
	u8 i=0;	//数据串下标
	u8 datalen = 0x1E;//该条指令的数据区长度
	u8 Parity = datalen;
	u8 SendStr[DMA_Tx_Len] =		//下面只缓存到校验位之前
										{0xAA,0x55,//数据包头
										0x1E,//数据段长度
										0x20,0x21,
										0x00,//是否需要应答
										0x5B,//功能码，由总协议文档规定
										0x02,0x02,0x02,0x02,0x02,0x02, 0x02,0x02,0x02,0x02,0x02,0x02,//不受控的对象，6个LED和6个传感器状态
										0x02,0x02,0x02,//三个泵对应的继电器
										0x02,0x02,0x02,//三个阀对应的继电器
										0x02,0x02,0x02,0x02,0x02,//其他可控对象，暂时没有使用
										0x02,0x02,0x02//三个杯子的使能控制
										};
				
		SendStr[18+CUPx] = PumpDisEnable;	
		SendStr[28+CUPx] = CupDisEnable;
		//重新写状态控制完毕
								
	for(i=3;i<32;i++)//计算校验数据//3+ 26+4 = 33
		Parity   =Parity^SendStr[i];//按位异或运算
										
	SendStr[i]=Parity;i++;//添加校验数据
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//添加数据包尾部
//数据帧打包完毕

	USART_SendLenData(UART4,SendStr,datalen+6);//发送数据包//长度需要改
}
	

