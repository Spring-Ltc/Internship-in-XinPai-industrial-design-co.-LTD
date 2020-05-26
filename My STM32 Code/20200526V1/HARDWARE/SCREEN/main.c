/************************************��Ȩ����********************************************
**                             ���ݴ�ʹ��Ƽ����޹�˾
**                             http://www.gz-dc.com
**-----------------------------------�ļ���Ϣ--------------------------------------------
** �ļ�����:   ulitity.c
** �޸�ʱ��:   2018-05-18
** �ļ�˵��:   �û�MCU��������������
** ����֧�֣�  Tel: 020-82186683  Email: hmi@gz-dc.com Web:www.gz-dc.com
--------------------------------------------------------------------------------------*/
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
static int32 progress_value = 0;                                                     //����������ֵ
static int32 test_value = 0;                                                         //����ֵ
static uint8 update_en = 0;                                                          //���±��
static int32 meter_flag = 0;                                                         //�Ǳ�ָ��������־λ
static int32 num = 0;                                                                //���߲��������
static int sec = 1;                                                                  //ʱ����
static int32 curves_type = 0;                                                        //���߱�־λ  0Ϊ���Ҳ���1Ϊ��ݲ�                  
static int32 second_flag=0;                                                          //ʱ���־λ
static int32 icon_flag = 0;                                                          //ͼ���־λ
static uint8 Select_H ;                                                              //����ѡ��Сʱ
static uint8 Select_M ;                                                              //����ѡ����� 
static uint8 Last_H ;                                                                //��һ��ѡ��Сʱ
static uint8 Last_M;                                                                 //��һ��ѡ����� 
static int32 Progress_Value = 0;                                                     //��������ֵ 

void UpdateUI(void);                                                                 //����UI����

void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str);


/*!                                                                                 
*  \brief  �������                                                                 
*/                                                                                  
int main()                                                                          
{                                                                                   
    static int32 test_value = 0;                                                     //����ֵ
    uint32 timer_tick_last_update = 0;                                               //��һ�θ��µ�ʱ��
    qsize  size = 0;                                                                

    //����ʱ��                                                                    
    Set_System();                                                                   
    //���ô����ж�                                                                
    Interrupts_Config();                                                            
    //����ʱ�ӽ���                                                                
    systicket_init();                                                               
    //���ڳ�ʼ��                                              
    UartInit(9600);                                                               

    //��մ��ڽ��ջ�����                                                          
    queue_reset();                                                                  

    //��ʱ�ȴ���������ʼ�����,����ȴ�300ms                                      
    delay_ms(300);                                                                  

    while(1)                                                                        
    {                                                                               
        size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);                              //һֱ��⻺����ָ����������յ�ָ�����������ȡ         
        if(size>0&&cmd_buffer[1]!=0x07)                                              //���յ�ָ�� �����ж��Ƿ�Ϊ������ʾ
        {                                                                           
            ProcessMessage((PCTRL_MSG)cmd_buffer, size);                             //����ָ�Ȼ����ö�Ӧָ�����͵ĺ�������ָ���������Ϊ�����������õĺ�����
        }                                                                           
        else if(size>0&&cmd_buffer[1]==0x07)                                         //���Ϊָ��0x07��������STM32  
        {                                                                           
            __disable_fault_irq();                                                   
            NVIC_SystemReset();                                                                                                                                          
        }           
				
				
				GetControlValue(17, 11);
				
        //    �����и���ʱ��־λ�����ı䣬��ʱ100����ˢ����Ļ
        if(update_en&&timer_tick_count-timer_tick_last_update>=TIME_1000MS)
        {
            update_en = 0;
            timer_tick_last_update = timer_tick_count;  
					
            UpdateUI();
        }

				
        //����Ϊ����ʾ���㣬ÿ20�����Զ�����һ��
        //��һ���ж�10ms��timer_tick_countֵ+1,100*timer_tick_count = 1s
//        if(timer_tick_count%100==0)
//        {   
//            UpdateUI(); 
//        }
    }  
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
            {
                NotifyScreen(screen_id);                                            //�����л������ĺ���
            }
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
                case kCtrlMeter:                                                    //�Ǳ��ؼ�
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
    //TODO: �����û�����
    current_screen_id = screen_id;                                                   //�ڹ��������п��������л�֪ͨ����¼��ǰ����ID

//    //��������3����һ����ť
//    if(screen_id == 2)
//    {
////        SetButtonValue(3,1,1);
//			SetTextValue(2,1,"�յ�");
//    }
//    //���������Զ�����GIF
//    if(current_screen_id == 9)
//    {
//        AnimationStart(9,1);                                                         //������ʼ����
//    }
//       //���������������ȡ��ǰ�ı�ֵ
//    if(current_screen_id==5)                                   
//    {
//        GetControlValue(5,1);
//    }
//    //������ά��ҳ�����ɶ�ά��
//    if(current_screen_id==14)                                   
//    {
//        //��ά��ؼ���ʾ�����ַ�ʱ����Ҫת��ΪUTF8���룬
//        //ͨ����ָ�����֡���ת����www.gz-dc.com�� ���õ��ַ�����������
//        uint8 dat[] = {0x77,0x77,0x77,0x2E,0x67,0x7A,0x2D,0x64,0x63,0x2E,0x63,0x6F,0x6D};
//        SetTextValue(14,1,dat);                                                      //���Ͷ�ά���ַ�����                     
//    }

//    //���ݼ�¼��ʾ
//    if(current_screen_id == 15)
//    {
//        Record_SetEvent(15,1,0,0);  
//        Record_SetEvent(15,1,1,0);
//        Record_SetEvent(15,1,2,0);
//        Record_SetEvent(15,1,3,0);
//        Record_SetEvent(15,1,4,0);
//        Record_SetEvent(15,1,5,0);
//        Record_SetEvent(15,1,6,0);
//        Record_SetEvent(15,1,7,0);
//        delay_ms(2000);                                                              //��ʱ����
//        Record_ResetEvent(15,1,0,0);
//        Record_ResetEvent(15,1,1,0);
//        Record_ResetEvent(15,1,2,0);

//    }
//    //�������ֻ����Զ�����
//    if(current_screen_id == 17)   
//    {
//         uint8 buffer[6] = {0x90,0x01 ,0x00 ,0x01 ,0x01};
//         SetButtonValue(17,3,1);
//         PlayMusic(buffer);                                                           //��������
//    }
}



/*! 
*  \brief  ���������¼���Ӧ
*  \param press 1���´�������3�ɿ�������
*  \param x x����
*  \param y y����
*/
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{ 
    //TODO: �����û�����
}



/*! 
*  \brief  ��������
*/ 
void UpdateUI()
{
	//�ı����ú���ʾ  ��ʱ20msˢ��һ��
    if(current_screen_id==17)                                              
    {
        //��ǰ�������¶ȴ�0��1000ѭ����ʾ�������ִ�0-999ѭ����ʾ
        SetTextInt32(17,11,10,0,2);                                      //��ǰ����
			delay_ms(1000);
			SetTextInt32(17,11,20,0,2);
        SetTextInt32(4,7,test_value%1000,0,1);                                      //������
        SetTextValue(17,7,"��");                                                 //�����ı�ֵ
			SetTextValue(17,8,"����");  
  
//        test_value++;                                                                
//        if(test_value >= 1000)                                                       
//        {                                                                            
//            test_value = 0;                                                            
//        }                                                                            
//        if(test_value>0&&test_value<500)                                             //����0С��500�ı���ʾ��ɫ
//        {                                                                            
//            SetControlBackColor(4,6,0xF800);                                         //�����ı�����ɫ
//        }                                                                            
//        else if(test_value>=500)                                                     //����500�ı�����ɫ
//        {                                                                            
//            SetControlBackColor(4,6,0x001F);                                         //�����ı�����ɫ
//        }                                                                            
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
    //TODO: �����û�����
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
	if(screen_id==2)                                                                 //����ID2���ı����ú���ʾ
			{                                                                            
					
				int32 value=0;                                                            
					sscanf(str,"%ld",&value);                                                    //���ַ���ת��Ϊ���� 
					if(control_id==1)                                                            //��ߵ�ѹ
					{                                                                         
							//�޶���ֵ��Χ��Ҳ�������ı��ؼ����������ã�                             
							if(value<0)                                                              
							{                                                                        
									value = 0;                                                            
							}      
							if(value == 6666)                                                              
							{                                                                        
									PW[0] = 2;
									PW[2] = 3;
							}    							
							else if(value>9999)                                                       
							{                                                                        
									value = 9999;                                                           
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
//    if(screen_id == 5)
//    {
//        Progress_Value = value;                                  
//        SetTextInt32(5,2,Progress_Value,0,1);                                        //�����ı����ֵ     
//    }    
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
*  \brief  �Ǳ��ؼ�֪ͨ
*  \details  ����GetControlValueʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param value ֵ
*/
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
    //TODO: �����û�����
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
    //TODO: �����û�����
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
    //TODO: �����û�����
}



/*! 
*  \brief  д�û�FLASH״̬����
*  \param status 0ʧ�ܣ�1�ɹ�
*/
void NotifyWriteFlash(uint8 status)
{
    //TODO: �����û�����
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