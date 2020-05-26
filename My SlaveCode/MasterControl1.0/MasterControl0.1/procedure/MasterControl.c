#include "MasterControl.h"




/****************************RLED1灯控******************************************************/	
void MasterControl()
{

	
/****************************RLED1灯控******************************************************/					
				if(MasterDataBuff[Reled1]==0x01)
				{
					RLED1=ON;  //亮
				}
				else if(MasterDataBuff[Reled1]==0x00)
				{
					RLED1=OFF; //灭
				}
				else
				{
					RLED1=RLED1;//保持原来的状态
				}
				
/*****************************RLED3灯控*****************************************************/				
				if(MasterDataBuff[Reled3]==0x01)
				{
					RLED3=ON;//亮
				}
				else if(MasterDataBuff[Reled3]==0x00)
				{
					RLED3=OFF;//灭
				}
				else
				{
					RLED3=RLED3;//保持原来的状态
				}
/*****************************RLED5灯控*****************************************************/
      if(MasterDataBuff[Reled5]==0x01)
				{
					RLED5=ON;
				}
				else if(MasterDataBuff[Reled5]==0x00)
				{
					RLED5=OFF;
				}
				else
				{
					RLED5=RLED5;//保持原来的状态
				}				
/**********************************************************************************/
			// RLED1=(RX1_Buffer[7]==0x01)?ON:OFF;
			 //RLED3=(RX1_Buffer[8]==0x01)?ON:OFF;//红灯控制
			// RLED5=(RX1_Buffer[9]==0x01)?ON:OFF;
/*****************************GLED1灯控*****************************************************/					
				if(MasterDataBuff[Grled1]==0x01)
				{
					GLED1=ON;	//亮
				}
				else if(MasterDataBuff[Grled1]==0x00)
				{
					GLED1=OFF;
				}
				else
				{
					GLED1=GLED1;
				}
				
/*****************************GLED3灯控*****************************************************/				
				if(MasterDataBuff[Grled3]==0x01)
				{
					GLED3=ON;	//亮
				}
				else if(MasterDataBuff[Grled3]==0x00)
				{
					GLED3=OFF;
				}
				else
				{
					GLED3=GLED3;//保持原来的状态
				}
/*******************************GLED5灯控***************************************************/
      if(MasterDataBuff[Grled5]==0x01)
				{
					GLED5=ON;  //亮
				}
				else if(MasterDataBuff[Grled5]==0x00)
				{
					GLED5=OFF;
				}
				else
				{
					GLED5=GLED5;//保持原来的状态
				}				
/**********************************************************************************/
//			 GLED1=(RX1_Buffer[10]==0x01)?ON:OFF;
//			 GLED3=(RX1_Buffer[11]==0x01)?ON:OFF;//绿灯控制
//			 GLED5=(RX1_Buffer[12]==0x01)?ON:OFF;
///**********************************************************************************/					
//				if(MasterDataBuff[CupOne]==0x01)
//				{
//					Cup1=1;//传感器杯子有
//				}
//				else if(MasterDataBuff[CupOne]==0x00)
//				{
//					Cup1=0;//传感器杯子无
//				}
////				else
////				{
////					Cup1=Cup1;//保持原来的状态
////				}
////				
///**********************************************************************************/				
//				if(MasterDataBuff[CupTwo]==0x01)
//				{
//					Cup3=1;
//				}
//				else if(MasterDataBuff[CupTwo]==0x00)
//				{
//					Cup3=0;
//				}
////				else
////				{
////					Cup3=Cup3;//保持原来的状态
////				}
///**********************************************************************************/
//      if(MasterDataBuff[CupThree]==0x01)
//				{
//					Cup5=1;
//				}
//				else if(MasterDataBuff[CupThree]==0x00)
//				{
//					Cup5=0;
//				}
////				else
////				{
////					Cup5=Cup5;//保持原来的状态
////				}				
///**********************************************************************************/			 
////			 Cup1=(RX1_Buffer[13]==0x01)?1:0;
////			 Cup3=(RX1_Buffer[14]==0x01)?1:0;//杯子有效性1：有效；0：无效
////			 Cup5=(RX1_Buffer[15]==0x01)?
///**********************************************************************************/					
//				if(MasterDataBuff[Water1]==0x01)
//				{
//					Water1Sensor=1;//控制有水流传感器工作
//				}
//				else if(MasterDataBuff[Water1]==0x00)
//				{
//					Water1Sensor=0;//控制无水流传感器停止
//				}
////				else
////				{
////					Water1Sensor=Water1Sensor;//保持原来的状态
////				}
////				
///**********************************************************************************/				
//				if(MasterDataBuff[Water3]==0x01)
//				{
//					Water3Sensor=1;
//				}
//				else if(MasterDataBuff[Water3]==0x00)
//				{
//					Water3Sensor=0;
//				}
////				else
////				{
////					Water3Sensor=Water3Sensor;//保持原来的状态
////				}
///**********************************************************************************/
//      if(MasterDataBuff[Water5]==0x01)
//				{
//					Water5Sensor=1;
//				}
//				else if(MasterDataBuff[Water5]==0x00)
//				{
//					Water5Sensor=0;
//				}
////				else   //这里有个问题 若加此语句Cup就会保持杯子拿走那的电平 导致一处于循环中，此语句还未带传感器是这样
//				{
//					Water5Sensor=Water5Sensor;//保持原来的状态
//				}				
/**********************************************************************************/					
//			 Water1Sensor=(RX1_Buffer[16]==0x01)?1:0;
//			 Water3Sensor=(RX1_Buffer[17]==0x01)?1:0;//传感器数据可以不用控制
//			 Water5Sensor=(RX1_Buffer[18]==0x01)?1:0;
/**********************************************************************************/					
				if(MasterDataBuff[pum1]==0x01)
				{
					Pump1=1;//控制泵启动
				}
				else if(MasterDataBuff[pum1]==0x00)
				{
					Pump1=0;//控制泵关闭
				}
//				else
//				{
//					Pump1=Pump1;//保持原来的状态
//				}
//				
/**********************************************************************************/				
				if(MasterDataBuff[pum3]==0x01)
				{
					Pump3=1;
				}
				else if(MasterDataBuff[pum3]==0x00)
				{
					Pump3=0;
				}
//				else
//				{
//					Pump3=Pump3;//保持原来的状态
//				}
/**********************************************************************************/
      if(MasterDataBuff[pum5]==0x01)
				{
					Pump5=1;
				}
				else if(MasterDataBuff[pum5]==0x00)
				{
					Pump5=0;
				}
//				else
//				{
//					Pump5=Pump5;//保持原来的状态
//				}				
/**********************************************************************************/				 
//			 Pump1=(RX1_Buffer[19]==0x01)?1:0;
//			 Pump3=(RX1_Buffer[20]==0x01)?1:0;//0：水泵关；1：水泵开
//			 Pump5=(RX1_Buffer[21]==0x01)?1:0;
/**********************************************************************************/					
				if(MasterDataBuff[Valve1]==0x01)
				{
					EXValve1=1;//控制继电器空气阀打开
				}
				else if(MasterDataBuff[Valve1]==0x00)
				{
				EXValve1=0;//控制继电器空气阀关闭
				}
				else
				{
					EXValve1=EXValve1;//控制继电器空气阀保持原来的状态
				}
				
/**********************************************************************************/				
				if(MasterDataBuff[Valve3]==0x01)
				{
					EXValve3=1;
				}
				else if(MasterDataBuff[Valve3]==0x00)
				{
					EXValve3=0;
				}
				else
				{
					EXValve3=EXValve3;//保持原来的状态
				}
/**********************************************************************************/
      if(MasterDataBuff[Valve5]==0x01)
				{
					EXValve5=1;
				}
				else if(MasterDataBuff[Valve5]==0x00)
				{
					EXValve5=0;
				}
				else
				{
					EXValve5=EXValve5;//保持原来的状态
				}				
/**********************************************************************************/
//			 EXValve1=(RX1_Buffer[22]==0x01)?1:0;
//			 EXValve3=(RX1_Buffer[23]==0x01)?1:0;//空气阀控制，拍去多余气体
//			 EXValve5=(RX1_Buffer[24]==0x01)?1:0;
/**********************************************************************************/
      if(MasterDataBuff[Sensor]==0x01)
				{
					CapSensor=1;//打开5V水流总开关
				}
				else if(MasterDataBuff[Sensor]==0x00)
				{
					CapSensor=0;//关闭5V水流总开关
				}
				else
				{
					CapSensor=CapSensor;//保持原来的状态
				}				
/**********************************************************************************/			 
//			 CapSensor=(RX1_Buffer[25]==0x01)?1:0;//控制水流5V电源
/**********************************************************************************/
//			if(RX1_Buffer[26]==0x00)
//			Cup1Eff=0；
//			else if（RX1_Buffer[26]==0x01）
//			up1Eff=0；
/**********************************************************************************/
      if(MasterDataBuff[Conf1]==0x01)
				{
					Cup1Eff=1;//有效杯子
				}
				else if(MasterDataBuff[Conf1]==0x00)
				{
					Cup1Eff=0;//无效杯子
				}
//				else
//				{
//					Cup1Eff=Cup1Eff;//保持原来的状态
//				}				
/**********************************************************************************/	
//		 Cup1Eff=(RX1_Buffer[25]==0x01)?1:0;//0x00: Cup1整个功能无效；0x01:整个功能有效；其他情况保持原来状态
/**********************************************************************************/
      if(MasterDataBuff[Conf3]==0x01)
				{
					Cup3Eff=1;//有效杯子
				}
				else if(MasterDataBuff[Conf3]==0x00)
				{
					Cup3Eff=0;//无效杯子
				}
//				else
//				{
//					Cup3Eff=Cup3Eff;//保持原来的状态
//				}				
/**********************************************************************************/
//			 Cup3Eff=(RX1_Buffer[26]==0x01)?1:0;//0x00: Cup3整个功能无效；0x01:整个功能有效；其他情况保持原来状态
/**********************************************************************************/
      if(MasterDataBuff[Conf5]==0x01)
				{
					Cup5Eff=1;//有效杯子
				}
				else if(MasterDataBuff[Conf5]==0x00)
				{
					Cup5Eff=0;//无效杯子
				}
//				else
//				{
//					Cup5Eff=Cup5Eff;//保持原来的状态
//				}				
/**********************************************************************************/
//Cup5Eff=(RX1_Buffer[27]==0x01)?1:0;//0x00: Cup5整个功能无效；0x01:整个功能有效；其他情况保持原来状态

/**********************************************************************************/
				
				
/**********************************************************************************/
//      if(RX1_Buffer[29]==0x01)
//				{
//					ChassisLockChassisLock=1;//锁开
//				}
//				else if(RX1_Buffer[29]==0x00)
//				{
//					CChassisLock=0;//锁闭
//				}
//				else
//				{
//					ChassisLock = ChassisLock;//保持原来的状态
//				}				
/**********************************************************************************/				

				//ChassisLock
//			    SlaveControl();
//						for(i=0;i<=30;i++)
//						{
//							UART_Send_Byte(TX1_Buffer[i]);//打印各个传感器的状态
//						}
			 
		 }

			
	
   
//}

//}


/**********************************************************************************/
/**********************************************************************************/




/**********************************************************************************/
//名称：MasterControlRead()
//功能：从控到主控数据包读剩余杯子次数
//参数：帧头+包长+识别码+功能码(0x58数据流)+数据段+效验码+帧尾  
//返回值：void
//--------------------------------
//作者：XiongXiaoping 	
//修改日期：2020/05/07
//修改内容：
//----------------------------
/**********************************************************************************/
//void MasterControlRead()
//{
//	    u8 y;
//			if(T0_100MS2)
//			{
//				T0_100MS2=0;
//			//	SlaveControlEE();//调用tx发送buff
//				SlaveControl();
//						for(y=0;y<=11;y++)
//						{
//							UART_Send_Byte(TX1_Buffer[y]);	//打印剩余杯子次数
//						}	 
//		 }
//}
/**********************************************************************************/


/**********************************************************************************/
//名称：void MasterControlReadEE()
//功能：从控到主控数据包读从控板状态
//参数：帧头+包长+识别码+功能码(0x57数据流)+数据段+效验码+帧尾  
//    :SlaveControlEE()：发送函数数据调用
//返回值：void
//--------------------------------
//作者：XiongXiaoping 	
//修改日期：2020/05/07
//修改内容：指令下发解析后数据上报
//----------------------------
/**********************************************************************************/
//void MasterControlReadEE()
//{
//	       u8 i;
//			   if(T0_100MS2)
//			   {
//				
//				    T0_100MS2=0;
//					 SlaveControl();//调用tx发送buff
//						for(i=0;i<=30;i++)
//						{
//							UART_Send_Byte(TX1_Buffer[i]);//打印各个传感器的状态
//						}		 
//		  	 }
// }

/**********************************************************************************/
