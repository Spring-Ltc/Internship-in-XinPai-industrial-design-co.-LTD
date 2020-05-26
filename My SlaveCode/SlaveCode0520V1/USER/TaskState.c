




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


u16 ScanCounter1=0, ScanCounter3=0, ScanCounter5=0;ExvalveCounter=0;//ɨ������������������ⴥ��

//��������״̬
u8 Cup1Status   =0;//1�ޱ���
u8 Cup5Status   =0;//5�ޱ���
u8 Cup3Status   =0;//3�ޱ���

//״̬��״̬��־
u8 Cup1Waiting=0,Cup1Runing=0,Cup1Ending=0;Cup1EndingDraw=0;
u8 Cup5Waiting=0,Cup5Runing=0,Cup5Ending=0;Cup5EndingDraw=0;
u8 Cup3Waiting=0,Cup3Runing=0,Cup3Ending=0;Cup3EndingDraw=0;

u8 ExitData1=1;ExitData3=1;ExitData5=1;//��ˮ��ˮ��־

u8 EEPROMDATA=255;
u8 MaxDrawingWaterTimes=120;//���ˮʱ��120��
u8 CupOffDrawinWaterTimes=6; //�����뿪���ˮʱ�� 100MS


u8 CupUsedCounter=0;




//��������ʲô��־ռʱ��û������
u8  Timer10MSCounter11=0,Timer10MSCounter12=0,Timer10MSCounter13=0,Timer10MSCounter14=0,Timer10MSCounter15=0;
u8  TimerOut=0,TimerOut1=0,TimerOut2=0,TimerOut3=0,TimerOut4=0,TimerOut5=0;
u8 vt1=0,vt2=0,vt3=0,vt4=0,vt5=0;
	


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




//��������״̬����˼��
void TaskBasic(void)
{
		ScanCounter1++;
		ScanCounter3++;
		ScanCounter5++;
	
		//ֻ��ȡ�ж�һ�Σ�Ȼ�����λ0x20���䣬�������������״̬
			if(MasterDataBuff[23] == 0x02);//���ֲ���
			else if(MasterDataBuff[23] == 0x01)//����ʹ��
			{Cup1Waiting=1;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;MasterDataBuff[23]=0x02;}
			else if(MasterDataBuff[23] == 0x00)//�ر�ʹ��
			{Cup1Waiting=0;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;MasterDataBuff[23]=0x02;}
			
			if(MasterDataBuff[24] == 0x02);//���ֲ���
			else if(MasterDataBuff[24] == 0x01)//����ʹ��
			{Cup3Waiting=1;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=0;MasterDataBuff[24]=0x02;}
			else if(MasterDataBuff[24] == 0x00)//�ر�ʹ��
			{Cup3Waiting=0;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=0;MasterDataBuff[24]=0x02;}
			
			if(MasterDataBuff[25] == 0x02);//���ֲ���
			else if(MasterDataBuff[25] == 0x01)//����ʹ��
			{Cup5Waiting=1;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=0;MasterDataBuff[25]=0x02;}
			else if(MasterDataBuff[25] == 0x00)//�ر�ʹ��
			{Cup5Waiting=0;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=0;MasterDataBuff[25]=0x02;}

	
	
	
//CUP1 *********************************************************
	 if(Cup1Waiting)//�ȴ�״̬�������
	 { 
			Cup1Waiting=1;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;GLED1=ON;
			if(SensorCup(1)==Exist)  //��⵽�б��ӷ��룬����˶�Σ��������ⴥ��
			{
				Cup1Runing=1;Cup1Waiting=0;//����ڶ���״̬��running̬�����waiting̬
				GLED1=OFF;//�رպ��
				ScanCounter1=0;		//�����󴥷���������
				TimerOut1=0;//���ӱ�ȡ�ߵı�־λ��Ϊ1��ʾ���ӱ�ȡ��
			} 		
		}	
		 
	 if(Cup1Runing)//��������״̬������ʱ�ж�����ˮ��
	 {
				 Cup1Waiting=0;Cup1Runing=1;Cup1Ending=0;Cup1EndingDraw=0;
				 RLED1=ON;//�����̵�ָʾ
				 Cup1Status=VALVEON;//����		//����cup1״̬Ϊ�б��ӣ���������ж�״̬�ٿ���
			 
				if (ScanCounter1>=WaitingWaterTimeOut && (TimerOut1==0))//�ȴ���ˮʱ��200 10s  400 20s �϶�������ˮ
				 { 	
					 ScanCounter1=0;
					 TimerOut1=1;
				 }	
				if(Water1Sensor==0)//��ˮ
				 { 
					 Timer10MSCounter11++;//��ˮ��������������̬ͳ���ж��ٴμ�⵽û��ˮ
					 if(Timer10MSCounter11>=SensorWaterTimeOut)  
					 { ExitData1=0;Timer10MSCounter11=0; }  //��ˮ�ı�־�����������������              
				 } 
				if(Water1Sensor==1)//��ˮ
				 {ExitData1=1; Timer10MSCounter11=0; } //��ˮ��־��λ����ˮͳ�Ʊ�������
			
				//ǰ3��IF ׼���ж�״̬ ��ȷ���Ƿ���ˮ		[û��ˮ�ˣ�����û����(��ǿ��ȡ�߱�����)��ִ���������]	 
				if((( ExitData1==NoExist)&&(TimerOut1))||SensorCupOut(1)==NoExist )//��⵽�� ˮ��  ����ֱ���ñ��Ӻ�ͣ�� 
				{ 
					TimerOut1=1;	 
					Cup1Runing=0;//�˳�����״̬
					Cup1Ending=1;//��ʾ����һ�β��������һ������
					RLED1=OFF;	//�رպ��
					Cup1Status=VALVEOFF;//��ˮ�����//����cup1״̬Ϊ�ޱ��ӣ���������ж�״̬�رձ�
					Pump1=0;//��ͣ��;  
					Delay50ms();  
					EXValve1=VALVEON;//�ſշ�����
					Delay1000ms(); //��ʱ��Źرշ�  				 
					EXValve1=VALVEOFF;
				 }
				 else {TimerOut1=0; Cup1Runing=1; }//������������״̬								 
   }
	 
	 if(Cup1Ending)//�������״̬
	 { 
			  TimerOut1=0;	//��ʾ�����Ѿ���ȡ����
		    Cup1Waiting=0;Cup1Runing=0;Cup1Ending=1;Cup1EndingDraw=0;//����״̬��־
				GLED1=~GLED1;
				RLED1=~RLED1;//ÿ����һ�����״̬��ָʾ�ƽ�����˸ 
       if(SensorCupOut(1)==NoExist) //��⵽�����Ѿ��ÿ���û���ÿ���һֱ��⣬ֱ���ÿ�
       { 
				 Cup1Waiting=0;Cup1Runing=0; Cup1Ending=0;Cup1EndingDraw=1;	//����״̬��־
				 RLED1=OFF;GLED1=OFF;//�����ƶ��ر�
				 ScanCounter1=0;
       } 
   }
		 
	 if(Cup1EndingDraw)//���������ȡ��״̬
	 {
				//��⵽�����뿪�󣬳�ȡXMSʱ��
				if(ScanCounter1<=CupOffDrawinWaterTimes)//���ڹ涨�ĳ�ȡʱ�䣬������ȡ
				{	
					RLED1=~RLED1;
				 // Valve1=VALVEON;//����
					Cup1Status=VALVEON;//����
					Cup1Waiting=0;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=1;//���Ǳ���endingdraw״̬
				}
				else //�����˹涨�ĳ�ȡʱ�䣬�����һ�ι������ָ����ȴ�״̬������һ��ʹ��
				{
					 RLED1=OFF;	//�رպ��
					 Cup1Status=VALVEOFF;//���ñ���1״̬Ϊ�ޱ���
					 Cup1Waiting=1;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;//����״̬����״̬
					 RLED1=OFF;GLED1=OFF;//�����ƶ��ر�
					 ScanCounter1=0;
				}
     }
	 
		 
//CUP3*********************************************************
	if(Cup3Waiting)//�ȴ�״̬�������
	{ 
			Cup3Waiting=1;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=0;GLED3=ON;
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
			 Cup3Status=VALVEON;//���� 
       if (ScanCounter3>=WaitingWaterTimeOut&&(TimerOut3==0))//�ȴ���ˮʱ��200 10s  400 20s �϶�������ˮ
	     { 	
			     ScanCounter3=0;
				   TimerOut3=1;
			 }	
			 if(Water3Sensor==0)//��ˮ
			 { 
				 Timer10MSCounter13++;//��ˮ������;  
				 if(Timer10MSCounter13>=SensorWaterTimeOut)  
         { ExitData3=0;Timer10MSCounter13=0; }                
			 } 
       if(Water3Sensor==1)//��ˮ
       {ExitData3=1; Timer10MSCounter13=0;} 
		
      //ǰ3��IF ׼���ж�״̬						 
      if((( ExitData3==NoExist)&&(TimerOut3))||SensorCupOut(3)==NoExist )//��⵽�� ˮ��  ����ֱ���ñ��Ӻ�ͣ�� 
			{ 
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
       }
			 else {TimerOut3=0; Cup3Runing=1; }
         					 
   }
	if(Cup3Ending)
	{ 
			TimerOut3=0;
			Cup3Waiting=0;Cup3Runing=0;Cup3Ending=1;Cup3EndingDraw=0;
			GLED3=~GLED3;
			RLED3=~RLED3;
      if(SensorCupOut(3)==NoExist) //��⵽�����Ѿ��ÿ�
      { 
					 Cup3Waiting=0;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=1;
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
						Cup3Status=VALVEON;//����
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
			 Cup5Status=VALVEON;//����
       if (ScanCounter5>=WaitingWaterTimeOut&&(TimerOut5==0))//�ȴ���ˮʱ��200 10s  400 20s �϶�������ˮ
	     { 	
			     ScanCounter5=0;
				   TimerOut5=1;
			  }	
			  if(Water5Sensor==0)//��ˮ
				{ 
					Timer10MSCounter15++;//��ˮ������
					if(Timer10MSCounter15>=SensorWaterTimeOut)  
          { ExitData5=0;Timer10MSCounter15=0; }                
				} 
				if(Water5Sensor==1)//��ˮ
        {ExitData5=1; Timer10MSCounter15=0;  } 
		
				//ǰ3��IF ׼���ж�״̬	 
				if((( ExitData5==NoExist)&&(TimerOut5))||SensorCupOut(5)==NoExist )//��⵽�� ˮ��  ����ֱ���ñ��Ӻ�ͣ�� 
        { 
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
          }
					else {TimerOut5=0; Cup5Runing=1; }					 
     }
	  if(Cup5Ending)
		{ 
				TimerOut5=0;
				Cup5Waiting=0;Cup5Runing=0;Cup5Ending=1;Cup5EndingDraw=0;
				GLED5=~GLED5;
				RLED5=~RLED5;
        if(SensorCupOut(5)==NoExist) //��⵽�����Ѿ��ÿ�
        { 
					 Cup5Waiting=0;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=1;
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
			        Cup5Status=VALVEON;//����
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


//���ƣ�u8 SensorCup(u8 Number)
//���ܣ���Ɑ�����ޱ���   100�� 10MS
//������U8 ���ı�����ţ�12345
//����ֵ��U8 �� 1 /  �� 0
//
u8 SensorCup (u8 NumberN) 
{
	u8 ExitData=0;
  static u8 Timer10MSCounter1=0,Timer10MSCounter3=0,Timer10MSCounter5=0;
  if(Timer10MSCounter1>=60) 
		Timer10MSCounter1=0;
	switch (NumberN)
	{ 
		case 1://����һ������
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
 //���ƣ�u8 SensorCupOut(u8 Number)
//���ܣ���Ɑ���ޱ���   100�� 10MS
//������U8 ���ı�����ţ�12345
//����ֵ��U8 �� 1 /  �� 0
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
		case 1://����һ������
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



void WriteStateToSlaveDataBuff(void)
{
	SlaveDataBuff[0] = RLED1;
	SlaveDataBuff[1] = RLED3;
	SlaveDataBuff[2] = RLED5;
	SlaveDataBuff[3] = GLED1;
	SlaveDataBuff[4] = GLED3;
	SlaveDataBuff[5] = GLED5;//дLED״̬
	
	SlaveDataBuff[6] = Cup1Status;
	SlaveDataBuff[7] = Cup3Status;
	SlaveDataBuff[8] = Cup5Status;//д���ޱ���״̬
	
	SlaveDataBuff[6] = ExitData1;
	SlaveDataBuff[7] = ExitData3;
	SlaveDataBuff[8] = ExitData5;//д����ˮ״̬

}

void PumpControl(void)
{
	if(Cup1Status==1)//�б���״̬ȷ���Ƿ�����
			 Pump1=1;// 
	else Pump1=0;//�ر�
	if(Cup3Status==1)//�б���״̬ȷ���Ƿ�����
			 Pump3=1;// 
	else Pump3=0;//�ر�
	if(Cup5Status==1)//�б���״̬ȷ���Ƿ�����
			 Pump5=1;// 
	else Pump5=0;//�ر�
}


