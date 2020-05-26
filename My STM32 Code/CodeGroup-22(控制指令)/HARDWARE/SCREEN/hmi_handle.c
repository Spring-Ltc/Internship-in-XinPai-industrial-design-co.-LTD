

#include "hmi_handle.h"


#include "led.h"
#include "delay.h"
#include "sys.h"

#include "timer.h"
#include "gprs.h"
#include "slave.h"
#include "code.h"
#include "user.h"
#include "adc.h"
#include "lock.h"
#include "lan.h"

#include "hmi_driver.h"
#include "hmi_user_uart.h"
#include "cmd_queue.h"
#include "cmd_process.h"
#include "stdio.h"
#include "hw_config.h"
#include "ulitity.h"
#include "string.h"



#define TIME_100MS 10                                                                //100����(10����λ)

volatile uint32  timer_tick_count = 0;                                               //��ʱ������

uint8  cmd_buffer[CMD_MAX_SIZE];                                                     //ָ���
char  PW[10];    
static uint16 current_screen_id = 0;                                                 //��ǰ����ID
//static int32 progress_value = 0;                                                     //����������ֵ
//static int32 test_value = 0;                                                         //����ֵ
//static uint8 update_en = 0;                                                          //���±��
//static int32 meter_flag = 0;                                                         //�Ǳ�ָ��������־λ
//static int32 num = 0;                                                                //���߲��������
//static int sec = 1;                                                                  //ʱ����
//static int32 curves_type = 0;                                                        //���߱�־λ  0Ϊ���Ҳ���1Ϊ��ݲ�                  
//static int32 second_flag=0;                                                          //ʱ���־λ
//static int32 icon_flag = 0;                                                          //ͼ���־λ
//static uint8 Select_H ;                                                              //����ѡ��Сʱ
//static uint8 Select_M ;                                                              //����ѡ����� 
//static uint8 Last_H ;                                                                //��һ��ѡ��Сʱ
//static uint8 Last_M;                                                                 //��һ��ѡ����� 
//static int32 Progress_Value = 0;                                                     //��������ֵ 

void UpdateUI(void);                                                                 //����UI����
void SensorStatusJudgment(void);                                                     //��ʾ�ӿذ崫����״̬

void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str);


//	 static int32 test_value = 0;                                                     //����ֵ
   uint32 timer_tick_last_update = 0;                                               //��һ�θ��µ�ʱ��
   qsize  size = 0; 




u8 FunctionCode = 0x00;

//char SetMaintainerPasswordBuf[10]="12345678";
char NewIPBuf[20], NewPortBuf[10], NewIPAndPortBuf[30];
char UserIDBuf[4], UserPasswordBuf[4], UserIDAndPasswordBuf[8];
char EquipmentNoAndPasswordBuf[16];
char GetMaintainerPasswordBuf[10];
char UserChangePasswordBuf1[4], UserChangePasswordBuf2[4], UserChangePasswordBuf[4];


//int OnceFlag = 0;



int HMITtoBCTFlag = 0;
int HMITtoAPTFlag = 0;
int HMITtoMSTGetGprsStateFlag = 0;
int HMITtoLKTFlag = 0;
int HMITtoBSTFlag = 0;
int HMITtoUSTTranspondBarCodeFlag = 0;
int HMITtoMSTSetGprsPramFlag = 0;
int HMITtoBSTGetSensorStateFlag = 0;
int HMITtoUSTTranspondAccountDataFlag = 0;
int HMITtoMSTRequestSynSeriesNumFlag = 0;

u8 TIM4Flag20Ms3 = 0;
u8 TIM4Flag300Ms4 = 0;



void HMITask(void)
{
	size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);                    //����������һֱ��⻺����ָ����������յ�ָ�����������ȡ  
	      
  if(size>0&&cmd_buffer[1]!=0x07)                                    //�������������յ�ָ�� �����жϲ��ǿ�����ʾ
  {                                                                           
    ProcessMessage((PCTRL_MSG)cmd_buffer, size);                   //��������������ָ�Ȼ����ö�Ӧָ�����͵ĺ�������ָ���������Ϊ�����������õĺ�����
	}
  else if(size>0&&cmd_buffer[1]==0x07)                               //�������������Ϊָ��0x07��������STM32  �� ��ʼ��ĳЩ����
  {                                                                           
		SetTextValue(6, 6, (uchar *)EquipmentNo);  //��ʼ����ʾ�豸���
		SensorStatusJudgmentInit();  //��ʼ����ʾ�豸״̬��Ϣ
//		AnimationStart(22, 23);  //������ͼ�꣩
		
		__disable_fault_irq();                                                   
    NVIC_SystemReset();  
		
				
  } 
	
	//��ʼ��ĳЩ���棨ֻ��������һ�Σ�
//	if(OnceFlag == 0)
//		{
//			SetTextValue(6, 6, (uchar *)EquipmentNo);  //��ʼ����ʾ�豸���
//			SensorStatusJudgmentInit();  //��ʼ����ʾ�豸״̬��Ϣ
//			AnimationStart(22, 23);  //������ͼ�꣩
//			OnceFlag = 1;
//		}

	
	
	//��ʱˢ����Ļ
	if(TIM4Flag10Ms3 == 1) 
  {   
		TIM4Flag10Ms3 = 0;
		if((TIM4Flag20Ms3++) >= 2)
		{
			TIM4Flag20Ms3 = 0;
			UpdateUI(); 
		}
  }	
	
	
	//������ĻID�Ƿ�����
	if(current_screen_id == 25) 
			GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	else if(current_screen_id != 25)
			GPIO_SetBits(GPIOC,GPIO_Pin_2);
	
}






/*! 
*  \brief  ��Ϣ��������
*  \param msg ��������Ϣ
*  \param size ��Ϣ����
*/
void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
    uint8 cmd_type = msg->cmd_type;                                                  //ָ������
    uint8 ctrl_msg = msg->ctrl_msg;                                                  //��Ϣ������
    uint8 control_type = msg->control_type;                                          //�ؼ�����
    uint16 screen_id = PTR2U16(&msg->screen_id);                                     //����ID
    uint16 control_id = PTR2U16(&msg->control_id);                                   //�ؼ�ID
    uint32 value = PTR2U32(msg->param);                                              //��ֵ

    switch(cmd_type)
    {  
    case NOTIFY_TOUCH_PRESS:                                                        //����������
    case NOTIFY_TOUCH_RELEASE:                                                      //�������ɿ�
        NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4)); 
        break;                                                                    
    case NOTIFY_WRITE_FLASH_OK:                                                     //дFLASH�ɹ�
        NotifyWriteFlash(1);                                                      
        break;                                                                    
    case NOTIFY_WRITE_FLASH_FAILD:                                                  //дFLASHʧ��
        NotifyWriteFlash(0);                                                      
        break;                                                                    
    case NOTIFY_READ_FLASH_OK:                                                      //��ȡFLASH�ɹ�
        NotifyReadFlash(1,cmd_buffer+2,size-6);                                     //ȥ��֡ͷ֡β
        break;                                                                    
    case NOTIFY_READ_FLASH_FAILD:                                                   //��ȡFLASHʧ��
        NotifyReadFlash(0,0,0);                                                   
        break;                                                                    
    case NOTIFY_READ_RTC:                                                           //��ȡRTCʱ��
        NotifyReadRTC(cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8]);
        break;
    case NOTIFY_CONTROL:
        {
            if(ctrl_msg==MSG_GET_CURRENT_SCREEN)                                    //����ID�仯֪ͨ
                NotifyScreen(screen_id);                                            //�����л������ĺ���
            else
            {
                switch(control_type)
                {
                case kCtrlButton:                                                   //��ť�ؼ�
                    NotifyButton(screen_id,control_id,msg->param[1]);                  
                    break;                                                             
                case kCtrlText:                                                     //�ı��ؼ�
                    NotifyText(screen_id,control_id,msg->param);                       
                    break;                                                             
                case kCtrlProgress:                                                 //�������ؼ�
                    NotifyProgress(screen_id,control_id,value);                        
                    break;                                                             
                case kCtrlSlider:                                                   //�������ؼ�
                    NotifySlider(screen_id,control_id,value);                          
                    break;                                                             
                case kCtrlMeter:                                                    //�Ǳ�ؼ�
                    NotifyMeter(screen_id,control_id,value);                           
                    break;                                                             
                case kCtrlMenu:                                                     //�˵��ؼ�
                    NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);      
                    break;                                                              
                case kCtrlSelector:                                                 //ѡ��ؼ�
                    NotifySelector(screen_id,control_id,msg->param[0]);                
                    break;                                                              
                case kCtrlRTC:                                                      //����ʱ�ؼ�
                    NotifyTimer(screen_id,control_id);
                    break;
                default:
                    break;
                }
            } 
            break;  
        } 
    case NOTIFY_HandShake:                                                          //����֪ͨ                                                     
        NOTIFYHandShake();
        break;
    default:
        break;
    }
}



/*! 
*  \brief  ����֪ͨ
*/
void NOTIFYHandShake()
{
   SetButtonValue(3,2,1);
}



/*! 
*  \brief  �����л�֪ͨ
*  \details  ��ǰ����ı�ʱ(�����GetScreen)��ִ�д˺���
*  \param screen_id ��ǰ����ID
*/
void NotifyScreen(uint16 screen_id)
{
    current_screen_id = screen_id;           //�ڹ��������п��������л�֪ͨ����¼��ǰ����ID
}



/*! 
*  \brief  ���������¼���Ӧ
*  \param press 1���´�������3�ɿ�������
*  \param x x����
*  \param y y����
*/
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{ 
    //TODO: ����û�����
}



/*! 
*  \brief  ��������
*/ 
void UpdateUI()
{
	//�ı����ú���ʾ  ��ʱ20msˢ��һ��
	
	
	//����Ա ��½ �޸Ľ���
	if(current_screen_id == 2)
	{
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)  //��¼��Ϣ���ͳɹ�
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetScreen(3);   //����Ա�޸�����ɹ�����
		}
	}
	
	//����Ա�޸��������
	if(current_screen_id == 8)
	{
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)  //�޸����뷢�ͳɹ�
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetTextValue(6, 8, (uchar *)UserChangePasswordBuf);  //UserChangePasswordBuf Ϊ�û��޸ĺ������������
			SetScreen(9);   //����Ա�޸�����ɹ�����
		}
	}
	
	//����Ա���Ӳ���Ա����
	if(current_screen_id == 11)
	{
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)  //��¼��Ϣ���ͳɹ�
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetScreen(12);   //����Ա���Ӳ���Ա�ɹ�����
		}
	}
	
	//����Աɾ������Ա����
	if(current_screen_id == 13)
	{
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)  //��¼��Ϣ���ͳɹ�
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetScreen(14);   //����Աɾ������Ա�ɹ�����
		}
	}
	
	//����Ա��½�޸Ľ���
	if(current_screen_id == 15)
	{
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)  //��¼��Ϣ���ͳɹ�
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetScreen(16);   //����Ա�����޸��������
		}
	}
	
	//����Ա�޸��������
	if(current_screen_id == 16)
	{
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)  //�޸����뷢�ͳɹ�
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetScreen(17);   //����Ա�޸�����ɹ�����
		}
	}
	
	//ά����Աѡ�����
	if(current_screen_id == 19)  
	{
		HMITtoBCTFlag = 0;
		HMITtoAPTFlag = 0;
		HMITtoMSTGetGprsStateFlag = 0;
		HMITtoBSTFlag = 0;
		HMITtoLKTFlag = 0;
	}	
	
	//���߹�������
	if(current_screen_id == 20)  
	{
		HMITtoBCTFlag = 1;
		if(BCTtoHMITFlag == 1)
		{
			SetTextValue(20, 1, (uchar *)BarCodeBuf);     //��������ֵ��ʾ�ڹ��������1�ſ�λ��
			BCTtoHMITFlag = 0;
		}
			
		HMITtoAPTFlag = 1;
		if(APTtoHMITFlag == 1)
		{
			SetProgressValue(20, 2, -(int)AP1);
			SetProgressValue(20, 9, -(int)AP2);
			SetProgressValue(20, 16, -(int)AP3);    //��ѹֵ������
			APTtoHMITFlag = 0;
		}
	}
	
	//�豸״̬����1
	if(current_screen_id == 21)   
	{
		HMITtoBCTFlag = 0;
		HMITtoAPTFlag = 0;
		
		if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_4) == 1)  //ɨ������������
			SetTextValue(21, 5, (uchar *)"��"); 
		else if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_4) == 0)  //ɨ������������
		{
			SetTextValue(21, 5, (uchar *)"��"); 
			
			HMITtoBCTFlag = 1;
			if(BCTtoHMITFlag == 1)
			{
				SetTextValue(21, 6, (uchar *)BarCodeBuf);     //��������ֵ��ʾ��
				BCTtoHMITFlag = 0;
			}
		}
		
		if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_8) == 1)  //�������������
			SetTextValue(21, 7, (uchar *)"��"); 
		else if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_8) == 0)  //�������������
			SetTextValue(21, 7, (uchar *)"��"); 
		
		
//		HMITtoMSTGetGprsStateFlag = 1;
//		if(MSTtoHMITGetGprsStateSuccessFlag == 1)
//		{
//			SetTextInt32(21,3,Progress_Value,0,6);
//			MSTtoHMITGetGprsStateSuccessFlag = 0;
//		}
		
	}
	
	//�豸״̬����2                    
  if(current_screen_id == 22)                            
  {
    HMITtoBCTFlag = 0;
		HMITtoMSTGetGprsStateFlag = 0;
		
		
		HMITtoAPTFlag = 1;
		if(APTtoHMITFlag == 1)
		{
			SetTextInt32(22, 7, -(int)AP1, 1, 2);    
			SetTextInt32(22, 14, -(int)AP2, 1, 2);  
			SetTextInt32(22, 21, -(int)AP3, 1, 2);     //������ѹ��������ֵ
			APTtoHMITFlag = 0;
		} 
		
		
		HMITtoBSTGetSensorStateFlag = 1;
		if(BSTtoHMITGetSensorStateSuccessFlag == 1)
		{
			SensorStatusJudgment();
		}
  }	
	
	//ά����Ա��������
	if(current_screen_id == 23)  
	{
		HMITtoLKTFlag = 1;
	}
	
	//����Ա��½��������
	if(current_screen_id == 24)  
	{
		HMITtoBCTFlag = 0;  //ɨ����������
		HMITtoAPTFlag = 0;  //��ѹ��������
		
		
		
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetScreen(37);   //����Ա��½�����ɹ�����
		}
	}	
	
	//��������
	if(current_screen_id == 25)  
	{
		HMITtoBCTFlag = 1;
		if(BCTtoHMITFlag == 1)
		{
			SetTextValue(25, 1, (uchar *)BarCodeBuf);     //��������ֵ��ʾ�ڹ��������1�ſ�λ��
			BCTtoHMITFlag = 0;
			
			//֪ͨ����������ת����ǰ�˱����к�
			HMITtoUSTTranspondBarCodeFlag = 1;
			if(USTtoHMITTranspondBarCodeSuccessFlag == 1)
			{
				USTtoHMITTranspondBarCodeSuccessFlag = 0;
			}
		}
		if(USTtoHMITComparisonBarCodeSuccessFlag == 1)  //�յ��û����������ص����кűȶ����ݰ�
		{
			if(USTtoLANBuf[7] == 0x00)
				AnimationPlayFrame(25, 3, 2);
			else if(USTtoLANBuf[7] == 0x01)
				AnimationPlayFrame(25, 3, 1);
			USTtoHMITComparisonBarCodeSuccessFlag = 0;
		}
		
		//ʵʱ�ɼ���ѹֵ
		HMITtoAPTFlag = 1;
		if(APTtoHMITFlag == 1)
		{
			SetProgressValue(25, 2, -(int)AP1);
			SetProgressValue(25, 9, -(int)AP2);
			SetProgressValue(25, 16, -(int)AP3);    //��ѹֵ������
			APTtoHMITFlag = 0;
		}
	}
}



/*! 
*  \brief  ��ť�ؼ�֪ͨ
*  \details  ����ť״̬�ı�(�����GetControlValue)ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param state ��ť״̬��0����1����
*/
void NotifyButton(uint16 screen_id, uint16 control_id, uint8  state)
{ 
    //TODO: ����û�����
	
	//ͬ��װ�����кŰ���
	if(screen_id==4&&control_id==1)
	{
		FunctionCode = 0x50;
		HMITtoMSTRequestSynSeriesNumFlag = 1;
	}
	
	//ά������ɨ��������
	if(screen_id==21&&control_id==8&&state==0&&HMITtoBCTFlag == 1)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
	}
	else if(screen_id==21&&control_id==8&&state==1&&HMITtoBCTFlag == 1)
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	}
	
	//ά����Ա���������
	if(HMITtoLKTFlag == 1)
	{
		if(screen_id==23&&control_id==2&&state==0)
		{
			GPIO_SetBits(GPIOC, GPIO_Pin_8);
		}
		else if(screen_id==23&&control_id==2&&state==1)
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_8);
		}
	}		
	
	//����Ա���� ��½ �޸İ���
	if(screen_id==2&&control_id==2)
	{
		int i=0, j;
		u8 EquipmentNoBuf[12]=EquipmentNo;
		memset(EquipmentNoAndPasswordBuf, 0, sizeof(EquipmentNoAndPasswordBuf));
		for(i=0; i<12; i++)
			EquipmentNoAndPasswordBuf[i] = EquipmentNoBuf[i];
		for(j=0; j<4; j++)
			EquipmentNoAndPasswordBuf[j+12] = UserPasswordBuf[j];
		FunctionCode = 0x54;
		HMITtoUSTTranspondAccountDataFlag = 1;
	}
	
	//����Ա����IP�Ͷ˿ں���ȷ�ϼ�����IP�Ͷ˿ںźϲ�ΪNewIPAndPort������ʾ��screen_id 6���ı�����
	if(screen_id==7&&control_id==3)
	{
		int i = 0, j;
		memset(NewIPAndPortBuf, 0, sizeof(NewIPAndPortBuf));
		NewIPAndPortBuf[i]='"';
		for(i=1; i<sizeof(NewIPBuf); i++)
		{
			NewIPAndPortBuf[i]=NewIPBuf[i-1];
			if(NewIPAndPortBuf[i]==0x00)
				break;
		}
		NewIPAndPortBuf[i] = '"'; i++;
		NewIPAndPortBuf[i] = ','; i++;
		for(j=0; j<sizeof(NewPortBuf); j++)
		{
			NewIPAndPortBuf[i+j]=NewPortBuf[j];
			if(NewIPAndPortBuf[i+j] == 0x00)
				break;
		}
		SetTextValue(6, 7, (uchar *)NewIPAndPortBuf);
	}
	
	//����Ա�������Ӳ���Ա�����ӡ�����
	if(screen_id==11&&control_id==2)
	{
		FunctionCode = 0x52;
		HMITtoUSTTranspondAccountDataFlag = 1;
	}
	
	//����Ա����ɾ������Ա��ɾ��������
	if(screen_id==13&&control_id==2)
	{
		FunctionCode = 0x53;
		HMITtoUSTTranspondAccountDataFlag = 1;
	}
	
	//����Ա ��½ �޸İ���ȷ�ϼ�����ID������ϲ�ΪUserIDAndPasswordBuf
	if(screen_id==15&&control_id==3)
	{
		int i, j;
		memset(UserIDAndPasswordBuf, 0, sizeof(UserIDAndPasswordBuf));
		for(i=0; i<sizeof(UserIDBuf); i++)
		{
			UserIDAndPasswordBuf[i] = UserIDBuf[i];
		}
		for(j=0; j<sizeof(UserPasswordBuf); j++)
		{
			UserIDAndPasswordBuf[i+j] = UserPasswordBuf[j];
		}
		FunctionCode = 0x55;
		HMITtoUSTTranspondAccountDataFlag = 1;
	}
	
	//����Ա ��½ ��������ȷ�ϼ�����ID������ϲ�ΪUserIDAndPasswordBuf
	if(screen_id==24&&control_id==2)
	{
		int i, j;
		memset(UserIDAndPasswordBuf, 0, sizeof(UserIDAndPasswordBuf));
		SetTextValue(25, 23, (uchar *)UserIDBuf);  //��ʾ����ԱID
		for(i=0; i<sizeof(UserIDBuf); i++)
		{
			UserIDAndPasswordBuf[i] = UserIDBuf[i];
		}
		for(j=0; j<sizeof(UserPasswordBuf); j++)
		{
			UserIDAndPasswordBuf[i+j] = UserPasswordBuf[j];
		}
		FunctionCode = 0x55;
		HMITtoUSTTranspondAccountDataFlag = 1;
	}
	
	//ά����Ա��½����ȷ�ϼ�
	if(screen_id==18&&control_id == 3)
	{
		long Password;
		sscanf(GetMaintainerPasswordBuf, "%ld", &Password);             //���ַ���ת��Ϊ����
		if(Password == SetMaintainerPassword)
			SetScreen(19);  //������ȷ
		else
			SetScreen(34);  //�������
		memset(GetMaintainerPasswordBuf, 0, sizeof(GetMaintainerPasswordBuf));
	}
	
	//����Ա�����޸�����ȷ�ϼ�
	if(screen_id==8&&control_id==3)
	{
		int i=0, j=0;
		memset(UserChangePasswordBuf, 0, sizeof(UserChangePasswordBuf));
		for(i=0; i<4; i++)
		{
			if(UserChangePasswordBuf1[i] == UserChangePasswordBuf2[i])
			{
				j++;
			}
			else if(UserChangePasswordBuf1[i] != UserChangePasswordBuf2[i])
				break;
		}
		if(j==4)
		{
			for(i=0; i<4; i++)
			{
				UserChangePasswordBuf[i] = UserChangePasswordBuf1[i];
			}
			FunctionCode = 0x56;
			HMITtoUSTTranspondAccountDataFlag = 1;
		}
		else
			SetScreen(29);
	}
	
	//����Ա�����޸�����ȷ�ϼ�
	if(screen_id==16&&control_id==3)
	{
		int i=0, j=0;
		int m, n;
		memset(UserChangePasswordBuf, 0, sizeof(UserChangePasswordBuf));
		for(i=0; i<4; i++)
		{
			if(UserChangePasswordBuf1[i] == UserChangePasswordBuf2[i])
			{
				j++;
			}
			else if(UserChangePasswordBuf1[i] != UserChangePasswordBuf2[i])
				break;
		}
		if(j==4)
		{
			for(i=0;i<4;i++)
			{
				UserChangePasswordBuf[i] = UserChangePasswordBuf1[i];   //UserChangePasswordBuf Ϊ�û��޸ĺ������������
			}
			memset(UserIDAndPasswordBuf, 0, sizeof(UserIDAndPasswordBuf));
			for(m=0; m<sizeof(UserIDBuf); m++)
			{
				UserIDAndPasswordBuf[m] = UserIDBuf[m];
			}
			for(n=0; n<sizeof(UserPasswordBuf); n++)
			{
				UserIDAndPasswordBuf[m+n] = UserChangePasswordBuf[n];  //������Ա�޸ĺ������ͬID��һ����Ϊ UserIDAndPasswordBuf
			}
			FunctionCode = 0x57;
			HMITtoUSTTranspondAccountDataFlag = 1;
		}
		else
			SetScreen(33);
	}
	
}



/*! 
*  \brief  �ı��ؼ�֪ͨ
*  \details  ���ı�ͨ�����̸���(�����GetControlValue)ʱ��ִ�д˺���
*  \details  �ı��ؼ����������ַ�����ʽ�·���MCU������ı��ؼ������Ǹ���ֵ��
*  \details  ����Ҫ�ڴ˺����н��·��ַ�������ת�ظ���ֵ��
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param str �ı��ؼ�����
*/
void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str)
{
	if(screen_id==2&&control_id==1)    
	{                                                
		
		int i=0;
		memset(UserPasswordBuf, 0, sizeof(UserPasswordBuf));
		for(i=0;i<4;i++)
		{
			UserPasswordBuf[i] = str[i];
		}
	}               
	
	//����Ա�����µ��û�������IP��ַ
	if(screen_id==7&&control_id==1)    
	{                                                
		
		int i=0;     
		memset(NewIPBuf, 0, sizeof(NewIPBuf));
		for(i=0;i<20;i++)
		{
			NewIPBuf[i] = str[i];
			if(NewIPBuf[i] == 0x00)
			{
				break;
			}
					
		}
	}   
	//����Ա�����µ��û��������˿ں�
	if(screen_id==7&&control_id==2)    
	{         
		int i=0;     
		memset(NewPortBuf, 0, sizeof(NewPortBuf));
		for(i=0; i<10; i++)
		{
			NewPortBuf[i] = str[i];
			if(NewPortBuf[i] == 0x00)
			{
				break;
			}
		}
	}
	
	//�����˻�ID
	if((screen_id==24&&control_id==3)||
		 (screen_id==15&&control_id==1)||
		 (screen_id==11&&control_id==1)||
		 (screen_id==13&&control_id==1))    //��24��3������Ա��½�������棻��15��1������Ա��½�޸ģ�����11��1�����Ӳ���Ա����13��1��ɾ������Ա
	{
		int i=0;
		memset(UserIDBuf, 0, sizeof(UserIDBuf));
		for(i=0; i<4; i++)
		{
			UserIDBuf[i] = str[i];
		}
	}
	//����Ա������Ա��������
	if((screen_id==24&&control_id==4)||
		 (screen_id==15&&control_id==2)||
		 (screen_id==2&&control_id==1))    //��24��4������Ա��½�������棻��15��2������Ա��½�޸ģ���2��1������Ա��½�޸�
	{
		int i=0;
		memset(UserPasswordBuf, 0, sizeof(UserPasswordBuf));
		for(i=0; i<4; i++)
		{
			UserPasswordBuf[i] = str[i];
		}
	}
	
	//ά����Ա��������
	if(screen_id==18&&control_id==2)    //��18��2��ά����Ա��½��������
	{
		int i=0;
		memset(GetMaintainerPasswordBuf, 0, sizeof(GetMaintainerPasswordBuf));
		for(i=0; i<10; i++)
		{
			GetMaintainerPasswordBuf[i] = str[i];
			if(GetMaintainerPasswordBuf[i] == 0x00)
				break;
		}
	}
	
	//����Ա������Ա�޸�����
	if(screen_id==8||screen_id==16)
	{
		if(control_id == 1)
		{
			int i=0;
			memset(UserChangePasswordBuf1, 0, sizeof(UserChangePasswordBuf1));
			for(i=0; i<4; i++)
			{
				UserChangePasswordBuf1[i] = str[i];
			}
		}
		if(control_id == 2)
		{
			int i=0;
			memset(UserChangePasswordBuf2, 0, sizeof(UserChangePasswordBuf2));
			for(i=0; i<4; i++)
			{
				UserChangePasswordBuf2[i] = str[i];
			}
		}
	}
}                                                                       



/*!                                                                              
*  \brief  �������ؼ�֪ͨ                                                       
*  \details  ����GetControlValueʱ��ִ�д˺���                                  
*  \param screen_id ����ID                                                      
*  \param control_id �ؼ�ID                                                     
*  \param value ֵ                                                              
*/                                                                              
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)           
{
	
}


/*!                                                                              
*  \brief  �������ؼ�֪ͨ                                                       
*  \details  ���������ı�(�����GetControlValue)ʱ��ִ�д˺���                  
*  \param screen_id ����ID                                                      
*  \param control_id �ؼ�ID                                                     
*  \param value ֵ                                                              
*/                                                                              
void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)             
{                                                             
//    uchar back[1] = {0};
//    if(screen_id==7&&control_id==2)                                                  //�������
//    {            
//        if(value<100||value>0)                                                       
//        {                                                                            
//            SetProgressValue(7,1,value);                                             //���½�������ֵ
//            SetTextInt32(7,3,value,0,1); 
//            sprintf(back,"%c",(255-value*2));                                        //���ñ������� ����ֵ��Χ 0~255��0������255�
//            SetBackLight(back[0]);
//        }
//    }
//    if(screen_id==7&&control_id==5)                                                  //�������
//    {                                                                              
//        if(value<100||value>0)                                                       
//        {                                                                            
//            SetProgressValue(7,4,value);                                             //���½�������ֵ
//            SetTextInt32(7,6,value,0,1);                                             
//        }
//    }
}



/*! 
*  \brief  �Ǳ�ؼ�֪ͨ
*  \details  ����GetControlValueʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param value ֵ
*/
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
    //TODO: ����û�����
}



/*! 
*  \brief  �˵��ؼ�֪ͨ
*  \details  ���˵���»��ɿ�ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param item �˵�������
*  \param state ��ť״̬��0�ɿ���1����
*/
void NotifyMenu(uint16 screen_id, uint16 control_id, uint8 item, uint8 state)
{
    //TODO: ����û�����
}



/*! 
*  \brief  ѡ��ؼ�֪ͨ
*  \details  ��ѡ��ؼ��仯ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param item ��ǰѡ��
*/
void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{
//    if(screen_id == 13&&control_id == 1)                                //��ȡ��ǰѡ��ؼ���ֵ
//    {
//        Select_H =  item;
//    } 
//    if(screen_id == 13&&control_id == 2)                                //��ȡ��ǰѡ��ؼ���ֵ
//    {
//        Select_M =  item;
//    } 

}



/*! 
*  \brief  ��ʱ����ʱ֪ͨ����
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*/
void NotifyTimer(uint16 screen_id, uint16 control_id)
{
//    if(screen_id==8&&control_id == 7)
//    {
//        SetBuzzer(100);
//    } 
}



/*! 
*  \brief  ��ȡ�û�FLASH״̬����
*  \param status 0ʧ�ܣ�1�ɹ�
*  \param _data ��������
*  \param length ���ݳ���
*/
void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
    //TODO: ����û�����
}



/*! 
*  \brief  д�û�FLASH״̬����
*  \param status 0ʧ�ܣ�1�ɹ�
*/
void NotifyWriteFlash(uint8 status)
{
    //TODO: ����û�����
}



/*! 
*  \brief  ��ȡRTCʱ�䣬ע�ⷵ�ص���BCD��
*  \param year �꣨BCD��
*  \param month �£�BCD��
*  \param week ���ڣ�BCD��
*  \param day �գ�BCD��
*  \param hour ʱ��BCD��
*  \param minute �֣�BCD��
*  \param second �루BCD��
*/
void NotifyReadRTC(uint8 year,uint8 month,uint8 week,uint8 day,uint8 hour,uint8 minute,uint8 second)
{
//    int years,months,weeks,days,hours,minutes;
//       
//    sec    =(0xff & (second>>4))*10 +(0xf & second);                                    //BCD��תʮ����
//    years   =(0xff & (year>>4))*10 +(0xf & year);                                      
//    months  =(0xff & (month>>4))*10 +(0xf & month);                                     
//    weeks   =(0xff & (week>>4))*10 +(0xf & week);                                      
//    days    =(0xff & (day>>4))*10 +(0xf & day);                                      
//    hours   =(0xff & (hour>>4))*10 +(0xf & hour);                                       
//    minutes =(0xff & (minute>>4))*10 +(0xf & minute);                                      
//   
//    SetTextInt32(8,1,years,1,1);
//    SetTextInt32(8,2,months,1,1);
//    SetTextInt32(8,3,days,1,1);
//    SetTextInt32(8,4,hours,1,1);
//    SetTextInt32(8,5,minutes,1,1);
//    SetTextInt32(8,6,sec,1,1);
//    if(weeks == 0)
//    {
//         SetTextValue(8,7,"������");
//    }
//    else if(weeks == 1)
//    {
//         SetTextValue(8,7,"����һ");
//    }
//    else if(weeks == 2)
//    {
//         SetTextValue(8,7,"���ڶ�");
//    }
//    else if(weeks == 3)
//    {
//         SetTextValue(8,7,"������");
//    }
//    else if(weeks == 4)
//    {
//         SetTextValue(8,7,"������"); 
//    }
//    else if(weeks == 5)
//    { 
//         SetTextValue(8,7,"������");
//    }
//    else if(weeks == 6)
//    {
//         SetTextValue(8,7,"������");
//    }
}


void SensorStatusJudgmentInit(void)
{
		//1�ſ�λ
		SetTextValue(22, 3, (uchar *)"�ޱ�");
		SetTextValue(22, 4, (uchar *)"�ù�");
		SetTextValue(22, 5, (uchar *)"��ˮ");
		SetTextValue(22, 8, (uchar *)"����");
		AnimationPlayFrame(22, 9, 0);
	
	//2�ſ�λ
		SetTextValue(22, 10, (uchar *)"�ޱ�");
		SetTextValue(22, 11, (uchar *)"�ù�");
		SetTextValue(22, 12, (uchar *)"��ˮ");
		SetTextValue(22, 15, (uchar *)"����");
		AnimationPlayFrame(22, 16, 0);
	
	//3�ſ�λ
		SetTextValue(22, 17, (uchar *)"�ޱ�");
		SetTextValue(22, 18, (uchar *)"�ù�");
		SetTextValue(22, 19, (uchar *)"��ˮ");
		SetTextValue(22, 22, (uchar *)"����");
		AnimationPlayFrame(22, 23, 0);
}


void SensorStatusJudgment(void)
{

static u8 lastCup1State,lastCup3State,lastCup5State,
					lastPUMP1State,lastPUMP3State,lastPUMP5State,
					lastWater1SensorState,lastWater3SensorState,lastWater5SensorState,
					lastVALVE1State,lastVALVE3State,lastVALVE5State;
	//1�ſ�λ
	if(Cup1State != lastCup1State)
	{
		lastCup1State=Cup1State;
		if(Cup1State==0x00)
			SetTextValue(22, 3, (uchar *)"�ޱ�");
		else if(Cup1State==0x01)		
			SetTextValue(22, 3, (uchar *)"�б�");
	}
	if(PUMP1State != lastPUMP1State)
	{
		lastPUMP1State=PUMP1State;
		if(PUMP1State==0x00)
			SetTextValue(22, 4, (uchar *)"�ù�");
		else if(PUMP1State==0x01)		
			SetTextValue(22, 4, (uchar *)"�ÿ�");
	}
	if(Water1SensorState != lastWater1SensorState)
	{
		lastWater1SensorState=Water1SensorState;
		if(Water1SensorState==0x00)
			SetTextValue(22, 5, (uchar *)"��ˮ");
		else if(Water1SensorState==0x01)		
			SetTextValue(22, 5, (uchar *)"��ˮ");
	}

	if(VALVE1State != lastVALVE1State)
	{
		lastVALVE1State=VALVE1State;
		if(VALVE1State==0x00)
			SetTextValue(22, 8, (uchar *)"����");
		else if(VALVE1State==0x01)		
			SetTextValue(22, 8, (uchar *)"����");
	}
	
	if(RLED1State==0x00&&GLED1State==0x00)
	{
		AnimationStop(22, 9);
		AnimationPlayFrame(22, 9, 0);
	}
	else if(RLED1State==0x00&&GLED1State==0x01)
	{
		AnimationStop(22, 9);
		AnimationPlayFrame(22, 9, 2);
	}
	else if(RLED1State==0x01&&GLED1State==0x00)
	{
		AnimationStop(22, 9);
		AnimationPlayFrame(22, 9, 1);
	}
	else if(RLED1State==0x01&&GLED1State==0x01)
		AnimationStart(22, 9);

	
		//2�ſ�λ
	if(Cup3State != lastCup3State)
	{
		lastCup3State=Cup3State;
		if(Cup3State==0x00)
			SetTextValue(22, 10, (uchar *)"�ޱ�");
		else if(Cup3State==0x01)		
			SetTextValue(22, 10, (uchar *)"�б�");
	}
	if(PUMP3State != lastPUMP3State)
	{
		lastPUMP3State=PUMP3State;
		if(PUMP3State==0x00)
			SetTextValue(22, 11, (uchar *)"�ù�");
		else if(PUMP3State==0x01)		
			SetTextValue(22, 11, (uchar *)"�ÿ�");
	}
	if(Water3SensorState != lastWater3SensorState)
	{
		lastWater3SensorState=Water3SensorState;
		if(Water3SensorState==0x00)
			SetTextValue(22, 12, (uchar *)"��ˮ");
		else if(Water3SensorState==0x01)		
			SetTextValue(22, 12, (uchar *)"��ˮ");
	}

	if(VALVE3State != lastVALVE3State)
	{
		lastVALVE3State=VALVE3State;
		if(VALVE3State==0x00)
			SetTextValue(22, 15, (uchar *)"����");
		else if(VALVE3State==0x01)		
			SetTextValue(22, 15, (uchar *)"����");
	}
	
	if(RLED3State==0x00&&GLED3State==0x00)
	{
		AnimationStop(22, 16);
		AnimationPlayFrame(22, 16, 0);
	}
	else if(RLED3State==0x00&&GLED3State==0x01)
	{
		AnimationStop(22, 16);
		AnimationPlayFrame(22, 16, 2);
	}
	else if(RLED3State==0x01&&GLED3State==0x00)
	{
		AnimationStop(22, 16);
		AnimationPlayFrame(22, 16, 1);
	}
	else if(RLED3State==0x01&&GLED3State==0x01)
		AnimationStart(22, 16);
	
	//3�ſ�λ
	if(Cup5State != lastCup5State)
	{
		lastCup5State=Cup5State;
		if(Cup5State==0x00)
			SetTextValue(22, 17, (uchar *)"�ޱ�");
		else if(Cup5State==0x01)		
			SetTextValue(22, 17, (uchar *)"�б�");
	}
	if(PUMP5State != lastPUMP5State)
	{
		lastPUMP5State=PUMP5State;
		if(PUMP5State==0x00)
			SetTextValue(22, 18, (uchar *)"�ù�");
		else if(PUMP5State==0x01)		
			SetTextValue(22, 18, (uchar *)"�ÿ�");
	}
	if(Water5SensorState != lastWater5SensorState)
	{
		lastWater5SensorState=Water5SensorState;
		if(Water5SensorState==0x00)
			SetTextValue(22, 19, (uchar *)"��ˮ");
		else if(Water5SensorState==0x01)		
			SetTextValue(22, 19, (uchar *)"��ˮ");
	}

	if(VALVE5State != lastVALVE5State)
	{
		lastVALVE5State = VALVE5State;
		if(VALVE5State == 0x00)
			SetTextValue(22, 22, (uchar *)"����");
		else if(VALVE5State==0x01)		
			SetTextValue(22, 22, (uchar *)"����");
	}
	
	if(RLED5State==0x00&&GLED5State==0x00)
	{
		AnimationStop(22, 23);
		AnimationPlayFrame(22, 23, 0);
	}
	else if(RLED5State==0x00&&GLED5State==0x01)
	{
		AnimationStop(22, 23);
		AnimationPlayFrame(22, 23, 2);
	}
	else if(RLED5State==0x01&&GLED5State==0x00)
	{
		AnimationStop(22, 23);
		AnimationPlayFrame(22, 23, 1);
	}
	else if(RLED5State==0x01&&GLED5State==0x01)
		AnimationStart(22, 23);
}

	










