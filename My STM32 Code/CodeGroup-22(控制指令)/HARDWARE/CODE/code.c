#include "delay.h"
#include "user.h"
#include "code.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "string.h"
#include "sys.h"
#include "hmi_handle.h"
#include "slave.h"


char USART3_RX_BUF[18]={0};   //串口3接收缓存
u16 USART3_RX_STA = 0;               //		最高位为存储使能标志位，为0时允许把收到的数据存到数组，为1不存储

int BCTtoHMITFlag = 0;

char BarCodeBuf[18];




//----------------------------------函数说明--------------------------------------------
//函数功能：	Code的引脚初始化函数
//入口参数：	bound：波特率
//返回值：		无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/09
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
void CodeInit(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //PC.4 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC.4
	GPIO_SetBits(GPIOC,GPIO_Pin_4);						 //PC.4 输出高
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);//使能USART3时钟
	
	//USART1_TX   GPIOB.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.10
   
  //USART1_RX	  GPIOB.11初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB.11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.11

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART3, &USART_InitStructure); //初始化串口3
	
  USART_Cmd(USART3, ENABLE);  //使能串口3
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	
  
}




//----------------------------------函数说明--------------------------------------------
//函数功能：	串口3（Code）对应的中断服务程序
//入口参数：	无
//返回值：		无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/09
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
void USART3_IRQHandler(void)                	//串口3中断服务程序
	{
	u8 Res;
		int t = 0;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
		
		if((USART3_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART3_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART3_RX_STA=0;//接收错误,重新开始
				else 
					USART3_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else
					{
						USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
						USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
	} 

	
	
//----------------------------------函数说明--------------------------------------------
//函数功能：	获取条形码的函数，将串口3的数据传输到条形码数组
//入口参数：	无
//返回值：		无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/13
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
void BarCodeTask(void)
{
	int i;
	if(HMITtoBCTFlag == 1)
	{
		if(BoradSlaveBuf[7]==0x01)
		{
//			BoradSlaveBuf[7]=0x00;
			GPIO_ResetBits(GPIOC, GPIO_Pin_4);
			if(USART3_RX_STA!=0)
			{
				for(i=0;i<17+1;i++)
				{
					BarCodeBuf[i]=USART3_RX_BUF[i];
				}
				BarCodeBuf[17]='0';
				
				if(i==17+1)
				{
					memset(USART3_RX_BUF,0,18);
					USART3_RX_STA=0;
					GPIO_SetBits(GPIOC, GPIO_Pin_4);
					BCTtoHMITFlag = 1;
					
					memset(BoradSlaveBuf,0,sizeof (BoradSlaveBuf));
				}
			}
		}
	}
	else if(HMITtoBCTFlag == 0)
//	else
			GPIO_SetBits(GPIOC, GPIO_Pin_4);
}



		








