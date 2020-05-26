/************************************版权申明********************************************
**                             广州大彩光电科技有限公司
**                             http://www.gz-dc.com
**-----------------------------------文件信息--------------------------------------------
** 文件名称:   ulitity.c
** 修改时间:   2018-05-18
** 文件说明:   用户MCU串口驱动函数库
** 技术支持：  Tel: 020-82186683  Email: hmi@gz-dc.com Web:www.gz-dc.com
--------------------------------------------------------------------------------------*/
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
static int32 test_value = 0;                                                         //测试值
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


/*!                                                                                 
*  \brief  程序入口                                                                 
*/                                                                                  
int main()                                                                          
{                                                                                   
    static int32 test_value = 0;                                                     //测试值
    uint32 timer_tick_last_update = 0;                                               //上一次更新的时间
    qsize  size = 0;                                                                

    //配置时钟                                                                    
    Set_System();                                                                   
    //配置串口中断                                                                
    Interrupts_Config();                                                            
    //配置时钟节拍                                                                
    systicket_init();                                                               
    //串口初始化                                              
    UartInit(9600);                                                               

    //清空串口接收缓冲区                                                          
    queue_reset();                                                                  

    //延时等待串口屏初始化完毕,必须等待300ms                                      
    delay_ms(300);                                                                  

    while(1)                                                                        
    {                                                                               
        size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);                              //一直检测缓冲区指令，若缓冲区收到指令，立即进行提取         
        if(size>0&&cmd_buffer[1]!=0x07)                                              //接收到指令 ，及判断是否为开机提示
        {                                                                           
            ProcessMessage((PCTRL_MSG)cmd_buffer, size);                             //解析指令（然后调用对应指令类型的函数并将指令的数据作为参数传进调用的函数）
        }                                                                           
        else if(size>0&&cmd_buffer[1]==0x07)                                         //如果为指令0x07就软重置STM32  
        {                                                                           
            __disable_fault_irq();                                                   
            NVIC_SystemReset();                                                                                                                                          
        }           
				
				
				GetControlValue(17, 11);
				
        //    数据有更新时标志位发生改变，定时100毫秒刷新屏幕
        if(update_en&&timer_tick_count-timer_tick_last_update>=TIME_1000MS)
        {
            update_en = 0;
            timer_tick_last_update = timer_tick_count;  
					
            UpdateUI();
        }

				
        //这里为了演示方便，每20毫秒自动更新一次
        //进一次中断10ms，timer_tick_count值+1,100*timer_tick_count = 1s
//        if(timer_tick_count%100==0)
//        {   
//            UpdateUI(); 
//        }
    }  
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
            {
                NotifyScreen(screen_id);                                            //画面切换调动的函数
            }
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

//    //进到画面3亮起一个按钮
//    if(screen_id == 2)
//    {
////        SetButtonValue(3,1,1);
//			SetTextValue(2,1,"收到");
//    }
//    //进到画面自动播放GIF
//    if(current_screen_id == 9)
//    {
//        AnimationStart(9,1);                                                         //动画开始播放
//    }
//       //进到进度条界面获取当前文本值
//    if(current_screen_id==5)                                   
//    {
//        GetControlValue(5,1);
//    }
//    //进到二维码页面生成二维码
//    if(current_screen_id==14)                                   
//    {
//        //二维码控件显示中文字符时，需要转换为UTF8编码，
//        //通过“指令助手”，转换“www.gz-dc.com” ，得到字符串编码如下
//        uint8 dat[] = {0x77,0x77,0x77,0x2E,0x67,0x7A,0x2D,0x64,0x63,0x2E,0x63,0x6F,0x6D};
//        SetTextValue(14,1,dat);                                                      //发送二维码字符编码                     
//    }

//    //数据记录显示
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
//        delay_ms(2000);                                                              //延时两秒
//        Record_ResetEvent(15,1,0,0);
//        Record_ResetEvent(15,1,1,0);
//        Record_ResetEvent(15,1,2,0);

//    }
//    //进入音乐画面自动播放
//    if(current_screen_id == 17)   
//    {
//         uint8 buffer[6] = {0x90,0x01 ,0x00 ,0x01 ,0x01};
//         SetButtonValue(17,3,1);
//         PlayMusic(buffer);                                                           //播放音乐
//    }
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
    if(current_screen_id==17)                                              
    {
        //当前电流、温度从0到1000循环显示，艺术字从0-999循环显示
        SetTextInt32(17,11,10,0,2);                                      //当前电流
			delay_ms(1000);
			SetTextInt32(17,11,20,0,2);
        SetTextInt32(4,7,test_value%1000,0,1);                                      //艺术字
        SetTextValue(17,7,"有");                                                 //设置文本值
			SetTextValue(17,8,"工作");  
  
//        test_value++;                                                                
//        if(test_value >= 1000)                                                       
//        {                                                                            
//            test_value = 0;                                                            
//        }                                                                            
//        if(test_value>0&&test_value<500)                                             //大于0小于500文本显示红色
//        {                                                                            
//            SetControlBackColor(4,6,0xF800);                                         //设置文本背景色
//        }                                                                            
//        else if(test_value>=500)                                                     //大于500文本显蓝色
//        {                                                                            
//            SetControlBackColor(4,6,0x001F);                                         //设置文本背景色
//        }                                                                            
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
	if(screen_id==2)                                                                 //画面ID2：文本设置和显示
			{                                                                            
					
				int32 value=0;                                                            
					sscanf(str,"%ld",&value);                                                    //把字符串转换为整数 
					if(control_id==1)                                                            //最高电压
					{                                                                         
							//限定数值范围（也可以在文本控件属性中设置）                             
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