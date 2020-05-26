

#include "USRK6.h"
#include "usart.h"
#include "delay.h"
#include "user.h"


//---------------------函数说明--------------------------------------------//
//函数功能：	以太网模块初始化，这里初始化串口
//					使用串口5，RX引脚PD2；TX引脚PC12
//入口参数：	bound:以太网模块所用的串口波特率
//返回值：	无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/07
//修改内容：初次完成基本功能
//----------------------------
void USRK6Init(u32 bound)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
	
	//UART5_TX   GPIOC.12
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure);
   
  //UART5_RX	  GPIOD.2初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOD, &GPIO_InitStructure);  

   //UART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(UART5, &USART_InitStructure); //初始化串口5
	
  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(UART5, ENABLE);                    //使能串口5
}





	//串口5中断服务程序
//接收以太网模块返回的配置提示信息
char UART5_RX_BUF[MaxRx5Len]={0};//串口5接收缓存
u16 UART5_RX_STA = 0;
//		最高位为存储使能标志位，为0时允许把收到的数据存到数组，为1不存储
void UART5_IRQHandler(void)                
{
	u8 Res;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(UART5);	//读取接收到的数据
		
//		if((UART5_RX_STA&0x8000)==0)//允许接收并存储到数组
//		{
			UART5_RX_BUF[UART5_RX_STA&0X3FFF]=Res ;
			UART5_RX_STA++;	
//			if((UART5_RX_STA  & 0X3FFF) > MaxRx5Len-1)//异常接收太多数据，重新开始接收
//				UART5_RX_STA = 0;
//		 }		 	 
	 } 
} 




//---------------------函数说明--------------------------------------------//
//函数功能：	运行该函数让以太网模块进入AT模式
//入口参数：无
//返回值：	标志模块是否进入AT模式，进入成功返回0，失败返回1

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/07
//修改内容：初次完成基本功能
//--------------------------------
u8 EnterAT(void)
{
	u8 time = 10;//超过该循环没收到应答也退出
	ClearArray(UART5_RX_BUF,MaxRx5Len);//先清空
	UART5_RX_STA=0;//允许接收
	USART_SendString(UART5,"+++");
	while(--time)//超时判断
	{
		delay_ms(5);//这个时间应该接收完一个字符了吧？
		if(UART5_RX_BUF[0]=='a' && UART5_RX_STA==1)//只接收到一个字符为‘a’
			break;
	}
	if(time==0)	 return 1;//超过指定时间没有收到回复

	ClearArray(UART5_RX_BUF,MaxRx5Len);//先清空
	UART5_RX_STA=0;//允许接收
	USART_SendString(UART5,"a");
	while(--time)//超时判断
	{
		delay_ms(5);//这个时间应该接收完一个字符了吧？
		if(UART5_RX_STA >= 2)//接收到‘+ok’
		{
			if((UART5_RX_BUF[0]=='+')&&
				(UART5_RX_BUF[1]=='o')&&
				(UART5_RX_BUF[2]=='k'))		return 0;//成功进入AT模式
			else break;
		}
	}
	return 1;//超过指定时间没有收到回复
}



//---------------------函数说明--------------------------------------------//
//函数功能：	运行该函数让以太网模块退出AT模式
//入口参数：无
//返回值：	是否退出AT模式成功，0表示成功；其他表示失败，其他对应错误类型

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------
u8 ExitAT(void )
{
	char AckBuff[15]="0";
	u8 status=0;
	ClearArray(AckBuff,15);
	status = ATControl("AT+ENTM\r\n",AckBuff);
	if(status) return 6;//错误类型6：没有收到应答

	if((AckBuff[2]=='+')&&
		(AckBuff[3]=='O')&&
		(AckBuff[4]=='K'))		return 0;//成功退出AT模式

	else return AckBuff[8];//返回手册上定义的错误类型
}

//---------------------函数说明--------------------------------------------//
//函数功能：	重启以太网模块
//入口参数：无
//返回值：	是否重启成功，0表示成功；其他表示失败，其他对应错误类型

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------
u8 Reboot(void)
{
	char AckBuff[15]="0";
	u8 status=0;
	status = ATControl("AT+Z\r\n",AckBuff);
	if(status) return 6;//错误类型6：没有收到应答

	if((AckBuff[2]=='+')&&
		(AckBuff[3]=='O')&&
		(AckBuff[4]=='K'))		return 0;//成功

	else return AckBuff[8];//返回手册上定义的错误类型
}



//---------------------函数说明--------------------------------------------//
//函数功能：	以太网模块恢复出厂设置
//入口参数：无
//返回值：	是否成功，0表示成功；其他表示失败，其他对应错误类型

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//--------------------------------
u8 ManufacturerReset(void)
{
	char AckBuff[15]="0";
	u8 status=0;
	status = ATControl("AT+RELD\r\n",AckBuff);
	if(status) return 6;//错误类型6：没有收到应答

	if((AckBuff[2]=='+')&&
		(AckBuff[3]=='O')&&
		(AckBuff[4]=='K'))		return 0;//成功

	else return AckBuff[8];//返回手册上定义的错误类型
}




	
//u8 ConfigWEBU(char *UserName,char *Password)
//u8 ConfigDNS(char *Address)
//u8 ConfigWEBPORT(char *Port)
//u8 ConfigTCPSE(char *Status)
//u8 ConfigSOCKPORT(char *Sta)
//u8 ConfigRFCEN(char *Status)
//u8 ConfigREGEN(char *Status)
//u8 ConfigREGTCP(char *Status)
//u8 ConfigREGCLOUD(char *Name,char *Password)
//u8 ConfigREGUSR(char *Data)
//u8 ConfigREGUSRHEX(char *Data)
//u8 ConfigHTPTP(char *Status)
//u8 ConfigHTPURL(char *URL)
//u8 ConfigHTPHEAD(char *Data)
//u8 ConfigHTPEAD(char *Data)
//u8 ConfigHTPCHD(char *Sta)
//u8 ConfigHEARTEN(char *Status)
//u8 ConfigHEARTTP(char *Type)
//u8 ConfigHEARTTM(char *Time)
//u8 ConfigHEARTDT(char *Data)
//u8 ConfigHEARTDTHEX(char *Data)
//u8 ConfigSCSLINK(char *Status)
//u8 ConfigCLIENTRST(char *Sta)
//u8 ConfigINDEXEN(char *Sta)
//u8 ConfigSOCKSL(char *Sta)
//u8 ConfigSHORTO(char *Sta)
//u8 ConfigUARTCLBUF(char *Sta)
//u8 ConfigRSTIM(char *Time)
//u8 ConfigMAXSK(char *Num)
//u8 ConfigMID(char *Name)




//---------------------函数说明--------------------------------------------//
//函数功能：	通过串口5向以太网模块USR-K6发送AT指令配置模块参数
//				未收到应答或收到模块的返回提示信息与期望值不符，均表示该条AT指令没有发挥作用
//入口参数：	*str:	发送的信息字符串保存地址
//					*ack:	发送配置信息后模块返回的提示信息存储地址
//返回值：	标志模块是否收到应答，0表示收到；1表示没有收到

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/07
//修改内容：初次完成基本功能
//----------------------------
u8 ATControl(char *str,char *ack)
{
	u8 time=10;//超过该循环没收到应答也退出,这个时间不要太小了哦
	u8 i=0;
	ClearArray(UART5_RX_BUF,MaxRx5Len);//先清空接收区
	UART5_RX_STA=0;//允许接收
	USART_SendString(UART5,str);//发送指令信息
	while(--time)//超时判断
	{
		delay_ms(10);//这个时间应该接收完应答了吧？
		if(UART5_RX_STA > 4)//接收到应答信息，开始判断结束标志符
		{
			break;
//			if(UART5_RX_BUF[UART5_RX_STA]=='\n' && UART5_RX_BUF[UART5_RX_STA-1]=='\r')//收到末尾的换行符
//			{	
//				UART5_RX_STA|= 0x8000;//把最高位置1，等待处理完再存储接收其他数据
//				break;//成功收完应答信息，提前退出
//			}
		}
	}
	if(time == 0) return 1;//规定时间未收到应答
	
	for(i=0;i<UART5_RX_STA;i++)//存储应答信息数据
		*(ack+i) = UART5_RX_BUF[i];
	
	return 0;
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
	char command[MaxCommandLen]="AT+";
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
	
	status = ATControl(command,Respond);//发送AT指令，并接收模块的应答信息
	if(status) 
		return 1;//没有收到应答
	return 0;
}



//---------------------函数说明--------------------------------------------//
//函数功能：	配置WAN口参数
//备注：所有入口参数必须是字符串的形式，要以'\0'作为结束标志符
//入口参数：		*Mode:	网络IP模式，STATIC:静态IP；DHCP:动态IP(后面三个参数省略)
//						*Address:	静态IP地址（比如192.168.1.117）
//						*Mask:子网掩码（比如255.255.255.0）
//						*Gateway:网关（比如192.168.0.1）
//返回值：	是否配置成功，0表示配置成功；其他表示失败，其他对应错误类型

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//----------------------------



u8 ConfigWANN(char *Mode,char *Address,char *Mask,char *Gateway)
{
	char command[MaxCommandLen]="AT+WANN=";
	char AckBuff[15]="0";
	u8 count=0;//统计整条指令字符数
	u8 i=0;//单项参数循环
	u8 status = 0;//返回指令运行状态
	
	u8 len=0;//整条指令字符数
	len = ATComPacking(command,"WANN",Mode,Address,Mask,Gateway,"#");
	
	status = ATControl(command,AckBuff);//发送AT指令，并接收模块的应答信息
	if(status) return 6;//错误类型6：没有收到应答

	if((AckBuff[2]=='+')&&
		(AckBuff[3]=='O')&&
		(AckBuff[4]=='K'))		return 0;//配置成功

	else return AckBuff[8];//返回手册上定义的错误类型
}
//Function OK


//---------------------函数说明--------------------------------------------//
//函数功能：	配置以太网模块的串口参数
//入口参数：	*BaudRate:	波特率（600到460800自定义）
//						*DataBits:	数据位（只能设置为7或8）
//						*StopBits:停止位（只能设置为1或2）
//						*Parity:校验位，NONE:无检验位；EVEN:偶校验；ODD:奇校验；MASK:1校验；SPACE:0校验
//						*Flowctrl：流控设置，NFC:无硬件流控；FCH：有硬件流控；FCR:软件流控。
//											（设备模块限制只能设置为NFC）
//返回值：	是否配置成功，0表示配置成功；其他表示失败，其他对应错误类型

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//----------------------------
u8 ConfigUART(char *BaudRate,char *DataBits,char *StopBits,char *Parity,char *Flowctrl)
{
	char command[MaxCommandLen]="AT+UART=";
	char AckBuff[15]="0";
	u8 count=0;//统计整条指令字符数
	u8 i=0;//单项参数循环
	u8 status = 0;//返回指令运行状态
	
	u8 len=0;//整条指令字符数
	len = ATComPacking(command,"UART",BaudRate,DataBits,StopBits,Parity,Flowctrl,"#");
	
	status = ATControl(command,AckBuff);//发送AT指令，并接收模块的应答信息
	if(status) return 6;//错误类型6：没有收到应答

	if((AckBuff[2]=='+')&&
		(AckBuff[3]=='O')&&
		(AckBuff[4]=='K'))		return 0;//配置成功

	else return AckBuff[8];//返回手册上定义的错误类型
}
//Function OK


//---------------------函数说明--------------------------------------------//
//函数功能：	配置SOCK参数
//入口参数：	*Protocol:	模块协议类型(TCPC、TCPS、UDPC、UDPS、HTPC五选一)
//						*IP:	当模块被设置为”CLIENT”时，目标服务器的 IP 地址或域名（比如192.168.1.117）
//						*Port:式下为本地端口号，Client 模式下为远程端口号，10 进制数，小于 6535
//返回值：	是否配置成功，0表示配置成功；其他表示失败，其他对应错误类型

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/08
//修改内容：初次完成基本功能
//----------------------------
u8 ConfigSOCK(char *Protocol,char *IP,char *Port)
{
	char command[MaxCommandLen]={0};//定义一个需要发送的AT指令字符串
	char AckBuff[15]="0";//发送指令后模块返回的数据

	u8 status = 0;//返回指令运行状态
	
	u8 len=0;//整条指令字符数
	len = ATComPacking(command,"SOCK",Protocol,IP,Port,"#");
	
	status = ATControl(command,AckBuff);//发送AT指令，并接收模块的应答信息
	if(status) return 6;//错误类型6：没有收到应答

	if((AckBuff[2]=='+')&&
		(AckBuff[3]=='O')&&
		(AckBuff[4]=='K'))		return 0;//配置成功

	else return AckBuff[8];//返回手册上定义的错误类型
}
//Function OK


