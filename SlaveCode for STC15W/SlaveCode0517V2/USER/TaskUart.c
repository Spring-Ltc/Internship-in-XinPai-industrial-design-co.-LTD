


#include "TaskUart.h"
#include "USART.h"






u8 ReceiveBuff[MaxReceiveLen];
//使用的USART1，定时器Timer2作为波特率发生器
void TaskUart1_Init(u16 bound)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE?DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityHigh;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P36_P37;	//切换端口,  
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 
}


u8 ReceiveEnable=0;//状态机状态标志
u8 ReceiveCount=0;
//采用状态机的方式
/********************* UART1中断函数************************/
void UART1_int (void) interrupt UART1_VECTOR
{
	u8 res;
	if(RI)//接收中断标志位
	{
		res = SBUF;
		ReceiveBuff[ReceiveCount++] = res;//存到缓存区
			
		if(ReceiveEnable != 2)	//状态：定时器未打开
		{
			ReceiveCount=0;
			switch (ReceiveEnable)
			{
				case 0://未收到AA
				{
					if(res == 0xAA)
						ReceiveEnable = 1;
					break;
				}
				case 1://收到AA
				{
					if(res == 0x55)
					{
						ReceiveEnable =2;
						ReceiveBuff[0]=0xAA,ReceiveBuff[1]=0x55;//确认收到头部，手动添加到缓存数组
						ReceiveCount=2;//下一个数据从RX1_Buffer[2]开始缓存
  					TR1 = 1;//【开定时器1】
					}
					else if(res == 0xAA)	ReceiveEnable = 1;
					else ReceiveEnable = 0;
					break;
				}
				default:	break;
			}
		}
		RI=0;//清中断标志位，准备接收下一个字节
	}
}





//--------------------------函数说明--------------------------------------------//
//函数功能：	完成对数据帧的校验
//入口参数：	*DATaFrame:待校验的数据帧地址
//返回值：	返回0表示校验成功，返回其他值表示校验失败
//											（1:包头错误；2:包尾或长度错误；3:校验和错误）

//-------------------------函数测试----------------------------------------//
//text1：输入：AA 55 06 20 21 00 58 FF FF 5F 55 AA    返回值：0		说明：数据帧校验成功
//text2：输入：AA 55 06 20 21 00 58 FF FF 11 55 AA    返回值：3		说明：校验和错误
//text3：输入：AA 06 20 21 00 58 FF FF 5F 55 AA       返回值：1		说明：包头错误
//text4：输入：AA 55 06 20 21 00 58 FF FF 5F 55       返回值：2		说明：包尾错误
//text5：输入：AA 55 04 20 21 00 58 FF FF 5F 55 AA    返回值：2		说明：数据长度错误
//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/15
//修改内容：初次完成基本功能
//----------------------------
u8 DataValidityCheck(u8 *DataFrame)
{
	u8 i = 0,datalen = 0,Parity = 0;
	u8 *point;
	point = DataFrame;
	if( (*(point) != 0xAA) || (*(point+1) != 0x55))
		return 1;//数据包头错误
	
	datalen = *(point+2);//获取数据部分长度
	if( (*(point+datalen+4) != 0x55) || (*(point+datalen+5) != 0xAA))
		return 2;//数据包尾错误
	
	Parity = datalen;
	for(i=3;i<datalen+3;i++)//计算校验和
		Parity =Parity ^ *(point+i);//按位异或运算
	if(Parity != *(point+datalen+3))
		return 3;//校验和错误
	
	return 0;//校验成功
}



//--------------------------函数说明--------------------------------------------//
//函数功能：	数据校验成功后，从控板调用该函数解析数据
//入口参数：	*DataBuff:需要解析的数据帧地址
//返回值：		无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/16
//修改内容：初次完成基本功能
//----------------------------
void DataAnalyze(u8 *DataBuff)
{
	u8 i=0;
	u8 *point;
	u8 data_H,data_L;
	u8 FunctionCode = *(point+6);//功能码
	point = DataBuff;
	switch(FunctionCode)
	{
		case 0x55://主控板至从控板的控制信息
		{
			for(i=0;i<6;i++)
				MasterDataBuff[i] = *(point+7+i);//写入6个LED的控制状态
			for(i=0;i<8;i++)
				MasterDataBuff[i+12] = *(point+19+i);//写入8个继电器的控制状态
			for(i=0;i<3;i++)
				MasterDataBuff[i+23] = *(point+30+i);//使能3个孔位			
			TaskControlFlag = 1;
			break;
		}
		case 0x57://设置限制抽液次数
		{
			data_H = *(point+7);
			data_L = *(point+8);
			MasterDataBuff[MA_limit] = data_H;MasterDataBuff[MA_limit+1] = data_L;//EEPROM任务从这里读取并写入，完成设置
			TaskEEPROMFlag = 1;//标志位置位，EEPROM任务写入完成后清标志位
			break;
		}
		case 0x58://询问剩余限制次数
		{
			FramePacking(0x70,&ReceiveBuff[SA_limit]);//直接上传
			//TaskUploadFlag = 1;//置位该上传任务
			break;
		}
		case 0x59://查询工作状态
		{
			TaskUploadFlag = 1;//置位该上传任务
			break;
		}
		case 0x5A://设置上传时间间隔
		{
			data_H = *(point+7);
			data_L = *(point+8);	//只用到了低8位数据
			SlaveDataBuff[SA_time] = data_L;//本地保存时间间隔
			MasterDataBuff[MA_time] = data_L;
			break;
		}
		default:break;
	}	
}


//--------------------------函数说明--------------------------------------------//
//函数功能：	将数据按照协议打包成帧并发送
//入口参数：	FunctionCode:功能码
//						*DataBuff:需要发送的数据区的内容
//返回值：		无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/16
//修改内容：初次完成基本功能
//----------------------------
void FramePacking(u8 FunctionCode,u8 *DataBuff)
{

	u8 DataLen;//数据区长度,指有效的数据长度，不包含设备编码和功能码
	u8 *point = DataBuff;
	u8 Parity;
	u8 i=0;	//数据串下标
	u8 SendStr[MaxSendStr] =		//下面只缓存到校验位之前
										{0xAA,0x55,//数据包头
										0xFF,//数据段长度【MCU的问题吗？初始化不能用变量赋值，后面重写一次吧】
										0x21,0x20,//发送和接收方识别码
										0x00,//是否需要应答
										0xFF//功能码，由总协议文档规定【MCU的问题吗？初始化不能用变量赋值，后面重写一次吧】
										};
	if(FunctionCode == 0x70)	DataLen =2;
	else if(FunctionCode == 0x71)	DataLen =23;	
	SendStr[2] = DataLen+4;//重新把长度写到数据帧里面，+4
	SendStr[6] = FunctionCode;//重新把功能码写到数据帧里面								
	for(i=0;i<DataLen;i++)							
				SendStr[i+7] = *(point+i);	//添加数据区	
										
	Parity = SendStr[2];
	for(i=3;i<SendStr[2]+3;i++)//计算校验数据
		Parity = Parity^SendStr[i];//按位异或运算
										
	SendStr[i]=Parity;i++;//添加校验数据
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//添加数据包尾部
//数据帧打包完毕

	SendDataLen(SendStr,DataLen+10);//发送数据包
}


/**************************************************************************
功能：STC15单片机的串口发送字节的函数
参数：dat:要发送的一个字节   
**************************************************************************/
void UART_Send_Byte(unsigned char dat)
{
		ES=0;		 // 使用查询发送结束方式，禁止中断干预  ES 串行中断 允许位
		SBUF = dat;
		while(!TI);	    
		TI=0;	 //此句可以不要，不影响后面数据的发送，只供代码查询数据是否发送完成
		ES=1;
}	




void SendDataLen(u8 *DataBuff,u8 len)
{
	u8 i=0;
	u8 *point = DataBuff;
	for(i=0;i<len;i++)
	{
		UART_Send_Byte(*(point+i));
	}
}