
#include "sys.h"
#include "user.h"
#include "tool.h"



//----------------------------------函数说明--------------------------------------------//
//函数功能：	将数据按照协议打包成一个帧
//入口参数：	len:数据区长度，不算身份识别码和功能码
//						localID：本地发送者的编码
//						destinationID：远程接收者的编码
//						IfAck：是否需要应答信号
//						FunctionCode：功能码
//						*Data：需要打包的数据区的地址
//						*Packed：用来存储打包结果的地址
//返回值：	无
//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/20
//修改内容：初次完成基本功能
//----------------------------
void DataPacking(u8 len, u8 localID,u8 destinationID,u8 IfAck,u8 FunctionCode,u8 *Data,u8 *Packed)
{
	u8 parity=len+4;
	u16 i=0;
	*(Packed+i) = 0xAA;i++;      					//添加包头AA
	*(Packed+i) = 0x55;i++;      					//添加包头55
	*(Packed+i) = len+4;i++;       					//添加长度len
	*(Packed+i) = localID;i++;   					//添加本地识别码
	*(Packed+i) = destinationID;i++;      //添加远程接收者识别码
	*(Packed+i) = IfAck;i++;              //添加功能码1，是否需要应答
	*(Packed+i) = FunctionCode;i++;				//添加功能码

	for(i=0;i<len;i++)										//存储数据	
		*(Packed+7+i) = *(Data+i);					//保存数据
	for(i=3;i<7+len;i++)
		parity = parity ^ *(Packed+i);			//计算校验和
	
	i=len+7;
	*(Packed+i) = parity;i++;							//添加校验和
	*(Packed+i) = 0x55;i++;								//添加包尾55
	*(Packed+i) = 0xAA;i++;								//添加包尾AA
}








//--------------------------函数说明--------------------------------------------//
//函数功能：	完成对数据帧的校验
//入口参数：	*DataFrame:待校验的数据帧地址
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







//---------------------------------函数说明--------------------------------------------//
//函数功能：	发送一个应答信号(校验成功后才发送应答信号)
//入口参数：	*USARTx:用哪一个串口发送
//				AckSource：指示应答码的发送方，即由谁发送应答码
//				*DATaFrame:收到的的数据帧地址（应答信号需要提取收到的数据帧里面的内容）
//返回值：	无
//---------------------------------函数测试---------------------------------------------//
//text1:生产商服务器收到数据发送应答
//		收到数据：AA 55 07 20 00 01 58 FF FF FF 81 55 AA			生产商收到设备发来的数据
//		发送应答：AA 55 06 00 20 00 03 07 81 A3 55 AA 				生产商发送应答给设备，应答功能码为0x03
//text2：用户服务器收到数据发送应答
//		收到数据：AA 55 07 20 01 01 58 FF FF FF 80 55 AA			用户收到设备发来的数据
//		发送应答：AA 55 06 01 20 00 A7 07 80 07 55 AA 				用户发送应答给设备，应答功能码为0xA7
//text3：设备收到数据发送应答
//		收到数据1：AA 55 07 00 20 01 58 FF FF FF 81 55 AA		设备收到生产商发来的数据
//		发送应答1：AA 55 06 20 00 00 72 07 81 D2 55 AA 			设备发送应答给生产商，应答功能码为0x72
//		收到数据2：AA 55 07 01 20 01 58 FF FF FF 80 55 AA		设备收到用户发来的数据
//		发送应答2：AA 55 06 20 01 00 72 07 80 D2 55 AA 			设备发送应答给用户，应答功能码为0x72
//-------------------------------------------------------------------------------------//
//作者：Liao_tingchun		
//修改日期：2020/05/16
//修改内容：初次完成基本功能
//----------------------------
void SendACK(USART_TypeDef *USARTx,u8 AckSource,u8 *DataFrame)
{
	u8 datalen = 0x06;//该条指令的数据区长度
	u8 AckType;
	u8 *point= DataFrame;
	u8 i=0;	//数据串下标
	if(AckSource == 0x00)		AckType = 0x03;
	else if(AckSource == 0x01)		AckType = 0xA7;
	else if(AckSource == 0x20)		AckType = 0x72;
	u8 SendStr[14] =		//下面只缓存到校验位之前
										{0xAA,0x55,//数据包头
										datalen,//数据段长度
										AckSource,*(point+3),//发送和接收方识别码，
										0x00,//是否需要应答【固定不需要应答】
										AckType,//应答功能码，由总协议文档规定
										*(point+2),*(point+(*(point+2))+3)
										};
	u8 Parity = datalen;
	for(i=3;i<datalen+3;i++)//计算校验数据
		Parity   =Parity^SendStr[i];//按位异或运算
	SendStr[i]=Parity;i++;//添加校验数据
	SendStr[i]=0x55;i++;
	SendStr[i]=0xAA;i++;//添加数据包尾部
//数据帧打包完毕
	USART_SendLenData(USARTx,SendStr,datalen+6);//发送数据包
}

