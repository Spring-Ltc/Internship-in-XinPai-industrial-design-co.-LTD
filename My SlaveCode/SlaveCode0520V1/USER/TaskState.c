




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


u16 ScanCounter1=0, ScanCounter3=0, ScanCounter5=0;ExvalveCounter=0;//扫描次数计数，避免意外触发

//杯子有无状态
u8 Cup1Status   =0;//1无杯子
u8 Cup5Status   =0;//5无杯子
u8 Cup3Status   =0;//3无杯子

//状态机状态标志
u8 Cup1Waiting=0,Cup1Runing=0,Cup1Ending=0;Cup1EndingDraw=0;
u8 Cup5Waiting=0,Cup5Runing=0,Cup5Ending=0;Cup5EndingDraw=0;
u8 Cup3Waiting=0,Cup3Runing=0,Cup3Ending=0;Cup3EndingDraw=0;

u8 ExitData1=1;ExitData3=1;ExitData5=1;//有水无水标志

u8 EEPROMDATA=255;
u8 MaxDrawingWaterTimes=120;//最长抽水时间120秒
u8 CupOffDrawinWaterTimes=6; //杯子离开后抽水时间 100MS


u8 CupUsedCounter=0;




//【下面是什么标志占时还没读懂】
u8  Timer10MSCounter11=0,Timer10MSCounter12=0,Timer10MSCounter13=0,Timer10MSCounter14=0,Timer10MSCounter15=0;
u8  TimerOut=0,TimerOut1=0,TimerOut2=0,TimerOut3=0,TimerOut4=0,TimerOut5=0;
u8 vt1=0,vt2=0,vt3=0,vt4=0,vt5=0;
	


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




//基础任务，状态机的思想
void TaskBasic(void)
{
		ScanCounter1++;
		ScanCounter3++;
		ScanCounter5++;
	
		//只读取判断一次，然后就置位0x20不变，避免锁死后面的状态
			if(MasterDataBuff[23] == 0x02);//保持不变
			else if(MasterDataBuff[23] == 0x01)//开启使能
			{Cup1Waiting=1;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;MasterDataBuff[23]=0x02;}
			else if(MasterDataBuff[23] == 0x00)//关闭使能
			{Cup1Waiting=0;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;MasterDataBuff[23]=0x02;}
			
			if(MasterDataBuff[24] == 0x02);//保持不变
			else if(MasterDataBuff[24] == 0x01)//开启使能
			{Cup3Waiting=1;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=0;MasterDataBuff[24]=0x02;}
			else if(MasterDataBuff[24] == 0x00)//关闭使能
			{Cup3Waiting=0;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=0;MasterDataBuff[24]=0x02;}
			
			if(MasterDataBuff[25] == 0x02);//保持不变
			else if(MasterDataBuff[25] == 0x01)//开启使能
			{Cup5Waiting=1;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=0;MasterDataBuff[25]=0x02;}
			else if(MasterDataBuff[25] == 0x00)//关闭使能
			{Cup5Waiting=0;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=0;MasterDataBuff[25]=0x02;}

	
	
	
//CUP1 *********************************************************
	 if(Cup1Waiting)//等待状态，红灯亮
	 { 
			Cup1Waiting=1;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;GLED1=ON;
			if(SensorCup(1)==Exist)  //检测到有杯子放入，检测了多次，避免意外触发
			{
				Cup1Runing=1;Cup1Waiting=0;//进入第二个状态，running态，清除waiting态
				GLED1=OFF;//关闭红灯
				ScanCounter1=0;		//避免误触发计数变量
				TimerOut1=0;//杯子被取走的标志位，为1表示杯子被取走
			} 		
		}	
		 
	 if(Cup1Runing)//进入运行状态，运行时判断有无水流
	 {
				 Cup1Waiting=0;Cup1Runing=1;Cup1Ending=0;Cup1EndingDraw=0;
				 RLED1=ON;//开启绿灯指示
				 Cup1Status=VALVEON;//开泵		//设置cup1状态为有杯子，在外面会判断状态再开泵
			 
				if (ScanCounter1>=WaitingWaterTimeOut && (TimerOut1==0))//等待来水时间200 10s  400 20s 断断续续来水
				 { 	
					 ScanCounter1=0;
					 TimerOut1=1;
				 }	
				if(Water1Sensor==0)//无水
				 { 
					 Timer10MSCounter11++;//无水计算器，在运行态统计有多少次检测到没有水
					 if(Timer10MSCounter11>=SensorWaterTimeOut)  
					 { ExitData1=0;Timer10MSCounter11=0; }  //有水的标志清除，计数变量清零              
				 } 
				if(Water1Sensor==1)//有水
				 {ExitData1=1; Timer10MSCounter11=0; } //有水标志置位，无水统计变量清零
			
				//前3个IF 准备判断状态 ，确认是否有水		[没有水了，或杯子没有了(被强行取走杯子了)，执行下面语句]	 
				if((( ExitData1==NoExist)&&(TimerOut1))||SensorCupOut(1)==NoExist )//检测到无 水流  增加直接拿杯子后停机 
				{ 
					TimerOut1=1;	 
					Cup1Runing=0;//退出运行状态
					Cup1Ending=1;//表示结束一次操作，损耗一个杯子
					RLED1=OFF;	//关闭红灯
					Cup1Status=VALVEOFF;//抽水电机，//设置cup1状态为无杯子，在外面会判断状态关闭泵
					Pump1=0;//急停泵;  
					Delay50ms();  
					EXValve1=VALVEON;//排空阀动作
					Delay1000ms(); //延时后才关闭阀  				 
					EXValve1=VALVEOFF;
				 }
				 else {TimerOut1=0; Cup1Runing=1; }//处于正常运行状态								 
   }
	 
	 if(Cup1Ending)//进入结束状态
	 { 
			  TimerOut1=0;	//表示杯子已经被取走了
		    Cup1Waiting=0;Cup1Runing=0;Cup1Ending=1;Cup1EndingDraw=0;//更新状态标志
				GLED1=~GLED1;
				RLED1=~RLED1;//每进入一次这个状态，指示灯交替闪烁 
       if(SensorCupOut(1)==NoExist) //检测到杯子已经拿开，没有拿开则一直检测，直到拿开
       { 
				 Cup1Waiting=0;Cup1Runing=0; Cup1Ending=0;Cup1EndingDraw=1;	//更新状态标志
				 RLED1=OFF;GLED1=OFF;//两个灯都关闭
				 ScanCounter1=0;
       } 
   }
		 
	 if(Cup1EndingDraw)//进入结束抽取的状态
	 {
				//检测到杯子离开后，抽取XMS时间
				if(ScanCounter1<=CupOffDrawinWaterTimes)//少于规定的抽取时间，继续抽取
				{	
					RLED1=~RLED1;
				 // Valve1=VALVEON;//开阀
					Cup1Status=VALVEON;//开泵
					Cup1Waiting=0;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=1;//还是保持endingdraw状态
				}
				else //到达了规定的抽取时间，完成了一次工作，恢复到等待状态，供下一次使用
				{
					 RLED1=OFF;	//关闭红灯
					 Cup1Status=VALVEOFF;//设置杯座1状态为无杯子
					 Cup1Waiting=1;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;//更新状态机的状态
					 RLED1=OFF;GLED1=OFF;//两个灯都关闭
					 ScanCounter1=0;
				}
     }
	 
		 
//CUP3*********************************************************
	if(Cup3Waiting)//等待状态，红灯亮
	{ 
			Cup3Waiting=1;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=0;GLED3=ON;
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
			 Cup3Status=VALVEON;//开泵 
       if (ScanCounter3>=WaitingWaterTimeOut&&(TimerOut3==0))//等待来水时间200 10s  400 20s 断断续续来水
	     { 	
			     ScanCounter3=0;
				   TimerOut3=1;
			 }	
			 if(Water3Sensor==0)//无水
			 { 
				 Timer10MSCounter13++;//无水计算器;  
				 if(Timer10MSCounter13>=SensorWaterTimeOut)  
         { ExitData3=0;Timer10MSCounter13=0; }                
			 } 
       if(Water3Sensor==1)//有水
       {ExitData3=1; Timer10MSCounter13=0;} 
		
      //前3个IF 准备判断状态						 
      if((( ExitData3==NoExist)&&(TimerOut3))||SensorCupOut(3)==NoExist )//检测到无 水流  增加直接拿杯子后停机 
			{ 
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
       }
			 else {TimerOut3=0; Cup3Runing=1; }
         					 
   }
	if(Cup3Ending)
	{ 
			TimerOut3=0;
			Cup3Waiting=0;Cup3Runing=0;Cup3Ending=1;Cup3EndingDraw=0;
			GLED3=~GLED3;
			RLED3=~RLED3;
      if(SensorCupOut(3)==NoExist) //检测到杯子已经拿开
      { 
					 Cup3Waiting=0;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=1;
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
						Cup3Status=VALVEON;//开泵
						Cup3Waiting=0;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=1;
				 }
				 else 
				 {
					 RLED3=OFF;	
				   Cup3Status=VALVEOFF;
					 Cup3Waiting=1;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=0;
           RLED3=OFF;GLED3=OFF;
					 ScanCounter3=0;
					}
   }
//CUP5*********************************************************
		if(Cup5Waiting)
	  { 
			Cup5Waiting=1;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=0;GLED5=ON;
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
			 Cup5Status=VALVEON;//开泵
       if (ScanCounter5>=WaitingWaterTimeOut&&(TimerOut5==0))//等待来水时间200 10s  400 20s 断断续续来水
	     { 	
			     ScanCounter5=0;
				   TimerOut5=1;
			  }	
			  if(Water5Sensor==0)//无水
				{ 
					Timer10MSCounter15++;//无水计算器
					if(Timer10MSCounter15>=SensorWaterTimeOut)  
          { ExitData5=0;Timer10MSCounter15=0; }                
				} 
				if(Water5Sensor==1)//有水
        {ExitData5=1; Timer10MSCounter15=0;  } 
		
				//前3个IF 准备判断状态	 
				if((( ExitData5==NoExist)&&(TimerOut5))||SensorCupOut(5)==NoExist )//检测到无 水流  增加直接拿杯子后停机 
        { 
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
          }
					else {TimerOut5=0; Cup5Runing=1; }					 
     }
	  if(Cup5Ending)
		{ 
				TimerOut5=0;
				Cup5Waiting=0;Cup5Runing=0;Cup5Ending=1;Cup5EndingDraw=0;
				GLED5=~GLED5;
				RLED5=~RLED5;
        if(SensorCupOut(5)==NoExist) //检测到杯子已经拿开
        { 
					 Cup5Waiting=0;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=1;
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
			        Cup5Status=VALVEON;//开泵
					    Cup5Waiting=0;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=1;
           }
					else 
					{
					 RLED5=OFF;	
				   Cup5Status=VALVEOFF;
					 Cup5Waiting=1;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=0;
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
{
	u8 ExitData=0;
  static u8 Timer10MSCounter1=0,Timer10MSCounter3=0,Timer10MSCounter5=0;
  if(Timer10MSCounter1>=60) 
		Timer10MSCounter1=0;
	switch (NumberN)
	{ 
		case 1://检测第一个杯座
		{
			if(Cup1==1)
			 { 
				 Timer10MSCounter1++;   	
				 if(Timer10MSCounter1>=SensorCupTimerOut)
						{ExitData=1;Timer10MSCounter1=0;} //else {ExitData=0;Timer10MSCounter1=0;}                 
			 }
		 }break;
		case 3: 
		{
			if(Cup3==1)
			{ 
				Timer10MSCounter3++;   
				 if(Timer10MSCounter3>=SensorCupTimerOut)
						{ExitData=1;Timer10MSCounter3=0;} //else {ExitData=0;Timer10MSCounter3=0;}                
			 }
		 } break;
		case 5:
		{
			if(Cup5==1)
			{ 
				Timer10MSCounter5++;   
				if(Timer10MSCounter5>=SensorCupTimerOut)
					{ExitData=1;Timer10MSCounter5=0;} //else {ExitData=0;Timer10MSCounter5=0;}                 
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
{
	u8 ExitData;
  static u16 Timer10MSCounter=0;
  if(Timer10MSCounter>=60) 
		Timer10MSCounter=0;
	switch (NumberN)
	{ 
		case 1://检测第一个杯座
		{
			if(Cup1==0)
			{ 
				Timer10MSCounter++;   
				if(Timer10MSCounter>=SensorCupOutTime)
					{ExitData=0;Timer10MSCounter=0;}                 
			 }
			 else {ExitData=1;}
		 } break;
		case 3: 
		{
			if(Cup3==0)
			{ 
				Timer10MSCounter++;   
				 if(Timer10MSCounter>=SensorCupOutTime)
						{ExitData=0;Timer10MSCounter=0;}                 
			}
			 else {ExitData=1;}
		 } break;
		case 5: 
		{
			if(Cup5==0)
			 {
				 Timer10MSCounter++;   
				 if(Timer10MSCounter>=SensorCupOutTime)
						{ExitData=0;Timer10MSCounter=0;}             
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



void WriteStateToSlaveDataBuff(void)
{
	SlaveDataBuff[0] = RLED1;
	SlaveDataBuff[1] = RLED3;
	SlaveDataBuff[2] = RLED5;
	SlaveDataBuff[3] = GLED1;
	SlaveDataBuff[4] = GLED3;
	SlaveDataBuff[5] = GLED5;//写LED状态
	
	SlaveDataBuff[6] = Cup1Status;
	SlaveDataBuff[7] = Cup3Status;
	SlaveDataBuff[8] = Cup5Status;//写有无杯子状态
	
	SlaveDataBuff[6] = ExitData1;
	SlaveDataBuff[7] = ExitData3;
	SlaveDataBuff[8] = ExitData5;//写有无水状态

}

void PumpControl(void)
{
	if(Cup1Status==1)//有杯子状态确定是否开启泵
			 Pump1=1;// 
	else Pump1=0;//关泵
	if(Cup3Status==1)//有杯子状态确定是否开启泵
			 Pump3=1;// 
	else Pump3=0;//关泵
	if(Cup5Status==1)//有杯子状态确定是否开启泵
			 Pump5=1;// 
	else Pump5=0;//关泵
}


