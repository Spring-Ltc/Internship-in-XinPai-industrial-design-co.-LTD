


#include "CupDet.h"

//extern	u8 TX1_Buffer[32];//存储要上传的数据ScanCounter1
u8 Cup1Eff=0;//接收杯子有效 缓冲区
u8 Cup3Eff=0; //接收杯子有效 缓冲区//接收杯子有效 缓冲区
u8 Cup5Eff=0; //接收杯子有效 缓冲区

u8  Timer10MSCounter11=0,Timer10MSCounter12=0,Timer10MSCounter13=0,Timer10MSCounter14=0,Timer10MSCounter15=0;  //计数10MS标志
u8  TimerOut=0,TimerOut1=0,TimerOut2=0,TimerOut3=0,TimerOut4=0,TimerOut5=0;//定时输出
u16 ScanCounter1, ScanCounter2, ScanCounter3, ScanCounter4, ScanCounter5;ExvalveCounter;//传感器标志|计数器

u8 ExitData1=1;ExitData2=1;ExitData3=1;ExitData4=1;ExitData5=1;//标志

u8 MaxDrawingWaterTimes=120;//最长抽水时间120 
u8 CupOffDrawinWaterTimes=6; //杯子离开后抽水时间 100MS

u8 Cup1Status   =0;//1无杯子
u8 Cup5Status   =0;//5无杯子
u8 Cup3Status   =0;//3无杯子

u8 Cup1Waiting=0,Cup1Runing=0,Cup1Ending=0;Cup1EndingDraw=0;//状态判断/运行/结束/结束标志
u8 Cup5Waiting=0,Cup5Runing=0,Cup5Ending=0;Cup5EndingDraw=0;
u8 Cup3Waiting=0,Cup3Runing=0,Cup3Ending=0;Cup3EndingDraw=0;
u16 SurplusCount;//杯子在本次开机运行时对对杯子使用计数

void  CupDetconfig(void)  //自检初始化
   {
	 
				 //自检
				 Pump1=VALVEOFF;// OFF PMUP
				 Pump5=VALVEOFF;// OFF PMUP	
				 Pump3=VALVEOFF;// OFF PMUP
				 EXValve1=VALVEOFF;
				 EXValve5=VALVEOFF;
				 EXValve3=VALVEOFF;  
				 SPARE=VALVEOFF;
				 CapSensor=VALVEOFF; //供电5V关


			/*******************泵自检  水流阀方式*********************************/

			 Pump1=VALVEON;//开PMUP1
			 Pump5=VALVEON;//开PMUP2
			 Pump3=VALVEON;//开PMUP3
			 Delay1000ms();
			 Delay1000ms();
			 EXValve1=VALVEON;
			 EXValve5=VALVEON;
			 EXValve3=VALVEON;
			 Delay1000ms(); 
			 Delay1000ms();
			 Delay1000ms(); 

			 Delay50ms();
			 Pump1=VALVEOFF;// OFF PMUP
			 Pump5=VALVEOFF;// OFF PMUP	
			 Pump3=VALVEOFF;// OFF PMUP
			 EXValve1=VALVEOFF;
			 EXValve5=VALVEOFF;
			 EXValve3=VALVEOFF;
			 
			 
			 Delay1000ms();
			 CapSensor=VALVEON; //供电5V关
			 
			 

			Cup1Waiting=0;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;//状态判断/运行/结束/结束标志|赋值
			Cup5Waiting=0;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=0;
			Cup3Waiting=0;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=0;
	 
	   
	 
	 }
	 
	 
void PumpControl()
{
		if(Cup1Status==1)//有杯子状态确定是否开启泵
							{ 
							 Pump1=1;// 
							}
						 else Pump1=0;//关泵
					if(Cup3Status==1)//有杯子状态确定是否开启泵
							{ 
							 Pump3=1;// 
							}
						 else Pump3=0;//关泵
					if(Cup5Status==1)//有杯子状态确定是否开启泵
							{ 
							 Pump5=1;// 
							}
						 else Pump5=0;//关泵
}

void CupDet()
 {
	
   
/******************************CUP1功能处理***********************************/
/******************************CUP1功能处理***********************************/
if(Cup1Eff==1)
{
     if(Cup1Waiting)                                                       //由于系统自检将cup1waiting 置1了
	   { 
			 Cup1Waiting=1;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;GLED1=ON; //进入检测状态|无运行|GLED1打开
		 
			 if(SensorCup(1)==Exist)                                           //检测到有杯子放入|调用函数SensorCup 返回exitdata值  Exist处值为1
        {
					Cup1Runing=1;                                                 //开始标志置1
					Cup1Waiting=0;                                               //停止等待
          GLED1=OFF;                                                  //GLED1关闭
					ScanCounter1=0;
					TimerOut1=0;
        } 		
		 }	
		 
     if(Cup1Runing)                                              //运行时判断有无水流
		 {
			 Cup1Waiting=0;Cup1Runing=1;Cup1Ending=0;Cup1EndingDraw=0;
			 RLED1=ON;
		   Cup1Status=1;                                          //开泵
			// Pump1=1;
      if (ScanCounter1>=WaitingWaterTimeOut&&(TimerOut1==0)) //等待来水时间200 10s  400 20s 断断续续来水  WaitingWaterTimeOut = 22
	        { 	
			     ScanCounter1=0;
				   TimerOut1=1;
			    }
					
					
			if(Water1Sensor==0)                               //无水 Water1Sensor外接传感器判断
						 { Timer10MSCounter11++;                     //无水 计数器
							                                             //  TX1_write2buff(Timer10MSCounter11);  
						 
							 if(Timer10MSCounter11>=SensorWaterTimeOut)     //SensorWaterTimeOut = 28 第二次来水与无水计数器相比
                 { ExitData1=0;Timer10MSCounter11=0; }                
						 } 
      if(Water1Sensor==1)                                       //有水
             {ExitData1=1; Timer10MSCounter11=0;
             } 
		
//前3个IF 准备判断状态  //（exitdata 为无水时的等于NoExist）=1 与 TimerOut1=1
						 
     if((( ExitData1==NoExist)&&(TimerOut1))||SensorCupOut(1)==NoExist )//检测到无 水流  增加直接拿杯子后停机 
    
           { 
						//PrintString1("No1\r\n"); 
            TimerOut1=1;	 
					  Cup1Runing=0;
					  Cup1Ending=1;
					  RLED1=OFF;	
            Cup1Status=VALVEOFF;            //泵停，要执行后面程序后停
						Pump1=0;                         //急停泵;  
						Delay50ms();  
						EXValve1=VALVEON;               //排空阀动作
						
				    Delay1000ms();                 //延时1s
						 
						EXValve1=VALVEOFF;            //关阀门
						 
   
           }else {TimerOut1=0; Cup1Runing=1; }
         					 
     }
	  if(Cup1Ending)
		  { 
			  TimerOut1=0;
		    Cup1Waiting=0;Cup1Runing=0;Cup1Ending=1;Cup1EndingDraw=0;
				
				GLED1=~GLED1;
				RLED1=~RLED1;
			 // PrintString1("C1End \r\n"); 
			 
       if(SensorCupOut(1)==NoExist) //检测到杯子已经拿开|所有传感器和led灯全部回到原始状态|结束标志为高
        { 
           SurplusCount+=1;//杯子拿走剩余杯子计数器加1
					 Cup1Eff=0;
					 MasterDataBuff[Conf1]=0x00;
					 RX1_Buffer[30]=0x00;//杯子已使用数组值清零，此杯子只能使用一次
					 Cup1Waiting=0;
					 Cup1Runing=0;
					 Cup1Ending=0;
					 Cup1EndingDraw=1;//结束标志
           RLED1=OFF;GLED1=OFF;  //杯子拿开，两个工作状态灯熄灭
					 ScanCounter1=0;
         } 
     }
		 
		if(Cup1EndingDraw)
			{
					//检测到杯子离开后，抽取XMS时间
					if(ScanCounter1<=CupOffDrawinWaterTimes)//检测时间（ScanCounter1*500Ms<=6*100MS）杯子离开后抽水时间 100MS
					  {	
							RLED1=~RLED1;
			       // Valve1=VALVEON;//开阀
			        Cup1Status=VALVEON;//开泵
					    Cup1Waiting=0;
					    Cup1Runing=0;
					    Cup1Ending=0;
							
							Cup1EndingDraw=1;
							//RX1_Buffer[27]=0X00;
             }
					else 
					{
					 RLED1=OFF;	
				   Cup1Status=VALVEOFF;
         //  Valve1=VALVEOFF;//关阀1
					 Cup1Waiting=1;//恢复状态等待
					 Cup1Runing=0;
					 Cup1Ending=0;
					 Cup1EndingDraw=0;
           RLED1=OFF;GLED1=OFF;
					 ScanCounter1=0;
					}
      }
}
else 
					{
					 RLED1=OFF;	
				   Cup1Status=0;
         //  Valve1=VALVEOFF;//关阀1
					 Cup1Waiting=1;//恢复状态等待
					 Cup1Runing=0;
					 Cup1Ending=0;
					 Cup1EndingDraw=0;
           RLED1=OFF;GLED1=OFF;
					 ScanCounter1=0;
					}
		
/******************************CUP3功能处理***********************************/\
if(Cup3Eff==1)
{
	if(Cup3Waiting)
	   { 
			Cup3Waiting=1;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=0;GLED3=ON;

		//	PrintString1("C1Wait \r\n"); 
		 
			if(SensorCup(3)==Exist)  //检测到有杯子放入
        {
					Cup3Runing=1;Cup3Waiting=0;
          GLED3=OFF;
					ScanCounter3=0;
					TimerOut3=0;
        } 		
		 }	
		 
	  if(Cup3Runing)//运行时判断有无水流
		 {
			 Cup3Waiting=0;Cup3Runing=1;Cup3Ending=0;Cup3EndingDraw=0;
			 RLED3=ON;
			//	EXValve1=VALVEON;//开阀
			 Cup3Status=VALVEON;//开泵
			// PrintString1("C1Run\r\n"); 
 		 
      if (ScanCounter3>=WaitingWaterTimeOut&&(TimerOut3==0))//等待来水时间200 10s  400 20s 断断续续来水
	        { 	
			     ScanCounter3=0;
				   TimerOut3=1;
			    }
					
					
			if(Water3Sensor==0)//无水
						 { Timer10MSCounter13++;//无水计算器
							 //  TX1_write2buff(Timer10MSCounter11);  
						 
							 if(Timer10MSCounter13>=SensorWaterTimeOut)  
                 { ExitData3=0;Timer10MSCounter13=0; }                
						 } 
      if(Water3Sensor==1)//有水
             {ExitData3=1; Timer10MSCounter13=0;
             } 
		
//前3个IF 准备判断状态
						 
     if((( ExitData3==NoExist)&&(TimerOut3))||SensorCupOut(3)==NoExist )//检测到无 水流  增加直接拿杯子后停机 
    
           { 
						//PrintString1("No1\r\n"); 
            TimerOut3=1;	 
					  Cup3Runing=0;
					  Cup3Ending=1;
					  RLED3=OFF;	
            Cup3Status=VALVEOFF;//抽水电机
						Pump3=0;//急停泵;  
						Delay50ms();  
						EXValve3=VALVEON;//排空阀动作
						
				    Delay1000ms(); //延时后才关闭阀  				 
            
						EXValve3=VALVEOFF;
						 
   
           }else {TimerOut3=0; Cup3Runing=1; }
         					 
     }
	  if(Cup3Ending)
		  { 
			  TimerOut3=0;
		    Cup3Waiting=0;Cup3Runing=0;Cup3Ending=1;Cup3EndingDraw=0;
				
				GLED3=~GLED3;
				RLED3=~RLED3;
			 // PrintString1("C1End \r\n"); 
			 
       if(SensorCupOut(3)==NoExist) //检测到杯子已经拿开
        { 
           SurplusCount+=1;//杯子拿走剩余杯子计数器加1
					MasterDataBuff[Conf3]=0x00;
					RX1_Buffer[31]=0x00;//杯子已使用数组值清零，此杯子只能使用一次
					 Cup3Eff=0;
					 Cup3Waiting=0;
					 Cup3Runing=0;
					 Cup3Ending=0;
					 Cup3EndingDraw=1;
           RLED3=OFF;GLED3=OFF;
					 ScanCounter3=0;
					
         } 
     }
		 
		if(Cup3EndingDraw)
			{
					//检测到杯子离开后，抽取XMS时间
					if(ScanCounter3<=CupOffDrawinWaterTimes)
					  {	
							RLED3=~RLED3;
			       // Valve1=VALVEON;//开阀
			        Cup3Status=VALVEON;//开泵
					    Cup3Waiting=0;
					    Cup3Runing=0;
					    Cup3Ending=0;
							Cup3EndingDraw=1;
							
             }
					else 
					{
					 RLED3=OFF;	
				   Cup3Status=VALVEOFF;
         //  Valve1=VALVEOFF;//关阀1
					 Cup3Waiting=1;
					 Cup3Runing=0;
					 Cup3Ending=0;
					 Cup3EndingDraw=0;
           RLED3=OFF;GLED3=OFF;
					 ScanCounter3=0;
					}
      }
}
else 
					{
					 RLED3=OFF;	
				   Cup3Status=VALVEOFF;
         //  Valve1=VALVEOFF;//关阀1
					 Cup3Waiting=1;
					 Cup3Runing=0;
					 Cup3Ending=0;
					 Cup3EndingDraw=0;
           RLED3=OFF;GLED3=OFF;
					 ScanCounter3=0;
					}
/******************************CUP3*******************************************/
			
/******************************CUP5功能处理***********************************/
if(Cup5Eff==1)
{
	if(Cup5Waiting)
	   { 
			Cup5Waiting=1;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=0;GLED5=ON;

		//	PrintString1("C1Wait \r\n"); 
		 
			if(SensorCup(5)==Exist)  //检测到有杯子放入
        {
					Cup5Runing=1;
					Cup5Waiting=0;
          GLED5=OFF;
					ScanCounter5=0;
					TimerOut5=0;
        } 		
		 }	
		 
	  if(Cup5Runing)//运行时判断有无水流
		 {
			 Cup5Waiting=0;Cup5Runing=1;Cup5Ending=0;Cup5EndingDraw=0;
			 RLED5=ON;
			//	EXValve1=VALVEON;//开阀
			 Cup5Status=VALVEON;//开泵
			// PrintString1("C1Run\r\n"); 
 		 
      if (ScanCounter5>=WaitingWaterTimeOut&&(TimerOut5==0))//等待来水时间200 10s  400 20s 断断续续来水
	        { 	
			     ScanCounter5=0;
				   TimerOut5=1;
			    }
					
					
			if(Water5Sensor==0)//无水
						 { Timer10MSCounter15++;//无水计数器
							 //  TX1_write2buff(Timer10MSCounter11);  
						 
							 if(Timer10MSCounter15>=SensorWaterTimeOut)  
                 { ExitData5=0;Timer10MSCounter15=0; }                
						 } 
      if(Water5Sensor==1)//有水
             {ExitData5=1; Timer10MSCounter15=0;
             } 
		
//前3个IF 准备判断状态
						 
     if((( ExitData5==NoExist)&&(TimerOut5))||SensorCupOut(5)==NoExist )//检测到无 水流  增加直接拿杯子后停机 
    
           { 
						//PrintString1("No1\r\n"); 
            TimerOut5=1;	 
					  Cup5Runing=0;
					  Cup5Ending=1;
					  RLED5=OFF;	
            Cup5Status=VALVEOFF;//抽水电机
						Pump5=0;//急停泵;  
						Delay50ms();  
						EXValve5=VALVEON;//排空阀动作
						
				    Delay1000ms(); //延时后才关闭阀  				 
            
						EXValve5=VALVEOFF;
						 
   
           }else {TimerOut5=0; Cup5Runing=1; }
         					 
     }
	  if(Cup5Ending)
		  { 
			  TimerOut5=0;
		    Cup5Waiting=0;Cup5Runing=0;Cup5Ending=1;Cup5EndingDraw=0;
				
				GLED5=~GLED5;
				RLED5=~RLED5;
			
			 
       if(SensorCupOut(5)==NoExist) //检测到杯子已经拿开
        { 
           SurplusCount+=1;//杯子拿走剩余杯子计数器加1
					 MasterDataBuff[Conf5]=0x00;
					 RX1_Buffer[32]=0x00;//杯子已使用数组值清零，此杯子只能使用一次
					 Cup5Eff=0; 
					 Cup5Waiting=0;
					 Cup5Runing=0;
					 Cup5Ending=0;
					 Cup5EndingDraw=1;
           RLED5=OFF;GLED5=OFF;
					 ScanCounter5=0;

         } 
     }
		 
		if(Cup5EndingDraw)
			{
					//检测到杯子离开后，抽取XMS时间
					if(ScanCounter5<=CupOffDrawinWaterTimes)
					  {	
							
							RLED5=~RLED5;
			       // Valve1=VALVEON;//开阀
			        Cup5Status=VALVEON;//开泵
					    Cup5Waiting=0;
					    Cup5Runing=0;
					    Cup5Ending=0;
							Cup5EndingDraw=1;
             }
					else 
					{
					 RLED5=OFF;	
				   Cup5Status=VALVEOFF;
         //  Valve1=VALVEOFF;//关阀1
					 Cup5Waiting=1;
					 Cup5Runing=0;
					 Cup5Ending=0;
					 Cup5EndingDraw=0;
           RLED5=OFF;GLED5=OFF;
					 ScanCounter5=0;
					
					}
      }
	}	
else 
					{
					 RLED5=OFF;	
				   Cup5Status=VALVEOFF;
         //  Valve1=VALVEOFF;//关阀1
					 Cup5Waiting=1;
					 Cup5Runing=0;
					 Cup5Ending=0;
					 Cup5EndingDraw=0;
           RLED5=OFF;GLED5=OFF;
					 ScanCounter5=0;
					}
/******************************CUP5***********************************/					

/********************************************************************/	

///********************************************************************/
			Confine();//触发杯子剩余次数减一	
			SlaveData();//检测状态值存入SlaveDatabuff里 
	} //time0定时500ms结束括号
	
/********************************************************************/	

u8 SensorCup (u8 NumberN) 
{ u8 ExitData=0;
  static u8 Timer10MSCounter1=0,Timer10MSCounter2=0,Timer10MSCounter3=0,Timer10MSCounter4=0,Timer10MSCounter5=0;
  if(Timer10MSCounter1>=60) Timer10MSCounter1=0;
	switch (NumberN)
	 { 
	    case 1://检测第一个杯座
				   {
					  if(Cup1==1)
						 { Timer10MSCounter1++;   	
							 if(Timer10MSCounter1>=SensorCupTimerOut){ExitData=1;Timer10MSCounter1=0;} //else {ExitData=0;Timer10MSCounter1（3）=0;}                 
						 }
           } break;
			case 2: {
					  if(Cup2==1)
						 { Timer10MSCounter2++;   
							 if(Timer10MSCounter2>=SensorCupTimerOut){ExitData=1;Timer10MSCounter2=0;} //else {ExitData=0;Timer10MSCounter2=0;}                 
						 }
           } break;
			case 3: {
					  if(Cup3==1)
						 { Timer10MSCounter3++;   
							 if(Timer10MSCounter3>=SensorCupTimerOut){ExitData=1;Timer10MSCounter3=0;} //else {ExitData=0;Timer10MSCounter3=0;}                
						 }
           } break;
			case 4:  {
					  if(Cup4==1)
						 { Timer10MSCounter4++;   
							 if(Timer10MSCounter4>=SensorCupTimerOut){ExitData=1;Timer10MSCounter4=0;} // else {ExitData=0;Timer10MSCounter4=0;}             
						 }
					 
           } break;
			case 5:  {
					  if(Cup5==1)
						 { Timer10MSCounter5++;   
							 if(Timer10MSCounter5>=SensorCupTimerOut){ExitData=1;Timer10MSCounter5=0;} //else {ExitData=0;Timer10MSCounter5=0;}                 
						 }
           }break;
				 default: break;
		}		 
	return ExitData;
}
/**********************************************************************************/

/**********************************************************************************/
 //名称：u8 SensorCupOut(u8 Number)
//功能：检测杯座无杯子   100次 10MS
//参数：U8 检测的杯座序号：12345
//返回值：U8 有 1 /  无 0
//
/**********************************************************************************/
u8 SensorCupOut (u8 NumberN) 
{ u8 ExitData;
  static u16 Timer10MSCounter=0;//局部静态变量
	
  if(Timer10MSCounter>=60) Timer10MSCounter=0;
	
	switch (NumberN)
	 { 
	    case 1://检测第一个杯座
				   {
					  if(Cup1==0)
						 { Timer10MSCounter++;   
							 if(Timer10MSCounter>=SensorCupOutTime){ExitData=0;Timer10MSCounter=0;}                 
						 }
						  else {ExitData=1;}
           } break;
			case 2: {
					  if(Cup2==0)
						 { Timer10MSCounter++;   
							 if(Timer10MSCounter>=SensorCupOutTime){ExitData=0;Timer10MSCounter=0;}               
						 }
						  else {ExitData=1;}
           } break;
			case 3: {
					  if(Cup3==0)
						 { Timer10MSCounter++;   
							 if(Timer10MSCounter>=SensorCupOutTime){ExitData=0;Timer10MSCounter=0;}                 
						 }
						  else {ExitData=1;}
           } break;
			case 4:  {
					  if(Cup4==0)
						 { Timer10MSCounter++;   
							 if(Timer10MSCounter>=SensorCupOutTime){ExitData=0;Timer10MSCounter=0;}              
						 }
						 else {ExitData=1;}
           } break;
			case 5:  {
					  if(Cup5==0)
						 { Timer10MSCounter++;   
							 if(Timer10MSCounter>=SensorCupOutTime){ExitData=0;Timer10MSCounter=0;}             
						 }
						  else {ExitData=1;}
           } break;
				 default: break;
		}		 
	return ExitData;
}
/**********************************************************************************/


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
//    TX1_Buffer[7]=SlaveDataBuff[Reled1];
//    TX1_Buffer[8]=SlaveDataBuff[Reled3];	//红灯状态
//		TX1_Buffer[9]=SlaveDataBuff[Reled5];
//		
//    TX1_Buffer[10]=SlaveDataBuff[Grled1];
//    TX1_Buffer[11]=SlaveDataBuff[Grled3];	//绿灯状态
//		TX1_Buffer[12]=SlaveDataBuff[Grled5];
//		
//    TX1_Buffer[13]=SlaveDataBuff[CupOne];
//    TX1_Buffer[14]=SlaveDataBuff[CupTwo];	//水杯有无状态
//		TX1_Buffer[15]=SlaveDataBuff[CupThree];
//    
//		TX1_Buffer[16]=SlaveDataBuff[Water1];
//    TX1_Buffer[17]=SlaveDataBuff[Water3];	//无有水检测
//		TX1_Buffer[18]=SlaveDataBuff[Water5];
//   
//    TX1_Buffer[19]=SlaveDataBuff[pum1];
//    TX1_Buffer[20]=SlaveDataBuff[pum3];	//泵状态检测
//		TX1_Buffer[21]=SlaveDataBuff[pum5];
//    
//    TX1_Buffer[22]=SlaveDataBuff[Valve1];
//    TX1_Buffer[23]=SlaveDataBuff[Valve3];	//空气阀状态检测
//		TX1_Buffer[24]=SlaveDataBuff[Valve5];

//    TX1_Buffer[25]=SlaveDataBuff[Sensor];	//5V电压控制
//		TX1_Buffer[26]=0x00;
//  //  TX1_Buffer[26]=(ChassisLock==1)?0x01:0x00;	//机箱锁状态
//		if(RX1_Buffer[6]==0X73)
//		{
//		TX1_Buffer[27]=SlaveDataBuff[Interval];	//时间间隔
//		}
//		else
//		{
//			TX1_Buffer[27]=0x00;
//		}
//    TX1_Buffer[28]=CheckSumTxd(TX1_Buffer,27);//校验和	
//		TX1_Buffer[29]=0x55;
//		TX1_Buffer[30]=0xAA;	//帧尾
//}



/**********************************************************************************/
//名称：SlaveData()
//功能：状态检测存入本地数据区
//参数： SlaveDataBuff：本地数据区
//       
//       
//返回值：void
//--------------------------------
//作者：XiongXiaoping 	
//修改日期：2020/05/07
//修改内容：数据采集存入发送缓冲数组
//----------------------------
/**********************************************************************************/
void SlaveData()
{
		SlaveDataBuff[Reled1]=(RLED1==ON)?0x01:0x00;
    SlaveDataBuff[Reled3]=(RLED3==ON)?0x01:0x00;	//红灯状态
		SlaveDataBuff[Reled5]=(RLED5==ON)?0x01:0x00;
		
    SlaveDataBuff[Grled1]=(GLED1==ON)?0x01:0x00;
    SlaveDataBuff[Grled3]=(GLED3==ON)?0x01:0x00;	//绿灯状态
		SlaveDataBuff[Grled5]=(GLED5==ON)?0x01:0x00;
		
    SlaveDataBuff[CupOne]=(Cup1==1)?0x01:0x00;
    SlaveDataBuff[CupTwo]=(Cup3==1)?0x01:0x00;	//水杯有无状态
		SlaveDataBuff[CupThree]=(Cup5==1)?0x01:0x00;
    
		SlaveDataBuff[Water1]=(Water1Sensor==1)?0x01:0x00;
    SlaveDataBuff[Water3]=(Water3Sensor==1)?0x01:0x00;	//无有水检测
		SlaveDataBuff[Water5]=(Water5Sensor==1)?0x01:0x00;
   
    SlaveDataBuff[pum1]=(Pump1==1)?0x01:0x00;
    SlaveDataBuff[pum3]=(Pump3==1)?0x01:0x00;	//泵状态检测
		SlaveDataBuff[pum5]=(Pump5==1)?0x01:0x00;
    
    SlaveDataBuff[Valve1]=(EXValve1==1)?0x01:0x00;
    SlaveDataBuff[Valve3]=(EXValve3==1)?0x01:0x00;	//空气阀状态检测
		SlaveDataBuff[Valve5]=(EXValve5==1)?0x01:0x00;

    SlaveDataBuff[Sensor]=(CapSensor==1)?0x01:0x00;	//5V电压控制
		SlaveDataBuff[Lock]=0x00;//机箱锁状态
   
		//SlaveDataBuff[Interval]=0x00;	//时间间隔
	
		SlaveDataBuff[TimH]=EEPROM_read_n(0x0001);
		SlaveDataBuff[TimL]=EEPROM_read_n(0x0000);	//  读取杯子剩余次数
}	





//u8  CheckSumTxd(u8 *dat,u8 length)
//{
//	u8 ResultCode=TX1_Buffer[2],m;//从长度开始效验
//	for(m=3;m<=length;m++)
//			{
//			ResultCode ^= dat[m];//每个字节逐步异或
//			} 
//   return ResultCode;
//	
//}
void Delay50ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 3;
	j = 26;
	k = 223;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}



void Delay1000ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 43;
	j = 6;
	k = 203;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
//
//
//
//
