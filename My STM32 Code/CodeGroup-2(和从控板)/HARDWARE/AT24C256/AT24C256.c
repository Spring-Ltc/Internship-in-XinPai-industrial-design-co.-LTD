#include "AT24C256.h"

//初始化IIC相关的IO口

//---------------------函数说明--------------------------//
//函数功能：	初始化AT24C256芯片，即IIC用到的引脚
//						SDA引脚为PB7；SCL引脚为PB6
//入口参数：	无
//返回值：无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/07
//修改内容：初次完成基本功能
//--------------------------------
void AT24C256Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	IIC_SCL=1;
	IIC_SDA=1;
}


//---------------------函数说明--------------------------//
//函数功能：	指定AT24C256的地址写入一个字节数据
//入口参数：	WriteAddr:	要写入的地址
//						DataToWrite：要写入的数据
//返回值：无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/07
//修改内容：初次完成基本功能
//--------------------------------
void WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{
	IIC_Start();  
	
	IIC_Send_Byte(0XA0);	    //发送写命令
	IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
		IIC_Wait_Ack();	   
  IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
		IIC_Send_Byte(DataToWrite);     //发送字节							   
		IIC_Wait_Ack();  	
	
  IIC_Stop();//产生一个停止条件 
	delay_ms(10);
}



//---------------------函数说明--------------------------//
//函数功能：	指定AT24C256的地址读取一个字节数据
//入口参数：	ReadAddr:	要读取的地址
//返回值：	读取的结果

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/07
//修改内容：初次完成基本功能
//--------------------------------
uint8_t ReadOneByte(uint16_t ReadAddr)
{
	uint8_t temp=0;	
	
  IIC_Start(); 
	
	IIC_Send_Byte(0XA0);	   //发送写命令
	IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址
		IIC_Wait_Ack();		  
  IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	
	
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	
	
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();//产生一个停止条件
	
	return temp;
}




//---------------------函数说明--------------------------------------------//
//函数功能：	指定起始地址写入指定长度个字节数据到AT24C256存储器
//入口参数：	StarAddr:	写入AT24C56的起始地址
//					*WriteBuffer:	需要写入的数据的存放地址（数据来源）
//					Len:	需要写入的字节数
//返回值：	无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/07
//修改内容：初次完成基本功能
//--------------------------------

 void WriteLenByte(uint16_t StartAddr,uint8_t *WriteBuffer,uint8_t Len)
{
	while(Len--)
	{
		WriteOneByte(StartAddr,*WriteBuffer);
		StartAddr++;
		WriteBuffer++;
	}
}


//---------------------函数说明--------------------------------------------//
//函数功能：指定AT24C256的起始地址读取指定长度个字节数据
//入口参数：StarAddr:起始地址
//					*WriteBuffer:需要读出的数据的存放起始地址
//					Len:需要读出的字节数
//返回值：	无

//--------------------------------
//作者：Liao_tingchun		
//修改日期：2020/05/07
//修改内容：初次完成基本功能
//--------------------------------
void ReadLenByte(uint16_t StartAddr,uint8_t *ReadBuffer,uint8_t Len)
{
	while(Len)
	{
		*ReadBuffer++=ReadOneByte(StartAddr++);	
		Len--;
	}
}



//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA = 1;	  	  
	IIC_SCL = 1;
	delay_us(4);
 	IIC_SDA = 0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL = 0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL = 0;
	IIC_SDA = 0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL = 1; 
	IIC_SDA = 1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA = 1;delay_us(1);	   
	IIC_SCL = 1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL = 0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 0;
	delay_us(2);
	IIC_SCL = 1;
	delay_us(2);
	IIC_SCL = 0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 1;
	delay_us(2);
	IIC_SCL = 1;
	delay_us(2);
	IIC_SCL = 0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL = 0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
			IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL = 1;
		delay_us(2); 
		IIC_SCL = 0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL = 0; 
        delay_us(2);
		IIC_SCL = 1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}



