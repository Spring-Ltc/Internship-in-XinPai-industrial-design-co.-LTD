#include "SlaveControl.h"




/**********************************************************************************/
//名称：void void SlaveControl()
//功能：从控到主控数据包/上传
//参数：帧头+包长+识别码+功能码(0x71数据流)+数据段+效验码+帧尾
//       TX1_Buffer：发送缓冲数组
//       RLED：条件判断将值存入发送缓冲数组
//返回值：void
//--------------------------------
//作者：XiongXiaoping 	
//修改日期：2020/05/07
//修改内容：数据采集存入发送缓冲数组
//----------------------------
/**********************************************************************************/
u8 Buffer[12];
//void SlaveControl()
//{
//    TX1_Buffer[0]=0xAA;  //帧头
//    TX1_Buffer[1]=0x55;  //帧头
//    
//    TX1_Buffer[2]=0x17;   //数据长度
//			
//		TX1_Buffer[3]=0x20;    //设备发送设备
//		TX1_Buffer[4]=0x21;    //本地设备发送给设备
//			
//		TX1_Buffer[5]=0x00;    //未要求应答
//    TX1_Buffer[6]=0x71;    //数据流方向
//			
//		
//			
//    TX1_Buffer[7]=(RLED1==ON)?0x01:0x00;
//    TX1_Buffer[8]=(RLED3==ON)?0x01:0x00;	//红灯状态
//		TX1_Buffer[9]=(RLED5==ON)?0x01:0x00;
//		
//    TX1_Buffer[10]=(GLED1==ON)?0x01:0x00;
//    TX1_Buffer[11]=(GLED3==ON)?0x01:0x00;	//绿灯状态
//		TX1_Buffer[12]=(GLED5==ON)?0x01:0x00;
//		
//    TX1_Buffer[13]=(Cup1==1)?0x01:0x00;
//    TX1_Buffer[14]=(Cup3==1)?0x01:0x00;	//水杯有无状态
//		TX1_Buffer[15]=(Cup5==1)?0x01:0x00;
//    
//		TX1_Buffer[16]=(Water1Sensor==1)?0x01:0x00;
//    TX1_Buffer[17]=(Water3Sensor==1)?0x01:0x00;	//无有水检测
//		TX1_Buffer[18]=(Water5Sensor==1)?0x01:0x00;
//   
//    TX1_Buffer[19]=(Pump1==1)?0x01:0x00;
//    TX1_Buffer[20]=(Pump3==1)?0x01:0x00;	//泵状态检测
//		TX1_Buffer[21]=(Pump5==1)?0x01:0x00;
//    
//    TX1_Buffer[22]=(EXValve1==1)?0x01:0x00;
//    TX1_Buffer[23]=(EXValve3==1)?0x01:0x00;	//空气阀状态检测
//		TX1_Buffer[24]=(EXValve5==1)?0x01:0x00;

//    TX1_Buffer[25]=(CapSensor==1)?0x01:0x00;	//5V电压控制
//		TX1_Buffer[26]=0x00;
//    //TX1_Buffer[26]=(ChassisLock==1)?0x01:0x00;	//机箱锁状态
//		if(RX1_Buffer[6]==0X73)
//		{
//		TX1_Buffer[27]=RX1_Buffer[8];	//5V电压控制
//		}
//		else
//		{
//			TX1_Buffer[27]=0x00;
//		}
//    TX1_Buffer[28]=CheckSumTxd(TX1_Buffer,25);//校验和
//		//TX1_Buffer[26]=0x00;	//暂时为效验码    杯子使用次数计数
//		
//		TX1_Buffer[29]=0x55;
//		TX1_Buffer[30]=0xAA;	//帧尾
//}
/**********************************************************************************/
/**********************************************************************************/


/**********************************************************************************/
//名称：void SlaveControlEE()
//功能：上报EEPROM剩余杯子的次数
//参数：帧头+包长+识别码+功能码(0x70数据流)+数据段+效验码+帧尾
//      
//返回值：void
//--------------------------------
//作者：XiongXiaoping 	
//修改日期：2020/05/07
//修改内容：查询剩余杯子数
//----------------------------
/**********************************************************************************/
void SlaveControlEE()
{
	

		Buffer[0]=0xAA;  //帧头
    Buffer[1]=0x55;  //帧头
    
    Buffer[2]=0x06;   //数据长度
			
		Buffer[3]=0x20;    //设备发给设备
    Buffer[4]=0x21;    //本地设备发给设备
			
		Buffer[5]=0x00;    //未要求应答
    Buffer[6]=0x70;    //数据流方向
	
	
		Buffer[7]=EEPROM_read_n(0x0001);
		Buffer[8]=EEPROM_read_n(0x0000);	//  读取杯子剩余次数
		
	//  Buffer[9]= CheckSumTxd(TX1_Buffer,0x08);
	//	TX1_Buffer[9]=0x00;	//暂时为效验码   
	
		Buffer[10]=0x55;
		Buffer[11]=0xAA;	//帧尾
	
}	
/**********************************************************************************/


