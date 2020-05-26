
#ifndef __AT24C256_H__
#define __AT24C256_H__

#include "sys.h"
#include "delay.h"

//IO��������
//#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=8<<28;}
//#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=3<<28;}
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0x80000000;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0x30000000;}

//IO��������	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //����SDA 

void AT24C256Init(void);


//AT24C256���
void WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite);//ָ����ַд��һ���ֽ�
uint8_t ReadOneByte(uint16_t ReadAddr);//ָ����ַ��ȡһ���ֽ�
void WriteLenByte(uint16_t StartAddr,uint8_t *WriteBuffer,uint8_t Len);//ָ����ʼ��ַд��ָ�����ȸ��ֽ�����
void ReadLenByte(uint16_t StartAddr,uint8_t *ReadBuffer,uint8_t Len);//ָ����ʼ��ַ����ָ�����ȸ��ֽڵ�����



//IIC���

void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	


//ÿ�����ݰ�256�ֽڴ洢�������Դ洢128�
//���水256�ֽڷָ�EEPROM��ַ
#define DataAddress0	256*0
#define DataAddress1  256*1
#define DataAddress2	256*2
#define DataAddress3  256*3
#define DataAddress4	256*4
#define DataAddress5  256*5


#define AddUSRK6  256*7


#endif




/*   ���ֺ���ʹ��ʾ��

	for(i=0;i<10;i++)	//���������鸳ֵ
		databuff1[i]=i+'0';
	for(i=0;i<10;i++)
		databuff2[i]=i+'A';
	for(i=0;i<10;i++)
		databuff3[i]=i+'a';	
	
		WriteLenByte(DataAddress0,databuff1,100);
		WriteLenByte(DataAddress1,databuff2,100);
		WriteLenByte(DataAddress2,databuff3,100);	//д������
		LED1_ON;//��ʾд�����
		
		ReadLenByte(DataAddress0,readbuff1,50);
		ReadLenByte(DataAddress1,readbuff2,50);
		ReadLenByte(DataAddress2,readbuff3,50);
		LED2_ON;//��ʾ��ȡ���
		
		USART_SendLenData(USART1,readbuff1,10);
		USART_SendLenData(USART1,readbuff2,10);
		USART_SendLenData(USART1,readbuff3,10);





*/












