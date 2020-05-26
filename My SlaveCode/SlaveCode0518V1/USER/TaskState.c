




#include "TaskState.h"
#include "GPIO.h"

#define Exist 1
#define NoExist 0
#define ON 0
#define OFF 1
#define VALVEON  1
#define VALVEOFF 0

#define WaitingWaterTimeOut 22  //第一次等待来水时间 22

#define SensorCupTimerOut 3       //传感器 放置杯子后启动时间
#define SensorCupOutTime 5        //传感器 杯子取出后回待杯时间
#define SensorWaterTimeOut  28    //第二次来水 传感器检测到无水后确认时间  12

//#define ExvalveRunTime  50        //排空阀运行间隔时间30/15秒


#define Pump1        P27
#define Pump3        P45// 
#define Pump5        P46// 
#define EXValve1     P00//
#define EXValve3     P01//
#define EXValve5     P02//
#define SPARE        P03//
#define CapSensor        P04// 传感器板子供电 OUT5V




#define RLED1  P06//
#define RLED2  P16//
#define RLED3  P11//
#define RLED4  P52//
#define RLED5  P13//

#define GLED1  P07//
#define GLED2  P17//
#define GLED3  P47//
#define GLED4  P53//
#define GLED5  P14//

/*INPUT PORTx  10 */
#define Cup1    P10//1无杯子
#define Cup2    P54//2无杯子
#define Cup3    P12//3无杯子
#define Cup4    P05//4无杯子
#define Cup5    P15//5无杯子
#define Water1Sensor P21// 0:无水  1：有水
#define Water2Sensor P20//
#define Water3Sensor P44//
#define Water4Sensor P43//
#define Water5Sensor P42//
#define LED P41

u8  Timer10MSCounter11=0,Timer10MSCounter12=0,Timer10MSCounter13=0,Timer10MSCounter14=0,Timer10MSCounter15=0;
u8  TimerOut=0,TimerOut1=0,TimerOut2=0,TimerOut3=0,TimerOut4=0,TimerOut5=0;
u16 ScanCounter1=0, ScanCounter2=0, ScanCounter3=0, ScanCounter4=0, ScanCounter5=0;ExvalveCounter=0;
  u8 vt1=0,vt2=0,vt3=0,vt4=0,vt5=0;
	u8 ExitData1=1;ExitData2=1;ExitData3=1;ExitData4=1;ExitData5=1;
u8 EEPROMDATA=255;
u8 MaxDrawingWaterTimes=120;//最长抽水时间120秒
u8 CupOffDrawinWaterTimes=6; //杯子离开后抽水时间 100MS
 

//u8 MachineStatus   =0;//机器状态  1:Runing  0:Stoping
//u8 WaitingStatus   =0;//等待放置
//u8 DrawingStatus   =0;//抽取液体
//u8 FinishingStatus =0;//结束状态
//u8 FailingStatus   =0;//故障态
//u8 PumpStatus   =0;//主泵工作状态
u8 Cup1Status   =0;//1无杯子
u8 Cup5Status   =0;//5无杯子
u8 Cup3Status   =0;//3无杯子


u8 Cup1Waiting=0,Cup1Runing=0,Cup1Ending=0;Cup1EndingDraw=0;
u8 Cup5Waiting=0,Cup5Runing=0,Cup5Ending=0;Cup5EndingDraw=0;
u8 Cup3Waiting=0,Cup3Runing=0,Cup3Ending=0;Cup3EndingDraw=0;







void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;	
	
	/*控制继电器工作引脚初始化*/
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7;//VALVE5 4 3 2 1   R GLED1
	GPIO_InitStructure.Mode = GPIO_OUT_PP	;	// 推挽输出		 
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);
	
	
	GPIO_InitStructure.Pin  = GPIO_Pin_5;// CUP4
	GPIO_InitStructure.Mode = GPIO_PullUp	;	//上拉准双向口 	 
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);	
	

	
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_5; //CUP1/CUP3/CUP5
	GPIO_InitStructure.Mode =GPIO_PullUp ;//上拉准双向口	 	 
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7; // RLED3 /R5/G5/ R GLED2
	GPIO_InitStructure.Mode = GPIO_OUT_PP	;	//推挽输出 
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	
	
	
 	GPIO_InitStructure.Pin  = GPIO_Pin_7;// //PUMP
 	GPIO_InitStructure.Mode = GPIO_OUT_PP	;	//推挽输出 		 
 	GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);	
	 
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_1;//   WATERSTATUSE 2/1
 	GPIO_InitStructure.Mode = GPIO_PullUp	;//上拉准双向口		 
 	GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);	
	
  GPIO_InitStructure.Pin  = GPIO_Pin_6|GPIO_Pin_7; // UART1
	GPIO_InitStructure.Mode = GPIO_PullUp	;	//上拉准双向口  
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	
	
	GPIO_InitStructure.Pin  = GPIO_Pin_1|GPIO_Pin_5| GPIO_Pin_6|GPIO_Pin_7; // LED /CAPSENS/ EXVALVE/GLED3
	GPIO_InitStructure.Mode = GPIO_OUT_PP;	// 			 
	GPIO_Inilize(GPIO_P4,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4; //WATERSTATUSE5 4 3
	GPIO_InitStructure.Mode = GPIO_PullUp;	// 			 
	GPIO_Inilize(GPIO_P4,&GPIO_InitStructure);
  
	GPIO_InitStructure.Pin  = GPIO_Pin_2|GPIO_Pin_3;  //R GLED4 
	GPIO_InitStructure.Mode = GPIO_OUT_PP;//  	 
	GPIO_Inilize(GPIO_P5,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_4 ;  //CUP2
	GPIO_InitStructure.Mode = GPIO_PullUp;//  	 
	GPIO_Inilize(GPIO_P5,&GPIO_InitStructure);
} 

//上电自检
void SelfCheckUp(void)
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
CapSensor=VALVEON; //供电5V	


Cup1Waiting=1;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;
Cup5Waiting=1;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=0;
Cup3Waiting=1;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=0;
}


void TaskBasic(void)
{
		ScanCounter1++;
		ScanCounter5++;
		ScanCounter3++;
	
	//CUP1 *********************************************************
	if(Cup1Waiting)
	   { 
			Cup1Waiting=1;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;GLED1=ON;

		//	PrintString1("C1Wait \r\n"); 
		 
			if(SensorCup(1)==Exist)  //检测到有杯子放入
        {
					Cup1Runing=1;Cup1Waiting=0;
          GLED1=OFF;
					ScanCounter1=0;
					TimerOut1=0;
        } 		
		 }	
		 
	  if(Cup1Runing)//运行时判断有无水流
		 {
			 Cup1Waiting=0;Cup1Runing=1;Cup1Ending=0;Cup1EndingDraw=0;
			 RLED1=ON;
			//	EXValve1=VALVEON;//开阀
			 Cup1Status=VALVEON;//开泵
			// PrintString1("C1Run\r\n"); 
 		 
      if (ScanCounter1>=WaitingWaterTimeOut&&(TimerOut1==0))//等待来水时间200 10s  400 20s 断断续续来水
	        { 	
			     ScanCounter1=0;
				   TimerOut1=1;
			    }
					
					
			if(Water1Sensor==0)//无水
						 { Timer10MSCounter11++;//无水计算器
							 //  TX1_write2buff(Timer10MSCounter11);  
						 
							 if(Timer10MSCounter11>=SensorWaterTimeOut)  
                 { ExitData1=0;Timer10MSCounter11=0; }                
						 } 
      if(Water1Sensor==1)//有水
             {ExitData1=1; Timer10MSCounter11=0;
             } 
		
//前3个IF 准备判断状态
						 
     if((( ExitData1==NoExist)&&(TimerOut1))||SensorCupOut(1)==NoExist )//检测到无 水流  增加直接拿杯子后停机 
    
           { 
						//PrintString1("No1\r\n"); 
            TimerOut1=1;	 
					  Cup1Runing=0;
					  Cup1Ending=1;
					  RLED1=OFF;	
            Cup1Status=VALVEOFF;//抽水电机
						Pump1=0;//急停泵;  
						Delay50ms();  
						EXValve1=VALVEON;//排空阀动作
						
				    Delay1000ms(); //延时后才关闭阀  				 
            
						EXValve1=VALVEOFF;
						 
   
           }else {TimerOut1=0; Cup1Runing=1; }
         					 
     }
	  if(Cup1Ending)
		  { 
			  TimerOut1=0;
		    Cup1Waiting=0;Cup1Runing=0;Cup1Ending=1;Cup1EndingDraw=0;
				
				GLED1=~GLED1;
				RLED1=~RLED1;
			 // PrintString1("C1End \r\n"); 
			 
       if(SensorCupOut(1)==NoExist) //检测到杯子已经拿开
        { 
           
					 Cup1Waiting=0;
					 Cup1Runing=0;
					 Cup1Ending=0;
					 Cup1EndingDraw=1;
           RLED1=OFF;GLED1=OFF;
					 ScanCounter1=0;
         } 
     }
		 
		if(Cup1EndingDraw)
			{
					//检测到杯子离开后，抽取XMS时间
					if(ScanCounter1<=CupOffDrawinWaterTimes)
					  {	
							RLED1=~RLED1;
			       // Valve1=VALVEON;//开阀
			        Cup1Status=VALVEON;//开泵
					    Cup1Waiting=0;
					    Cup1Runing=0;
					    Cup1Ending=0;
							Cup1EndingDraw=1;
             }
					else 
					{
					 RLED1=OFF;	
				   Cup1Status=VALVEOFF;
         //  Valve1=VALVEOFF;//关阀1
					 Cup1Waiting=1;
					 Cup1Runing=0;
					 Cup1Ending=0;
					 Cup1EndingDraw=0;
           RLED1=OFF;GLED1=OFF;
					 ScanCounter1=0;
					}
      }
	//CUP3*********************************************************
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
			//CUP5*********************************************************
	if(Cup5Waiting)
	   { 
			Cup5Waiting=1;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=0;GLED5=ON;

		//	PrintString1("C1Wait \r\n"); 
		 
			if(SensorCup(5)==Exist)  //检测到有杯子放入
        {
					Cup5Runing=1;Cup5Waiting=0;
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
						 { Timer10MSCounter15++;//无水计算器
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
			 // PrintString1("C1End \r\n"); 
			 
       if(SensorCupOut(5)==NoExist) //检测到杯子已经拿开
        { 
           
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
					 ScanCounter3=0;
					}
      }
	
	
	
	
	
	
}


//名称：u8 SensorCup(u8 Number)
//功能：检测杯座有无杯子   100次 10MS
//参数：U8 检测的杯座序号：12345
//返回值：U8 有 1 /  无 0
//
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
							 if(Timer10MSCounter1>=SensorCupTimerOut){ExitData=1;Timer10MSCounter1=0;} //else {ExitData=0;Timer10MSCounter1=0;}                 
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
           } break;
				 default: break;
		}		 
	return ExitData;
}

 //
 //名称：u8 SensorCupOut(u8 Number)
//功能：检测杯座无杯子   100次 10MS
//参数：U8 检测的杯座序号：12345
//返回值：U8 有 1 /  无 0
//
//
u8 SensorCupOut (u8 NumberN) 
{ u8 ExitData;
  static u16 Timer10MSCounter=0;
	
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

//名称：u8 SensorWater(u8 Number)
//功能：检测管道里有无液体  100次 10MS
//参数：U8 检测的管道序号：12345
//返回值：U8 有 1 /  无 0
//

//u8 SensorWater(u8 NumberN) 
//{ 
//	u8  ExitData=0;
//	
////	if(Timer10MSCounter11>=60) Timer10MSCounter11=0;
////	if(Timer10MSCounter12>=60) Timer10MSCounter11=0;
////	if(Timer10MSCounter13>=60) Timer10MSCounter11=0;
////	if(Timer10MSCounter14>=60) Timer10MSCounter11=0;
////	if(Timer10MSCounter15>=60) Timer10MSCounter11=0;
//	
//	switch (NumberN)
//	 { 
//	    case 1://检测第一条管道
//				   {
//					  if(Water1Sensor==0)//无水
//						 { Timer10MSCounter11++;     TX1_write2buff(Timer10MSCounter11);  
//						 
//							 if(Timer10MSCounter11>=SensorWaterTimeOut)  
//                 { ExitData=0;Timer10MSCounter11=0; }                
//						 } 
//            if(Water1Sensor==1)//有水
//             {ExitData=1; Timer10MSCounter11=0;
//             } 
//           } break;
//			case 2: {
//					  if(Water2Sensor==0)	
//						 { Timer10MSCounter12++;   
//							 if(Timer10MSCounter12>=SensorWaterTimeOut){ExitData=0;Timer10MSCounter12=0;} //else {ExitData=0;}                 
//						 }
//            else {ExitData=1;Timer10MSCounter12=0;}
//           } break;
//			case 3: {
//					  if(Water3Sensor==0)
//						 { Timer10MSCounter13++;   
//							 if(Timer10MSCounter13>=SensorWaterTimeOut){ExitData=0;Timer10MSCounter13=0;} //else {ExitData=0;}                 
//						 } 
//             else {ExitData=1;Timer10MSCounter13=0;}
//           } break;
//			case 4:  {
//					  if(Water4Sensor==0)
//						 { Timer10MSCounter14++;   
//							 if(Timer10MSCounter14>=SensorWaterTimeOut){ExitData=0;Timer10MSCounter14=0;} //else ExitData=1;                   
//						 }
//            else {ExitData=1;Timer10MSCounter14=0;}
// 
//           } break;
//			case 5:{
//					      if(Water5Sensor==0)
//						      { Timer10MSCounter15++;   
//							      if(Timer10MSCounter15>=SensorWaterTimeOut){ExitData=0;Timer10MSCounter15=0;} //else {ExitData=0;}                 
//						      } 
//                else {ExitData=1;Timer10MSCounter15=0;}
//             } break;
//				 default: break;
//		}	

//	TX1_write2buff(ExitData+2);	
//	return ExitData;
//}
//延时程序50MS

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