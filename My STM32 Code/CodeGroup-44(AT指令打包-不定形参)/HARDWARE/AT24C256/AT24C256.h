
#ifndef __AT24C256_H__
#define __AT24C256_H__

#include "sys.h"
#include "delay.h"

//IO方向设置
//#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=8<<28;}
//#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=3<<28;}
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0x80000000;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0x30000000;}

//IO操作函数	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //输入SDA 

void AT24C256Init(void);


//AT24C256相关
void WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite);//指定地址写入一个字节
uint8_t ReadOneByte(uint16_t ReadAddr);//指定地址读取一个字节
void WriteLenByte(uint16_t StartAddr,uint8_t *WriteBuffer,uint8_t Len);//指定起始地址写入指定长度个字节数据
void ReadLenByte(uint16_t StartAddr,uint8_t *ReadBuffer,uint8_t Len);//指定起始地址读出指定长度个字节的数据



//IIC相关

void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	


//每项数据按256字节存储，最多可以存储128项，
//下面按256字节分割EEPROM地址
#define DataAddress0	256*0
#define DataAddress1  256*1
#define DataAddress2	256*2
#define DataAddress3  256*3
#define DataAddress4	256*4
#define DataAddress5  256*5


#define AddUSRK6  256*7


#endif




/*   部分函数使用示例

	for(i=0;i<10;i++)	//给数据数组赋值
		databuff1[i]=i+'0';
	for(i=0;i<10;i++)
		databuff2[i]=i+'A';
	for(i=0;i<10;i++)
		databuff3[i]=i+'a';	
	
		WriteLenByte(DataAddress0,databuff1,100);
		WriteLenByte(DataAddress1,databuff2,100);
		WriteLenByte(DataAddress2,databuff3,100);	//写入数据
		LED1_ON;//提示写入完毕
		
		ReadLenByte(DataAddress0,readbuff1,50);
		ReadLenByte(DataAddress1,readbuff2,50);
		ReadLenByte(DataAddress2,readbuff3,50);
		LED2_ON;//提示读取完毕
		
		USART_SendLenData(USART1,readbuff1,10);
		USART_SendLenData(USART1,readbuff2,10);
		USART_SendLenData(USART1,readbuff3,10);





*/












