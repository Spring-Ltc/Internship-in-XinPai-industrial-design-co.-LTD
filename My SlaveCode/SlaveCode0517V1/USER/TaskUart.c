

#include "TaskUart.h"



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
			MasterDataBuff[MAlimit] = data_H;MasterDataBuff[MAlimit+1] = data_L;//EEPROM任务从这里读取并写入，完成设置
			TaskEEPROMFlag = 1;//标志位置位，EEPROM任务写入完成后清标志位
			break;
		}
		case 0x58://询问剩余限制次数
		{
			//解析完成后，直接发送状态数据区的所有内容，或读取状态数据区只发送次数内容,或TaskUploadFlag置位让While(1)里面的任务发送
			//Send(SlaveDataBuff) 或 Send(LimitNumber)
			TaskUploadFlag = 1;//置位该上传任务
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
			SlaveDataBuff[SAuploadtime] = data_L;//本地保存时间间隔
			MasterDataBuff[MAuploadtime] = data_L;
			//直接修改主动上传数据计数器的阈值,或计数变量阈值读取MasterDataBuff的值
			break;
		}
		default:break;
	}	
}




//--------------------------函数说明--------------------------------------------//
//函数功能：	将数据按照协议打包成帧并发送
//入口参数：	FunctionCode:功能码
//					*DataBuff:需要发送的数据区的内容
//返回值：		无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/16
//修改内容：初次完成基本功能
//----------------------------
//void FramePacking(u8 FunctionCode,u8 *DataBuff)
//{

//	u8 datalen;//该条指令的数据区长度
//	u8 *point = DataBuff;
//	u8 Funcode = FunctionCode;
////	u8 SendStr[MaxSendStr];
//	u8 Parity;
//	u8 i=0;	//数据串下标
//	if(FunctionCode == 0x70)	datalen = 4+2;
//	else if(FunctionCode == 0x71)	datalen = 4+23;
////	u8 SendStr[MaxSendStr] =		//下面只缓存到校验位之前
////										{0xAA,0x55,//数据包头
////									//	datalen,//数据段长度
////										0x21,0x20,//发送和接收方识别码
////										0x00,//是否需要应答
////										Funcode//功能码，由总协议文档规定
////										};
////	for(i=0;i<datalen-4;i++)	//添加数据区							
////				SendStr[i+7] = *(point+i);	
////										
////	Parity = datalen;
////	for(i=3;i<datalen+3;i++)//计算校验数据
////	{
////		Parity   =Parity^SendStr[i];//按位异或运算
////	}
////	SendStr[i]=Parity;i++;//添加校验数据
////	SendStr[i]=0x55;i++;
////	SendStr[i]=0xAA;i++;//添加数据包尾部
//////数据帧打包完毕

////	USART_SendLenData(PortSlave,SendStr,datalen+6);//发送数据包
//}







