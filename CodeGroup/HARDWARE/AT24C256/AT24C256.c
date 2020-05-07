#include "AT24C256.h"

//��ʼ��IIC��ص�IO��
void AT24C256Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	IIC_SCL=1;
	IIC_SDA=1;
}


//ָ����ַд��һ���ֽ�
//��ڲ�����	WriteAddr:��Ҫд��ĵ�ַ
//					DataToWrite:��Ҫд�������
void WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{
	IIC_Start();  
	
	IIC_Send_Byte(0XA0);	    //����д����
	IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
		IIC_Wait_Ack();	   
  IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
		IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
		IIC_Wait_Ack();  	
	
  IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);
}


//ָ����ַ��ȡһ���ֽ�
//��ڲ�����	ReadAddr:��Ҫ��ȡ�ĵ�ַ
//����ֵ��		��ȡ��������
uint8_t ReadOneByte(uint16_t ReadAddr)
{
	uint8_t temp=0;	
	
  IIC_Start(); 
	
	IIC_Send_Byte(0XA0);	   //����д����
	IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
		IIC_Wait_Ack();		  
  IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	
	
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	
	
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();//����һ��ֹͣ����
	
	return temp;
}


//ָ����ʼ��ַд��ָ�����ȸ��ֽ�����
//��ڲ�����	StarAddr:��ʼ��ַ
//					*WriteBuffer:��Ҫд������ݵĴ�ŵ�ַ
//					Len:��Ҫд����ֽ���
void WriteLenByte(uint16_t StartAddr,uint8_t *WriteBuffer,uint8_t Len)
{
	while(Len--)
	{
		WriteOneByte(StartAddr,*WriteBuffer);
		StartAddr++;
		WriteBuffer++;
	}
}


//ָ����ʼ��ַ����ָ�����ȸ��ֽڵ�����
//��ڲ�����	StarAddr:��ʼ��ַ
//					*WriteBuffer:��Ҫ���������ݵĴ����ʼ��ַ
//					Len:��Ҫ�������ֽ���
void ReadLenByte(uint16_t StartAddr,uint8_t *ReadBuffer,uint8_t Len)
{
	while(Len)
	{
		*ReadBuffer++=ReadOneByte(StartAddr++);	
		Len--;
	}
}



//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA = 1;	  	  
	IIC_SCL = 1;
	delay_us(4);
 	IIC_SDA = 0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL = 0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL = 0;
	IIC_SDA = 0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL = 1; 
	IIC_SDA = 1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
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
	IIC_SCL = 0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL = 0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
			IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL = 1;
		delay_us(2); 
		IIC_SCL = 0;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}


