


#include "CupDet.h"

//extern	u8 TX1_Buffer[32];//�洢Ҫ�ϴ�������ScanCounter1
u8 Cup1Eff=0;//���ձ�����Ч ������
u8 Cup3Eff=0; //���ձ�����Ч ������//���ձ�����Ч ������
u8 Cup5Eff=0; //���ձ�����Ч ������

u8  Timer10MSCounter11=0,Timer10MSCounter12=0,Timer10MSCounter13=0,Timer10MSCounter14=0,Timer10MSCounter15=0;  //����10MS��־
u8  TimerOut=0,TimerOut1=0,TimerOut2=0,TimerOut3=0,TimerOut4=0,TimerOut5=0;//��ʱ���
u16 ScanCounter1, ScanCounter2, ScanCounter3, ScanCounter4, ScanCounter5;ExvalveCounter;//��������־|������

u8 ExitData1=1;ExitData2=1;ExitData3=1;ExitData4=1;ExitData5=1;//��־

u8 MaxDrawingWaterTimes=120;//���ˮʱ��120 
u8 CupOffDrawinWaterTimes=6; //�����뿪���ˮʱ�� 100MS

u8 Cup1Status   =0;//1�ޱ���
u8 Cup5Status   =0;//5�ޱ���
u8 Cup3Status   =0;//3�ޱ���

u8 Cup1Waiting=0,Cup1Runing=0,Cup1Ending=0;Cup1EndingDraw=0;//״̬�ж�/����/����/������־
u8 Cup5Waiting=0,Cup5Runing=0,Cup5Ending=0;Cup5EndingDraw=0;
u8 Cup3Waiting=0,Cup3Runing=0,Cup3Ending=0;Cup3EndingDraw=0;
u16 SurplusCount;//�����ڱ��ο�������ʱ�ԶԱ���ʹ�ü���

void  CupDetconfig(void)  //�Լ��ʼ��
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
			 CapSensor=VALVEON; //����5V��
			 
			 

			Cup1Waiting=0;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;//״̬�ж�/����/����/������־|��ֵ
			Cup5Waiting=0;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=0;
			Cup3Waiting=0;Cup3Runing=0;Cup3Ending=0;Cup3EndingDraw=0;
	 
	   
	 
	 }
	 
	 
void PumpControl()
{
		if(Cup1Status==1)//�б���״̬ȷ���Ƿ�����
							{ 
							 Pump1=1;// 
							}
						 else Pump1=0;//�ر�
					if(Cup3Status==1)//�б���״̬ȷ���Ƿ�����
							{ 
							 Pump3=1;// 
							}
						 else Pump3=0;//�ر�
					if(Cup5Status==1)//�б���״̬ȷ���Ƿ�����
							{ 
							 Pump5=1;// 
							}
						 else Pump5=0;//�ر�
}

void CupDet()
 {
	
   
/******************************CUP1���ܴ���***********************************/
/******************************CUP1���ܴ���***********************************/
if(Cup1Eff==1)
{
     if(Cup1Waiting)                                                       //����ϵͳ�Լ콫cup1waiting ��1��
	   { 
			 Cup1Waiting=1;Cup1Runing=0;Cup1Ending=0;Cup1EndingDraw=0;GLED1=ON; //������״̬|������|GLED1��
		 
			 if(SensorCup(1)==Exist)                                           //��⵽�б��ӷ���|���ú���SensorCup ����exitdataֵ  Exist��ֵΪ1
        {
					Cup1Runing=1;                                                 //��ʼ��־��1
					Cup1Waiting=0;                                               //ֹͣ�ȴ�
          GLED1=OFF;                                                  //GLED1�ر�
					ScanCounter1=0;
					TimerOut1=0;
        } 		
		 }	
		 
     if(Cup1Runing)                                              //����ʱ�ж�����ˮ��
		 {
			 Cup1Waiting=0;Cup1Runing=1;Cup1Ending=0;Cup1EndingDraw=0;
			 RLED1=ON;
		   Cup1Status=1;                                          //����
			// Pump1=1;
      if (ScanCounter1>=WaitingWaterTimeOut&&(TimerOut1==0)) //�ȴ���ˮʱ��200 10s  400 20s �϶�������ˮ  WaitingWaterTimeOut = 22
	        { 	
			     ScanCounter1=0;
				   TimerOut1=1;
			    }
					
					
			if(Water1Sensor==0)                               //��ˮ Water1Sensor��Ӵ������ж�
						 { Timer10MSCounter11++;                     //��ˮ ������
							                                             //  TX1_write2buff(Timer10MSCounter11);  
						 
							 if(Timer10MSCounter11>=SensorWaterTimeOut)     //SensorWaterTimeOut = 28 �ڶ�����ˮ����ˮ���������
                 { ExitData1=0;Timer10MSCounter11=0; }                
						 } 
      if(Water1Sensor==1)                                       //��ˮ
             {ExitData1=1; Timer10MSCounter11=0;
             } 
		
//ǰ3��IF ׼���ж�״̬  //��exitdata Ϊ��ˮʱ�ĵ���NoExist��=1 �� TimerOut1=1
						 
     if((( ExitData1==NoExist)&&(TimerOut1))||SensorCupOut(1)==NoExist )//��⵽�� ˮ��  ����ֱ���ñ��Ӻ�ͣ�� 
    
           { 
						//PrintString1("No1\r\n"); 
            TimerOut1=1;	 
					  Cup1Runing=0;
					  Cup1Ending=1;
					  RLED1=OFF;	
            Cup1Status=VALVEOFF;            //��ͣ��Ҫִ�к�������ͣ
						Pump1=0;                         //��ͣ��;  
						Delay50ms();  
						EXValve1=VALVEON;               //�ſշ�����
						
				    Delay1000ms();                 //��ʱ1s
						 
						EXValve1=VALVEOFF;            //�ط���
						 
   
           }else {TimerOut1=0; Cup1Runing=1; }
         					 
     }
	  if(Cup1Ending)
		  { 
			  TimerOut1=0;
		    Cup1Waiting=0;Cup1Runing=0;Cup1Ending=1;Cup1EndingDraw=0;
				
				GLED1=~GLED1;
				RLED1=~RLED1;
			 // PrintString1("C1End \r\n"); 
			 
       if(SensorCupOut(1)==NoExist) //��⵽�����Ѿ��ÿ�|���д�������led��ȫ���ص�ԭʼ״̬|������־Ϊ��
        { 
           SurplusCount+=1;//��������ʣ�౭�Ӽ�������1
					 Cup1Eff=0;
					 MasterDataBuff[Conf1]=0x00;
					 RX1_Buffer[30]=0x00;//������ʹ������ֵ���㣬�˱���ֻ��ʹ��һ��
					 Cup1Waiting=0;
					 Cup1Runing=0;
					 Cup1Ending=0;
					 Cup1EndingDraw=1;//������־
           RLED1=OFF;GLED1=OFF;  //�����ÿ�����������״̬��Ϩ��
					 ScanCounter1=0;
         } 
     }
		 
		if(Cup1EndingDraw)
			{
					//��⵽�����뿪�󣬳�ȡXMSʱ��
					if(ScanCounter1<=CupOffDrawinWaterTimes)//���ʱ�䣨ScanCounter1*500Ms<=6*100MS�������뿪���ˮʱ�� 100MS
					  {	
							RLED1=~RLED1;
			       // Valve1=VALVEON;//����
			        Cup1Status=VALVEON;//����
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
         //  Valve1=VALVEOFF;//�ط�1
					 Cup1Waiting=1;//�ָ�״̬�ȴ�
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
         //  Valve1=VALVEOFF;//�ط�1
					 Cup1Waiting=1;//�ָ�״̬�ȴ�
					 Cup1Runing=0;
					 Cup1Ending=0;
					 Cup1EndingDraw=0;
           RLED1=OFF;GLED1=OFF;
					 ScanCounter1=0;
					}
		
/******************************CUP3���ܴ���***********************************/\
if(Cup3Eff==1)
{
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
           SurplusCount+=1;//��������ʣ�౭�Ӽ�������1
					MasterDataBuff[Conf3]=0x00;
					RX1_Buffer[31]=0x00;//������ʹ������ֵ���㣬�˱���ֻ��ʹ��һ��
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
/******************************CUP3*******************************************/
			
/******************************CUP5���ܴ���***********************************/
if(Cup5Eff==1)
{
	if(Cup5Waiting)
	   { 
			Cup5Waiting=1;Cup5Runing=0;Cup5Ending=0;Cup5EndingDraw=0;GLED5=ON;

		//	PrintString1("C1Wait \r\n"); 
		 
			if(SensorCup(5)==Exist)  //��⵽�б��ӷ���
        {
					Cup5Runing=1;
					Cup5Waiting=0;
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
			
			 
       if(SensorCupOut(5)==NoExist) //��⵽�����Ѿ��ÿ�
        { 
           SurplusCount+=1;//��������ʣ�౭�Ӽ�������1
					 MasterDataBuff[Conf5]=0x00;
					 RX1_Buffer[32]=0x00;//������ʹ������ֵ���㣬�˱���ֻ��ʹ��һ��
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
					 ScanCounter5=0;
					
					}
      }
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
					 ScanCounter5=0;
					}
/******************************CUP5***********************************/					

/********************************************************************/	

///********************************************************************/
			Confine();//��������ʣ�������һ	
			SlaveData();//���״ֵ̬����SlaveDatabuff�� 
	} //time0��ʱ500ms��������
	
/********************************************************************/	

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
							 if(Timer10MSCounter1>=SensorCupTimerOut){ExitData=1;Timer10MSCounter1=0;} //else {ExitData=0;Timer10MSCounter1��3��=0;}                 
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
 //���ƣ�u8 SensorCupOut(u8 Number)
//���ܣ���Ɑ���ޱ���   100�� 10MS
//������U8 ���ı�����ţ�12345
//����ֵ��U8 �� 1 /  �� 0
//
/**********************************************************************************/
u8 SensorCupOut (u8 NumberN) 
{ u8 ExitData;
  static u16 Timer10MSCounter=0;//�ֲ���̬����
	
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
/**********************************************************************************/


//void SlaveControl()
//{
//    TX1_Buffer[0]=0xAA;  //֡ͷ
//    TX1_Buffer[1]=0x55;  //֡ͷ
//    
//    TX1_Buffer[2]=0x17;   //���ݳ���
//			
//		TX1_Buffer[3]=0x20;    //�豸�����豸
//		TX1_Buffer[4]=0x21;    //�����豸���͸��豸
//			
//		TX1_Buffer[5]=0x00;    //δҪ��Ӧ��
//    TX1_Buffer[6]=0x71;    //����������
//			
//		
//			
//    TX1_Buffer[7]=SlaveDataBuff[Reled1];
//    TX1_Buffer[8]=SlaveDataBuff[Reled3];	//���״̬
//		TX1_Buffer[9]=SlaveDataBuff[Reled5];
//		
//    TX1_Buffer[10]=SlaveDataBuff[Grled1];
//    TX1_Buffer[11]=SlaveDataBuff[Grled3];	//�̵�״̬
//		TX1_Buffer[12]=SlaveDataBuff[Grled5];
//		
//    TX1_Buffer[13]=SlaveDataBuff[CupOne];
//    TX1_Buffer[14]=SlaveDataBuff[CupTwo];	//ˮ������״̬
//		TX1_Buffer[15]=SlaveDataBuff[CupThree];
//    
//		TX1_Buffer[16]=SlaveDataBuff[Water1];
//    TX1_Buffer[17]=SlaveDataBuff[Water3];	//����ˮ���
//		TX1_Buffer[18]=SlaveDataBuff[Water5];
//   
//    TX1_Buffer[19]=SlaveDataBuff[pum1];
//    TX1_Buffer[20]=SlaveDataBuff[pum3];	//��״̬���
//		TX1_Buffer[21]=SlaveDataBuff[pum5];
//    
//    TX1_Buffer[22]=SlaveDataBuff[Valve1];
//    TX1_Buffer[23]=SlaveDataBuff[Valve3];	//������״̬���
//		TX1_Buffer[24]=SlaveDataBuff[Valve5];

//    TX1_Buffer[25]=SlaveDataBuff[Sensor];	//5V��ѹ����
//		TX1_Buffer[26]=0x00;
//  //  TX1_Buffer[26]=(ChassisLock==1)?0x01:0x00;	//������״̬
//		if(RX1_Buffer[6]==0X73)
//		{
//		TX1_Buffer[27]=SlaveDataBuff[Interval];	//ʱ����
//		}
//		else
//		{
//			TX1_Buffer[27]=0x00;
//		}
//    TX1_Buffer[28]=CheckSumTxd(TX1_Buffer,27);//У���	
//		TX1_Buffer[29]=0x55;
//		TX1_Buffer[30]=0xAA;	//֡β
//}



/**********************************************************************************/
//���ƣ�SlaveData()
//���ܣ�״̬�����뱾��������
//������ SlaveDataBuff������������
//       
//       
//����ֵ��void
//--------------------------------
//���ߣ�XiongXiaoping 	
//�޸����ڣ�2020/05/07
//�޸����ݣ����ݲɼ����뷢�ͻ�������
//----------------------------
/**********************************************************************************/
void SlaveData()
{
		SlaveDataBuff[Reled1]=(RLED1==ON)?0x01:0x00;
    SlaveDataBuff[Reled3]=(RLED3==ON)?0x01:0x00;	//���״̬
		SlaveDataBuff[Reled5]=(RLED5==ON)?0x01:0x00;
		
    SlaveDataBuff[Grled1]=(GLED1==ON)?0x01:0x00;
    SlaveDataBuff[Grled3]=(GLED3==ON)?0x01:0x00;	//�̵�״̬
		SlaveDataBuff[Grled5]=(GLED5==ON)?0x01:0x00;
		
    SlaveDataBuff[CupOne]=(Cup1==1)?0x01:0x00;
    SlaveDataBuff[CupTwo]=(Cup3==1)?0x01:0x00;	//ˮ������״̬
		SlaveDataBuff[CupThree]=(Cup5==1)?0x01:0x00;
    
		SlaveDataBuff[Water1]=(Water1Sensor==1)?0x01:0x00;
    SlaveDataBuff[Water3]=(Water3Sensor==1)?0x01:0x00;	//����ˮ���
		SlaveDataBuff[Water5]=(Water5Sensor==1)?0x01:0x00;
   
    SlaveDataBuff[pum1]=(Pump1==1)?0x01:0x00;
    SlaveDataBuff[pum3]=(Pump3==1)?0x01:0x00;	//��״̬���
		SlaveDataBuff[pum5]=(Pump5==1)?0x01:0x00;
    
    SlaveDataBuff[Valve1]=(EXValve1==1)?0x01:0x00;
    SlaveDataBuff[Valve3]=(EXValve3==1)?0x01:0x00;	//������״̬���
		SlaveDataBuff[Valve5]=(EXValve5==1)?0x01:0x00;

    SlaveDataBuff[Sensor]=(CapSensor==1)?0x01:0x00;	//5V��ѹ����
		SlaveDataBuff[Lock]=0x00;//������״̬
   
		//SlaveDataBuff[Interval]=0x00;	//ʱ����
	
		SlaveDataBuff[TimH]=EEPROM_read_n(0x0001);
		SlaveDataBuff[TimL]=EEPROM_read_n(0x0000);	//  ��ȡ����ʣ�����
}	





//u8  CheckSumTxd(u8 *dat,u8 length)
//{
//	u8 ResultCode=TX1_Buffer[2],m;//�ӳ��ȿ�ʼЧ��
//	for(m=3;m<=length;m++)
//			{
//			ResultCode ^= dat[m];//ÿ���ֽ������
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
