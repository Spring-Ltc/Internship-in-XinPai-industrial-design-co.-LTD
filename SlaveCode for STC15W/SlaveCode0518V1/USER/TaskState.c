




#include "TaskState.h"
#include "GPIO.h"

#define Exist 1
#define NoExist 0
#define ON 0
#define OFF 1
#define VALVEON  1
#define VALVEOFF 0

#define WaitingWaterTimeOut 22  //��һ�εȴ���ˮʱ�� 22

#define SensorCupTimerOut 3       //������ ���ñ��Ӻ�����ʱ��
#define SensorCupOutTime 5        //������ ����ȡ����ش���ʱ��
#define SensorWaterTimeOut  28    //�ڶ�����ˮ ��������⵽��ˮ��ȷ��ʱ��  12

//#define ExvalveRunTime  50        //�ſշ����м��ʱ��30/15��


#define Pump1        P27
#define Pump3        P45// 
#define Pump5        P46// 
#define EXValve1     P00//
#define EXValve3     P01//
#define EXValve5     P02//
#define SPARE        P03//
#define CapSensor        P04// ���������ӹ��� OUT5V




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
#define Cup1    P10//1�ޱ���
#define Cup2    P54//2�ޱ���
#define Cup3    P12//3�ޱ���
#define Cup4    P05//4�ޱ���
#define Cup5    P15//5�ޱ���
#define Water1Sensor P21// 0:��ˮ  1����ˮ
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
u8 MaxDrawingWaterTimes=120;//���ˮʱ��120��
u8 CupOffDrawinWaterTimes=6; //�����뿪���ˮʱ�� 100MS
 

//u8 MachineStatus   =0;//����״̬  1:Runing  0:Stoping
//u8 WaitingStatus   =0;//�ȴ�����
//u8 DrawingStatus   =0;//��ȡҺ��
//u8 FinishingStatus =0;//����״̬
//u8 FailingStatus   =0;//����̬
//u8 PumpStatus   =0;//���ù���״̬
u8 Cup1Status   =0;//1�ޱ���
u8 Cup5Status   =0;//5�ޱ���
u8 Cup3Status   =0;//3�ޱ���


u8 Cup1Waiting=0,Cup1Runing=0,Cup1Ending=0;Cup1EndingDraw=0;
u8 Cup5Waiting=0,Cup5Runing=0,Cup5Ending=0;Cup5EndingDraw=0;
u8 Cup3Waiting=0,Cup3Runing=0,Cup3Ending=0;Cup3EndingDraw=0;







void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;	
	
	/*���Ƽ̵����������ų�ʼ��*/
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7;//VALVE5 4 3 2 1   R GLED1
	GPIO_InitStructure.Mode = GPIO_OUT_PP	;	// �������		 
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);
	
	
	GPIO_InitStructure.Pin  = GPIO_Pin_5;// CUP4
	GPIO_InitStructure.Mode = GPIO_PullUp	;	//����׼˫��� 	 
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);	
	

	
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_5; //CUP1/CUP3/CUP5
	GPIO_InitStructure.Mode =GPIO_PullUp ;//����׼˫���	 	 
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7; // RLED3 /R5/G5/ R GLED2
	GPIO_InitStructure.Mode = GPIO_OUT_PP	;	//������� 
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	
	
	
 	GPIO_InitStructure.Pin  = GPIO_Pin_7;// //PUMP
 	GPIO_InitStructure.Mode = GPIO_OUT_PP	;	//������� 		 
 	GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);	
	 
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_1;//   WATERSTATUSE 2/1
 	GPIO_InitStructure.Mode = GPIO_PullUp	;//����׼˫���		 
 	GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);	
	
  GPIO_InitStructure.Pin  = GPIO_Pin_6|GPIO_Pin_7; // UART1
	GPIO_InitStructure.Mode = GPIO_PullUp	;	//����׼˫���  
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

//�ϵ��Լ�
void SelfCheckUp(void)
{
	 
	//�Լ�
	 Pump1=VALVEOFF;// OFF PMUP
   Pump5=VALVEOFF;// OFF PMUP	
   Pump3=VALVEOFF;// OFF PMUP
   EXValve1=VALVEOFF;
	 EXValve5=VALVEOFF;
	 EXValve3=VALVEOFF;  
	 SPARE=VALVEOFF;
	 CapSensor=VALVEOFF; //����5V��


/*******************���Լ�  ˮ������ʽ*********************************/

 Pump1=VALVEON;//��PMUP1
 Pump5=VALVEON;//��PMUP2
 Pump3=VALVEON;//��PMUP3
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
CapSensor=VALVEON; //����5V	


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
		 
			if(SensorCup(1)==Exist)  //��⵽�б��ӷ���
        {
					Cup1Runing=1;Cup1Waiting=0;
          GLED1=OFF;
					ScanCounter1=0;
					TimerOut1=0;
        } 		
		 }	
		 
	  if(Cup1Runing)//����ʱ�ж�����ˮ��
		 {
			 Cup1Waiting=0;Cup1Runing=1;Cup1Ending=0;Cup1EndingDraw=0;
			 RLED1=ON;
			//	EXValve1=VALVEON;//����
			 Cup1Status=VALVEON;//����
			// PrintString1("C1Run\r\n"); 
 		 
      if (ScanCounter1>=WaitingWaterTimeOut&&(TimerOut1==0))//�ȴ���ˮʱ��200 10s  400 20s �϶�������ˮ
	        { 	
			     ScanCounter1=0;
				   TimerOut1=1;
			    }
					
					
			if(Water1Sensor==0)//��ˮ
						 { Timer10MSCounter11++;//��ˮ������
							 //  TX1_write2buff(Timer10MSCounter11);  
						 
							 if(Timer10MSCounter11>=SensorWaterTimeOut)  
                 { ExitData1=0;Timer10MSCounter11=0; }                
						 } 
      if(Water1Sensor==1)//��ˮ
             {ExitData1=1; Timer10MSCounter11=0;
             } 
		
//ǰ3��IF ׼���ж�״̬
						 
     if((( ExitData1==NoExist)&&(TimerOut1))||SensorCupOut(1)==NoExist )//��⵽�� ˮ��  ����ֱ���ñ��Ӻ�ͣ�� 
    
           { 
						//PrintString1("No1\r\n"); 
            TimerOut1=1;	 
					  Cup1Runing=0;
					  Cup1Ending=1;
					  RLED1=OFF;	
            Cup1Status=VALVEOFF;//��ˮ���
						Pump1=0;//��ͣ��;  
						Delay50ms();  
						EXValve1=VALVEON;//�ſշ�����
						
				    Delay1000ms(); //��ʱ��Źرշ�  				 
            
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
			 
       if(SensorCupOut(1)==NoExist) //��⵽�����Ѿ��ÿ�
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
					//��⵽�����뿪�󣬳�ȡXMSʱ��
					if(ScanCounter1<=CupOffDrawinWaterTimes)
					  {	
							RLED1=~RLED1;
			       // Valve1=VALVEON;//����
			        Cup1Status=VALVEON;//����
					    Cup1Waiting=0;
					    Cup1Runing=0;
					    Cup1Ending=0;
							Cup1EndingDraw=1;
             }
					else 
					{
					 RLED1=OFF;	
				   Cup1Status=VALVEOFF;
         //  Valve1=VALVEOFF;//�ط�1
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
		 
			if(SensorCup(3)==Exist)  //��⵽�б��ӷ���
        {
					Cup3Runing=1;Cup3Waiting=0;
          GLED3=OFF;
					ScanCounter3=0;
					TimerOut3=0;
        } 		
		 }	
		 
	  if(Cup3Runing)//����ʱ�ж�����ˮ��
		 {
			 Cup3Waiting=0;Cup3Runing=1;Cup3Ending=0;Cup3EndingDraw=0;
			 RLED3=ON;
			//	EXValve1=VALVEON;//����
			 Cup3Status=VALVEON;//����
			// PrintString1("C1Run\r\n"); 
 		 
      if (ScanCounter3>=WaitingWaterTimeOut&&(TimerOut3==0))//�ȴ���ˮʱ��200 10s  400 20s �϶�������ˮ
	        { 	
			     ScanCounter3=0;
				   TimerOut3=1;
			    }
					
					
			if(Water3Sensor==0)//��ˮ
						 { Timer10MSCounter13++;//��ˮ������
							 //  TX1_write2buff(Timer10MSCounter11);  
						 
							 if(Timer10MSCounter13>=SensorWaterTimeOut)  
                 { ExitData3=0;Timer10MSCounter13=0; }                
						 } 
      if(Water3Sensor==1)//��ˮ
             {ExitData3=1; Timer10MSCounter13=0;
             } 
		
//ǰ3��IF ׼���ж�״̬
						 
     if((( ExitData3==NoExist)&&(TimerOut3))||SensorCupOut(3)==NoExist )//��⵽�� ˮ��  ����ֱ���ñ��Ӻ�ͣ�� 
    
           { 
						//PrintString1("No1\r\n"); 
            TimerOut3=1;	 
					  Cup3Runing=0;
					  Cup3Ending=1;
					  RLED3=OFF;	
            Cup3Status=VALVEOFF;//��ˮ���
						Pump3=0;//��ͣ��;  
						Delay50ms();  
						EXValve3=VALVEON;//�ſշ�����
						
				    Delay1000ms(); //��ʱ��Źرշ�  				 
            
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
			 
       if(SensorCupOut(3)==NoExist) //��⵽�����Ѿ��ÿ�
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
					//��⵽�����뿪�󣬳�ȡXMSʱ��
					if(ScanCounter3<=CupOffDrawinWaterTimes)
					  {	
							RLED3=~RLED3;
			       // Valve1=VALVEON;//����
			        Cup3Status=VALVEON;//����
					    Cup3Waiting=0;
					    Cup3Runing=0;
					    Cup3Ending=0;
							Cup3EndingDraw=1;
             }
					else 
					{
					 RLED3=OFF;	
				   Cup3Status=VALVEOFF;
         //  Valve1=VALVEOFF;//�ط�1
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
		 
			if(SensorCup(5)==Exist)  //��⵽�б��ӷ���
        {
					Cup5Runing=1;Cup5Waiting=0;
          GLED5=OFF;
					ScanCounter5=0;
					TimerOut5=0;
        } 		
		 }	
		 
	  if(Cup5Runing)//����ʱ�ж�����ˮ��
		 {
			 Cup5Waiting=0;Cup5Runing=1;Cup5Ending=0;Cup5EndingDraw=0;
			 RLED5=ON;
			//	EXValve1=VALVEON;//����
			 Cup5Status=VALVEON;//����
			// PrintString1("C1Run\r\n"); 
 		 
      if (ScanCounter5>=WaitingWaterTimeOut&&(TimerOut5==0))//�ȴ���ˮʱ��200 10s  400 20s �϶�������ˮ
	        { 	
			     ScanCounter5=0;
				   TimerOut5=1;
			    }
					
					
			if(Water5Sensor==0)//��ˮ
						 { Timer10MSCounter15++;//��ˮ������
							 //  TX1_write2buff(Timer10MSCounter11);  
						 
							 if(Timer10MSCounter15>=SensorWaterTimeOut)  
                 { ExitData5=0;Timer10MSCounter15=0; }                
						 } 
      if(Water5Sensor==1)//��ˮ
             {ExitData5=1; Timer10MSCounter15=0;
             } 
		
//ǰ3��IF ׼���ж�״̬
						 
     if((( ExitData5==NoExist)&&(TimerOut5))||SensorCupOut(5)==NoExist )//��⵽�� ˮ��  ����ֱ���ñ��Ӻ�ͣ�� 
    
           { 
						//PrintString1("No1\r\n"); 
            TimerOut5=1;	 
					  Cup5Runing=0;
					  Cup5Ending=1;
					  RLED5=OFF;	
            Cup5Status=VALVEOFF;//��ˮ���
						Pump5=0;//��ͣ��;  
						Delay50ms();  
						EXValve5=VALVEON;//�ſշ�����
						
				    Delay1000ms(); //��ʱ��Źرշ�  				 
            
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
			 
       if(SensorCupOut(5)==NoExist) //��⵽�����Ѿ��ÿ�
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
					//��⵽�����뿪�󣬳�ȡXMSʱ��
					if(ScanCounter5<=CupOffDrawinWaterTimes)
					  {	
							RLED5=~RLED5;
			       // Valve1=VALVEON;//����
			        Cup5Status=VALVEON;//����
					    Cup5Waiting=0;
					    Cup5Runing=0;
					    Cup5Ending=0;
							Cup5EndingDraw=1;
             }
					else 
					{
					 RLED5=OFF;	
				   Cup5Status=VALVEOFF;
         //  Valve1=VALVEOFF;//�ط�1
					 Cup5Waiting=1;
					 Cup5Runing=0;
					 Cup5Ending=0;
					 Cup5EndingDraw=0;
           RLED5=OFF;GLED5=OFF;
					 ScanCounter3=0;
					}
      }
	
	
	
	
	
	
}


//���ƣ�u8 SensorCup(u8 Number)
//���ܣ���Ɑ�����ޱ���   100�� 10MS
//������U8 ���ı�����ţ�12345
//����ֵ��U8 �� 1 /  �� 0
//
u8 SensorCup (u8 NumberN) 
{ u8 ExitData=0;
  static u8 Timer10MSCounter1=0,Timer10MSCounter2=0,Timer10MSCounter3=0,Timer10MSCounter4=0,Timer10MSCounter5=0;
  if(Timer10MSCounter1>=60) Timer10MSCounter1=0;
	switch (NumberN)
	 { 
	    case 1://����һ������
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
 //���ƣ�u8 SensorCupOut(u8 Number)
//���ܣ���Ɑ���ޱ���   100�� 10MS
//������U8 ���ı�����ţ�12345
//����ֵ��U8 �� 1 /  �� 0
//
//
u8 SensorCupOut (u8 NumberN) 
{ u8 ExitData;
  static u16 Timer10MSCounter=0;
	
  if(Timer10MSCounter>=60) Timer10MSCounter=0;
	
	switch (NumberN)
	 { 
	    case 1://����һ������
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

//���ƣ�u8 SensorWater(u8 Number)
//���ܣ����ܵ�������Һ��  100�� 10MS
//������U8 ���Ĺܵ���ţ�12345
//����ֵ��U8 �� 1 /  �� 0
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
//	    case 1://����һ���ܵ�
//				   {
//					  if(Water1Sensor==0)//��ˮ
//						 { Timer10MSCounter11++;     TX1_write2buff(Timer10MSCounter11);  
//						 
//							 if(Timer10MSCounter11>=SensorWaterTimeOut)  
//                 { ExitData=0;Timer10MSCounter11=0; }                
//						 } 
//            if(Water1Sensor==1)//��ˮ
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
//��ʱ����50MS

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