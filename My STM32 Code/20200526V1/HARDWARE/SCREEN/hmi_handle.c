

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

#include "hmi_driver.h"
#include "hmi_user_uart.h"
#include "cmd_queue.h"
#include "cmd_process.h"
#include "stdio.h"
#include "hw_config.h"
#include "ulitity.h"
#include "string.h"



#define TIME_100MS 10                                                                //100毫秒(10个单位)

volatile uint32  timer_tick_count = 0;                                               //定时器节拍

uint8  cmd_buffer[CMD_MAX_SIZE];                                                     //指令缓存
char  PW[10];    
static uint16 current_screen_id = 0;                                                 //当前画面ID
static int32 progress_value = 0;                                                     //进度条测试值
//static int32 test_value = 0;                                                         //测试值
static uint8 update_en = 0;                                                          //更新标记
static int32 meter_flag = 0;                                                         //仪表指针往返标志位
static int32 num = 0;                                                                //曲线采样点计数
static int sec = 1;                                                                  //时间秒
static int32 curves_type = 0;                                                        //曲线标志位  0为正弦波，1为锯齿波                  
static int32 second_flag=0;                                                          //时间标志位
static int32 icon_flag = 0;                                                          //图标标志位
static uint8 Select_H ;                                                              //滑动选择小时
static uint8 Select_M ;                                                              //滑动选择分钟 
static uint8 Last_H ;                                                                //上一个选择小时
static uint8 Last_M;                                                                 //上一个选择分钟 
static int32 Progress_Value = 0;                                                     //进度条的值 

void UpdateUI(void);                                                                 //更新UI数据

void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str);


//	 static int32 test_value = 0;                                                     //测试值
   uint32 timer_tick_last_update = 0;                                               //上一次更新的时间
   qsize  size = 0; 


extern char BarCodeBuf[18];

char Password[4];
int HMITtoBCTFlag = 0;
int HMITtoAPTFlag = 0;

u8 TIM4Flag20Ms3 = 0;
u8 TIM4Flag300Ms4 = 0;



void HMITask(void)
{
//	HMITtoAPTFlag = 0;
//	HMITtoBCTFlag = 0;
	
	
//		if(current_screen_id == 0) 
//			GPIO_ResetBits(GPIOC,GPIO_Pin_2);
//		else
//			GPIO_SetBits(GPIOC,GPIO_Pin_2);
	

	
	
	size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);                    //（串口屏）一直检测缓冲区指令，若缓冲区收到指令，立即进行提取         
  if(size>0&&cmd_buffer[1]!=0x07)                                    //（串口屏）接收到指令 ，及判断是否为开机提示
  {                                                                           
    ProcessMessage((PCTRL_MSG)cmd_buffer, size);                   //（串口屏）解析指令（然后调用对应指令类型的函数并将指令的数据作为参数传进调用的函数）
	}                                                                           
  else if(size>0&&cmd_buffer[1]==0x07)                               //（串口屏）如果为指令0x07就软重置STM32  
  {                                                                           
    __disable_fault_irq();                                                   
    NVIC_SystemReset();                                                                                                                                          
  } 

	
	if(TIM4Flag10Ms3 == 1) 
  {   
		TIM4Flag10Ms3 = 0;
		if((TIM4Flag20Ms3++) >= 2)
		{
			TIM4Flag20Ms3 = 0;
			UpdateUI(); 
		}
    //delay_ms(20); 
//		UpdateUI(); 
  }	
	
	if(current_screen_id == 0) 
			GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	if(current_screen_id != 0)
			GPIO_SetBits(GPIOC,GPIO_Pin_2);
				
				
//				SetTextValue(22,4,"关"); 
//				GetControlValue(17, 11);   //获取（17，11）的文本内容
				
        //    数据有更新时标志位发生改变，定时100毫秒刷新屏幕
//        if(update_en&&timer_tick_count-timer_tick_last_update>=TIME_100MS)
//        {
//            update_en = 0;
//            timer_tick_last_update = timer_tick_count;  
//					
//            UpdateUI();
//        }

				
        //这里为了演示方便，每20毫秒自动更新一次
	
}






/*! 
*  \brief  消息处理流程
*  \param msg 待处理消息
*  \param size 消息长度
*/
void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
    uint8 cmd_type = msg->cmd_type;                                                  //指令类型
    uint8 ctrl_msg = msg->ctrl_msg;                                                  //消息的类型
    uint8 control_type = msg->control_type;                                          //控件类型
    uint16 screen_id = PTR2U16(&msg->screen_id);                                     //画面ID
    uint16 control_id = PTR2U16(&msg->control_id);                                   //控件ID
    uint32 value = PTR2U32(msg->param);                                              //数值

    switch(cmd_type)
    {  
    case NOTIFY_TOUCH_PRESS:                                                        //触摸屏按下
    case NOTIFY_TOUCH_RELEASE:                                                      //触摸屏松开
        NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4)); 
        break;                                                                    
    case NOTIFY_WRITE_FLASH_OK:                                                     //写FLASH成功
        NotifyWriteFlash(1);                                                      
        break;                                                                    
    case NOTIFY_WRITE_FLASH_FAILD:                                                  //写FLASH失败
        NotifyWriteFlash(0);                                                      
        break;                                                                    
    case NOTIFY_READ_FLASH_OK:                                                      //读取FLASH成功
        NotifyReadFlash(1,cmd_buffer+2,size-6);                                     //去除帧头帧尾
        break;                                                                    
    case NOTIFY_READ_FLASH_FAILD:                                                   //读取FLASH失败
        NotifyReadFlash(0,0,0);                                                   
        break;                                                                    
    case NOTIFY_READ_RTC:                                                           //读取RTC时间
        NotifyReadRTC(cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8]);
        break;
    case NOTIFY_CONTROL:
        {
            if(ctrl_msg==MSG_GET_CURRENT_SCREEN)                                    //画面ID变化通知
                NotifyScreen(screen_id);                                            //画面切换调动的函数
            else
            {
                switch(control_type)
                {
                case kCtrlButton:                                                   //按钮控件
                    NotifyButton(screen_id,control_id,msg->param[1]);                  
                    break;                                                             
                case kCtrlText:                                                     //文本控件
                    NotifyText(screen_id,control_id,msg->param);                       
                    break;                                                             
                case kCtrlProgress:                                                 //进度条控件
                    NotifyProgress(screen_id,control_id,value);                        
                    break;                                                             
                case kCtrlSlider:                                                   //滑动条控件
                    NotifySlider(screen_id,control_id,value);                          
                    break;                                                             
                case kCtrlMeter:                                                    //仪表控件
                    NotifyMeter(screen_id,control_id,value);                           
                    break;                                                             
                case kCtrlMenu:                                                     //菜单控件
                    NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);      
                    break;                                                              
                case kCtrlSelector:                                                 //选择控件
                    NotifySelector(screen_id,control_id,msg->param[0]);                
                    break;                                                              
                case kCtrlRTC:                                                      //倒计时控件
                    NotifyTimer(screen_id,control_id);
                    break;
                default:
                    break;
                }
            } 
            break;  
        } 
    case NOTIFY_HandShake:                                                          //握手通知                                                     
        NOTIFYHandShake();
        break;
    default:
        break;
    }
}



/*! 
*  \brief  握手通知
*/
void NOTIFYHandShake()
{
   SetButtonValue(3,2,1);
}



/*! 
*  \brief  画面切换通知
*  \details  当前画面改变时(或调用GetScreen)，执行此函数
*  \param screen_id 当前画面ID
*/
void NotifyScreen(uint16 screen_id)
{
    //TODO: 添加用户代码
    current_screen_id = screen_id;                                                   //在工程配置中开启画面切换通知，记录当前画面ID

}



/*! 
*  \brief  触摸坐标事件响应
*  \param press 1按下触摸屏，3松开触摸屏
*  \param x x坐标
*  \param y y坐标
*/
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{ 
    //TODO: 添加用户代码
}



/*! 
*  \brief  更新数据
*/ 
void UpdateUI()
{
	//文本设置和显示  定时20ms刷新一次
	
	if(current_screen_id == 20)  //离线工作界面
	{
		HMITtoBCTFlag = 1;
		if(BCTtoHMITFlag == 1)
		{
			SetTextValue(20, 1, (uchar *)BarCodeBuf);     //将条形码值显示在工作界面的1号孔位上
			BCTtoHMITFlag = 0;
		}
			
		HMITtoAPTFlag = 1;
		if(APTtoHMITFlag == 1)
		{
			SetProgressValue(20, 2, -(int)AP1);
			SetProgressValue(20, 9, -(int)AP2);
			SetProgressValue(20, 16, -(int)AP3);    //气压值进度条
			APTtoHMITFlag = 0;
		}
	}
		
  if(current_screen_id==22)   //设备状态界面2                                             
  {
    HMITtoAPTFlag = 1;
		if(APTtoHMITFlag == 1)
		{
			SetTextInt32(22,7,-(int)AP1,1,2);    
			SetTextInt32(22,14,-(int)AP2,1,2);  
			SetTextInt32(22,21,-(int)AP3,1,2);     //三个气压传感器的值
			APTtoHMITFlag = 0;
		}
			
    SetTextValue(22, 3, (uchar *)"无");                                                 //设置文本值
		SetTextValue(22, 4, (uchar *)"工作");                                                                      
  }
	
	if(current_screen_id == 25)  //工作界面
	{
		HMITtoBCTFlag = 1;
		if(BCTtoHMITFlag == 1)
		{
			SetTextValue(25, 1, (uchar *)BarCodeBuf);     //将条形码值显示在工作界面的1号孔位上
			BCTtoHMITFlag = 0;
		}
			
		HMITtoAPTFlag = 1;
		if(APTtoHMITFlag == 1)
		{
			SetProgressValue(25, 2, -(int)AP1);
			SetProgressValue(25, 9, -(int)AP2);
			SetProgressValue(25, 16, -(int)AP3);    //气压值进度条
			APTtoHMITFlag = 0;
		}
	}

	if(current_screen_id == 24)
	{
		//delay_ms(300);
		HMITtoBCTFlag = 0;
		HMITtoAPTFlag = 0;
	}	

}



/*! 
*  \brief  按钮控件通知
*  \details  当按钮状态改变(或调用GetControlValue)时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param state 按钮状态：0弹起，1按下
*/
void NotifyButton(uint16 screen_id, uint16 control_id, uint8  state)
{ 
    //TODO: 添加用户代码
//	if(screen_id==25&&control_id==22)
//	{
//		delay_ms(500);
//		HMITtoBCTFlag = 0;
//		HMITtoAPTFlag = 0;
//		
//		
////		if(TIM4Flag10Ms3 == 1)
////		{
//////			GPIO_ResetBits(GPIOC, GPIO_Pin_1);
////			TIM4Flag10Ms4 = 0;
////			if((TIM4Flag300Ms4++) >= 30)
////			{
////				GPIO_ResetBits(GPIOC, GPIO_Pin_1);
////				TIM4Flag300Ms4 = 0;
////				HMITtoBCTFlag = 0;
////				HMITtoAPTFlag = 0; 
////			}
////		}
//		
//		
//	}
//	if(screen_id==25&&control_id==6)
//	{
//		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
//	}
			
		
}



/*! 
*  \brief  文本控件通知
*  \details  当文本通过键盘更新(或调用GetControlValue)时，执行此函数
*  \details  文本控件的内容以字符串形式下发到MCU，如果文本控件内容是浮点值，
*  \details  则需要在此函数中将下发字符串重新转回浮点值。
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param str 文本控件内容
*/
void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str)
{
	if(screen_id==2)    
	{                                                
		if(control_id==1)       
		{                                                                         
			int32 password=0,i=0;
			sscanf(str,"%ld",&password);             //把字符串转换为整数
			
			memset(Password,0,sizeof(Password));
			for(i=0;i<4;i++)
			Password[i]=str[i];
		}  
	}                                                                  
}                                                                                



/*!                                                                              
*  \brief  进度条控件通知                                                       
*  \details  调用GetControlValue时，执行此函数                                  
*  \param screen_id 画面ID                                                      
*  \param control_id 控件ID                                                     
*  \param value 值                                                              
*/                                                                              
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)           
{
	
		
	
//    if(screen_id == 5)
//    {
//        Progress_Value = value;                                  
//        SetTextInt32(5,2,Progress_Value,0,1);                                        //设置文本框的值     
//    }    
}


/*!                                                                              
*  \brief  滑动条控件通知                                                       
*  \details  当滑动条改变(或调用GetControlValue)时，执行此函数                  
*  \param screen_id 画面ID                                                      
*  \param control_id 控件ID                                                     
*  \param value 值                                                              
*/                                                                              
void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)             
{                                                             
//    uchar back[1] = {0};
//    if(screen_id==7&&control_id==2)                                                  //滑块控制
//    {            
//        if(value<100||value>0)                                                       
//        {                                                                            
//            SetProgressValue(7,1,value);                                             //更新进度条数值
//            SetTextInt32(7,3,value,0,1); 
//            sprintf(back,"%c",(255-value*2));                                        //设置背光亮度 背光值范围 0~255，0最亮，255最暗
//            SetBackLight(back[0]);
//        }
//    }
//    if(screen_id==7&&control_id==5)                                                  //滑块控制
//    {                                                                              
//        if(value<100||value>0)                                                       
//        {                                                                            
//            SetProgressValue(7,4,value);                                             //更新进度条数值
//            SetTextInt32(7,6,value,0,1);                                             
//        }
//    }
}



/*! 
*  \brief  仪表控件通知
*  \details  调用GetControlValue时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param value 值
*/
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
    //TODO: 添加用户代码
}



/*! 
*  \brief  菜单控件通知
*  \details  当菜单项按下或松开时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param item 菜单项索引
*  \param state 按钮状态：0松开，1按下
*/
void NotifyMenu(uint16 screen_id, uint16 control_id, uint8 item, uint8 state)
{
    //TODO: 添加用户代码
}



/*! 
*  \brief  选择控件通知
*  \details  当选择控件变化时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param item 当前选项
*/
void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{
//    if(screen_id == 13&&control_id == 1)                                //获取当前选择控件的值
//    {
//        Select_H =  item;
//    } 
//    if(screen_id == 13&&control_id == 2)                                //获取当前选择控件的值
//    {
//        Select_M =  item;
//    } 

}



/*! 
*  \brief  定时器超时通知处理
*  \param screen_id 画面ID
*  \param control_id 控件ID
*/
void NotifyTimer(uint16 screen_id, uint16 control_id)
{
//    if(screen_id==8&&control_id == 7)
//    {
//        SetBuzzer(100);
//    } 
}



/*! 
*  \brief  读取用户FLASH状态返回
*  \param status 0失败，1成功
*  \param _data 返回数据
*  \param length 数据长度
*/
void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
    //TODO: 添加用户代码
}



/*! 
*  \brief  写用户FLASH状态返回
*  \param status 0失败，1成功
*/
void NotifyWriteFlash(uint8 status)
{
    //TODO: 添加用户代码
}



/*! 
*  \brief  读取RTC时间，注意返回的是BCD码
*  \param year 年（BCD）
*  \param month 月（BCD）
*  \param week 星期（BCD）
*  \param day 日（BCD）
*  \param hour 时（BCD）
*  \param minute 分（BCD）
*  \param second 秒（BCD）
*/
void NotifyReadRTC(uint8 year,uint8 month,uint8 week,uint8 day,uint8 hour,uint8 minute,uint8 second)
{
//    int years,months,weeks,days,hours,minutes;
//       
//    sec    =(0xff & (second>>4))*10 +(0xf & second);                                    //BCD码转十进制
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
//         SetTextValue(8,7,"星期日");
//    }
//    else if(weeks == 1)
//    {
//         SetTextValue(8,7,"星期一");
//    }
//    else if(weeks == 2)
//    {
//         SetTextValue(8,7,"星期二");
//    }
//    else if(weeks == 3)
//    {
//         SetTextValue(8,7,"星期三");
//    }
//    else if(weeks == 4)
//    {
//         SetTextValue(8,7,"星期四"); 
//    }
//    else if(weeks == 5)
//    { 
//         SetTextValue(8,7,"星期五");
//    }
//    else if(weeks == 6)
//    {
//         SetTextValue(8,7,"星期六");
//    }
}


