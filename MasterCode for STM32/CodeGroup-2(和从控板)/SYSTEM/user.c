

#include "user.h"


//
//---------------------函数说明--------------------------------------------//
//函数功能：	清空一个数组，实际上使用0填充数组覆盖原来的数据
//入口参数：	*array:	数组名称
//					len:	需要清空的前多少个数据
//返回值：无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/07
//修改内容：初次完成基本功能
//-------------------------------
void ClearArray(char *array,u8 len)
{
	while(len)
	{
		len--;
		*(array+len) = 0;
	}
}




#define NewLine 0		//发送字符串或数据需要自动添加换行与否

//串口发送字符串函数
void USART_SendString(USART_TypeDef *USARTx,char *str)
{
	u16 k=0;
	while((*(str+k)) != '\0')
	{
		USART_SendData(USARTx,*(str+k));
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
		k++;
	}
	#if NewLine
			USART_SendData(USARTx,'\r');
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
			USART_SendData(USARTx,'\n');
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
	#endif	
}

//串口发送指定字节数据
void USART_SendLenData(USART_TypeDef *USARTx,u8 *DataBuff,u16 Len)
{
	u16 k=0;
	for(k=0;k<Len;k++)
	{
		USART_SendData(USARTx,*(DataBuff+k));
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
	}
	#if NewLine
			USART_SendData(USARTx,'\r');
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
			USART_SendData(USARTx,'\n');
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
	#endif	
}



//---------------------函数说明--------------------------------------------//
//函数功能：	发送一个应答信号
//入口参数：	*USARTx:用哪一个串口发送
//						SendCode：本地发送者编码
//						ReceiveCode：对方接收者编码
//						ReceiveLen：收到的数据长度
//						ParityResult：收到的校验结果
//返回值：	无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/09
//修改内容：初次完成基本功能
//----------------------------
void SendACK(USART_TypeDef *USARTx,u8 SendCode,u8 ReceiveCode,u8 ReceiveLen,u8 ParityResult)
{
	u8 SendStr[4] ={SendCode,ReceiveCode,ReceiveLen,ParityResult,};	
	USART_SendLenData(USARTx,SendStr,12);
}



