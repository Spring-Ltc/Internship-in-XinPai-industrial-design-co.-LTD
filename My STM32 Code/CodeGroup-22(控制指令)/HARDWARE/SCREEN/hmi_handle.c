

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



#define TIME_100MS 10                                                                //100毫秒(10个单位)

volatile uint32  timer_tick_count = 0;                                               //定时器节拍

uint8  cmd_buffer[CMD_MAX_SIZE];                                                     //指令缓存
char  PW[10];    
static uint16 current_screen_id = 0;                                                 //当前画面ID
//static int32 progress_value = 0;                                                     //进度条测试值
//static int32 test_value = 0;                                                         //测试值
//static uint8 update_en = 0;                                                          //更新标记
//static int32 meter_flag = 0;                                                         //仪表指针往返标志位
//static int32 num = 0;                                                                //曲线采样点计数
//static int sec = 1;                                                                  //时间秒
//static int32 curves_type = 0;                                                        //曲线标志位  0为正弦波，1为锯齿波                  
//static int32 second_flag=0;                                                          //时间标志位
//static int32 icon_flag = 0;                                                          //图标标志位
//static uint8 Select_H ;                                                              //滑动选择小时
//static uint8 Select_M ;                                                              //滑动选择分钟 
//static uint8 Last_H ;                                                                //上一个选择小时
//static uint8 Last_M;                                                                 //上一个选择分钟 
//static int32 Progress_Value = 0;                                                     //进度条的值 

void UpdateUI(void);                                                                 //更新UI数据
void SensorStatusJudgment(void);                                                     //显示从控板传感器状态

void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str);


//	 static int32 test_value = 0;                                                     //测试值
   uint32 timer_tick_last_update = 0;                                               //上一次更新的时间
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
	size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);                    //（串口屏）一直检测缓冲区指令，若缓冲区收到指令，立即进行提取  
	      
  if(size>0&&cmd_buffer[1]!=0x07)                                    //（串口屏）接收到指令 ，及判断不是开机提示
  {                                                                           
    ProcessMessage((PCTRL_MSG)cmd_buffer, size);                   //（串口屏）解析指令（然后调用对应指令类型的函数并将指令的数据作为参数传进调用的函数）
	}
  else if(size>0&&cmd_buffer[1]==0x07)                               //（串口屏）如果为指令0x07就软重置STM32  并 初始化某些界面
  {                                                                           
		SetTextValue(6, 6, (uchar *)EquipmentNo);  //初始化显示设备序号
		SensorStatusJudgmentInit();  //初始化显示设备状态信息
//		AnimationStart(22, 23);  //（测试图标）
		
		__disable_fault_irq();                                                   
    NVIC_SystemReset();  
		
				
  } 
	
	//初始化某些界面（只开机进行一次）
//	if(OnceFlag == 0)
//		{
//			SetTextValue(6, 6, (uchar *)EquipmentNo);  //初始化显示设备序号
//			SensorStatusJudgmentInit();  //初始化显示设备状态信息
//			AnimationStart(22, 23);  //（测试图标）
//			OnceFlag = 1;
//		}

	
	
	//定时刷新屏幕
	if(TIM4Flag10Ms3 == 1) 
  {   
		TIM4Flag10Ms3 = 0;
		if((TIM4Flag20Ms3++) >= 2)
		{
			TIM4Flag20Ms3 = 0;
			UpdateUI(); 
		}
  }	
	
	
	//测试屏幕ID是否正常
	if(current_screen_id == 25) 
			GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	else if(current_screen_id != 25)
			GPIO_SetBits(GPIOC,GPIO_Pin_2);
	
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
    current_screen_id = screen_id;           //在工程配置中开启画面切换通知，记录当前画面ID
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
	
	
	//管理员 登陆 修改界面
	if(current_screen_id == 2)
	{
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)  //登录信息发送成功
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetScreen(3);   //管理员修改密码成功界面
		}
	}
	
	//管理员修改密码界面
	if(current_screen_id == 8)
	{
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)  //修改密码发送成功
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetTextValue(6, 8, (uchar *)UserChangePasswordBuf);  //UserChangePasswordBuf 为用户修改后的密码存放数组
			SetScreen(9);   //管理员修改密码成功界面
		}
	}
	
	//管理员增加操作员界面
	if(current_screen_id == 11)
	{
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)  //登录信息发送成功
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetScreen(12);   //管理员增加操作员成功界面
		}
	}
	
	//管理员删除操作员界面
	if(current_screen_id == 13)
	{
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)  //登录信息发送成功
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetScreen(14);   //管理员删除操作员成功界面
		}
	}
	
	//操作员登陆修改界面
	if(current_screen_id == 15)
	{
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)  //登录信息发送成功
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetScreen(16);   //操作员进入修改密码界面
		}
	}
	
	//操作员修改密码界面
	if(current_screen_id == 16)
	{
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)  //修改密码发送成功
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetScreen(17);   //操作员修改密码成功界面
		}
	}
	
	//维护人员选择界面
	if(current_screen_id == 19)  
	{
		HMITtoBCTFlag = 0;
		HMITtoAPTFlag = 0;
		HMITtoMSTGetGprsStateFlag = 0;
		HMITtoBSTFlag = 0;
		HMITtoLKTFlag = 0;
	}	
	
	//离线工作界面
	if(current_screen_id == 20)  
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
	
	//设备状态界面1
	if(current_screen_id == 21)   
	{
		HMITtoBCTFlag = 0;
		HMITtoAPTFlag = 0;
		
		if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_4) == 1)  //扫码器触发引脚
			SetTextValue(21, 5, (uchar *)"关"); 
		else if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_4) == 0)  //扫码器触发引脚
		{
			SetTextValue(21, 5, (uchar *)"开"); 
			
			HMITtoBCTFlag = 1;
			if(BCTtoHMITFlag == 1)
			{
				SetTextValue(21, 6, (uchar *)BarCodeBuf);     //将条形码值显示在
				BCTtoHMITFlag = 0;
			}
		}
		
		if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_8) == 1)  //电磁锁触发引脚
			SetTextValue(21, 7, (uchar *)"开"); 
		else if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_8) == 0)  //电磁锁触发引脚
			SetTextValue(21, 7, (uchar *)"关"); 
		
		
//		HMITtoMSTGetGprsStateFlag = 1;
//		if(MSTtoHMITGetGprsStateSuccessFlag == 1)
//		{
//			SetTextInt32(21,3,Progress_Value,0,6);
//			MSTtoHMITGetGprsStateSuccessFlag = 0;
//		}
		
	}
	
	//设备状态界面2                    
  if(current_screen_id == 22)                            
  {
    HMITtoBCTFlag = 0;
		HMITtoMSTGetGprsStateFlag = 0;
		
		
		HMITtoAPTFlag = 1;
		if(APTtoHMITFlag == 1)
		{
			SetTextInt32(22, 7, -(int)AP1, 1, 2);    
			SetTextInt32(22, 14, -(int)AP2, 1, 2);  
			SetTextInt32(22, 21, -(int)AP3, 1, 2);     //三个气压传感器的值
			APTtoHMITFlag = 0;
		} 
		
		
		HMITtoBSTGetSensorStateFlag = 1;
		if(BSTtoHMITGetSensorStateSuccessFlag == 1)
		{
			SensorStatusJudgment();
		}
  }	
	
	//维护人员开锁界面
	if(current_screen_id == 23)  
	{
		HMITtoLKTFlag = 1;
	}
	
	//操作员登陆工作界面
	if(current_screen_id == 24)  
	{
		HMITtoBCTFlag = 0;  //扫码任务置零
		HMITtoAPTFlag = 0;  //气压任务置零
		
		
		
		if(USTtoHMITTranspondAccountDataSuccessFlag == 1)
		{
			USTtoHMITTranspondAccountDataSuccessFlag = 0;
			SetScreen(37);   //操作员登陆工作成功界面
		}
	}	
	
	//工作界面
	if(current_screen_id == 25)  
	{
		HMITtoBCTFlag = 1;
		if(BCTtoHMITFlag == 1)
		{
			SetTextValue(25, 1, (uchar *)BarCodeBuf);     //将条形码值显示在工作界面的1号孔位上
			BCTtoHMITFlag = 0;
			
			//通知局域网任务转发当前滤杯序列号
			HMITtoUSTTranspondBarCodeFlag = 1;
			if(USTtoHMITTranspondBarCodeSuccessFlag == 1)
			{
				USTtoHMITTranspondBarCodeSuccessFlag = 0;
			}
		}
		if(USTtoHMITComparisonBarCodeSuccessFlag == 1)  //收到用户服务器返回的序列号比对数据包
		{
			if(USTtoLANBuf[7] == 0x00)
				AnimationPlayFrame(25, 3, 2);
			else if(USTtoLANBuf[7] == 0x01)
				AnimationPlayFrame(25, 3, 1);
			USTtoHMITComparisonBarCodeSuccessFlag = 0;
		}
		
		//实时采集气压值
		HMITtoAPTFlag = 1;
		if(APTtoHMITFlag == 1)
		{
			SetProgressValue(25, 2, -(int)AP1);
			SetProgressValue(25, 9, -(int)AP2);
			SetProgressValue(25, 16, -(int)AP3);    //气压值进度条
			APTtoHMITFlag = 0;
		}
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
	
	//同步装箱序列号按键
	if(screen_id==4&&control_id==1)
	{
		FunctionCode = 0x50;
		HMITtoMSTRequestSynSeriesNumFlag = 1;
	}
	
	//维护界面扫码器开关
	if(screen_id==21&&control_id==8&&state==0&&HMITtoBCTFlag == 1)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
	}
	else if(screen_id==21&&control_id==8&&state==1&&HMITtoBCTFlag == 1)
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	}
	
	//维护人员电磁锁开关
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
	
	//管理员按下 登陆 修改按键
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
	
	//管理员设置IP和端口后按下确认键，将IP和端口号合并为NewIPAndPort，并显示在screen_id 6的文本框内
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
	
	//管理员按下增加操作员“增加”按键
	if(screen_id==11&&control_id==2)
	{
		FunctionCode = 0x52;
		HMITtoUSTTranspondAccountDataFlag = 1;
	}
	
	//管理员按下删除操作员“删除”按键
	if(screen_id==13&&control_id==2)
	{
		FunctionCode = 0x53;
		HMITtoUSTTranspondAccountDataFlag = 1;
	}
	
	//操作员 登陆 修改按下确认键，将ID和密码合并为UserIDAndPasswordBuf
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
	
	//操作员 登陆 工作按下确认键，将ID和密码合并为UserIDAndPasswordBuf
	if(screen_id==24&&control_id==2)
	{
		int i, j;
		memset(UserIDAndPasswordBuf, 0, sizeof(UserIDAndPasswordBuf));
		SetTextValue(25, 23, (uchar *)UserIDBuf);  //显示操作员ID
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
	
	//维护人员登陆按下确认键
	if(screen_id==18&&control_id == 3)
	{
		long Password;
		sscanf(GetMaintainerPasswordBuf, "%ld", &Password);             //把字符串转换为整数
		if(Password == SetMaintainerPassword)
			SetScreen(19);  //密码正确
		else
			SetScreen(34);  //密码错误
		memset(GetMaintainerPasswordBuf, 0, sizeof(GetMaintainerPasswordBuf));
	}
	
	//管理员按下修改密码确认键
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
	
	//操作员按下修改密码确认键
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
				UserChangePasswordBuf[i] = UserChangePasswordBuf1[i];   //UserChangePasswordBuf 为用户修改后的密码存放数组
			}
			memset(UserIDAndPasswordBuf, 0, sizeof(UserIDAndPasswordBuf));
			for(m=0; m<sizeof(UserIDBuf); m++)
			{
				UserIDAndPasswordBuf[m] = UserIDBuf[m];
			}
			for(n=0; n<sizeof(UserPasswordBuf); n++)
			{
				UserIDAndPasswordBuf[m+n] = UserChangePasswordBuf[n];  //将操作员修改后的密码同ID号一起打包为 UserIDAndPasswordBuf
			}
			FunctionCode = 0x57;
			HMITtoUSTTranspondAccountDataFlag = 1;
		}
		else
			SetScreen(33);
	}
	
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
	if(screen_id==2&&control_id==1)    
	{                                                
		
		int i=0;
		memset(UserPasswordBuf, 0, sizeof(UserPasswordBuf));
		for(i=0;i<4;i++)
		{
			UserPasswordBuf[i] = str[i];
		}
	}               
	
	//管理员输入新的用户服务器IP地址
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
	//管理员输入新的用户服务器端口号
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
	
	//输入账户ID
	if((screen_id==24&&control_id==3)||
		 (screen_id==15&&control_id==1)||
		 (screen_id==11&&control_id==1)||
		 (screen_id==13&&control_id==1))    //（24，3）操作员登陆工作界面；（15，1）操作员登陆修改；；（11，1）增加操作员；（13，1）删除操作员
	{
		int i=0;
		memset(UserIDBuf, 0, sizeof(UserIDBuf));
		for(i=0; i<4; i++)
		{
			UserIDBuf[i] = str[i];
		}
	}
	//管理员、操作员输入密码
	if((screen_id==24&&control_id==4)||
		 (screen_id==15&&control_id==2)||
		 (screen_id==2&&control_id==1))    //（24，4）操作员登陆工作界面；（15，2）操作员登陆修改；（2，1）管理员登陆修改
	{
		int i=0;
		memset(UserPasswordBuf, 0, sizeof(UserPasswordBuf));
		for(i=0; i<4; i++)
		{
			UserPasswordBuf[i] = str[i];
		}
	}
	
	//维护人员输入密码
	if(screen_id==18&&control_id==2)    //（18，2）维护人员登陆工作界面
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
	
	//管理员、操作员修改密码
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
*  \brief  进度条控件通知                                                       
*  \details  调用GetControlValue时，执行此函数                                  
*  \param screen_id 画面ID                                                      
*  \param control_id 控件ID                                                     
*  \param value 值                                                              
*/                                                                              
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)           
{
	
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


void SensorStatusJudgmentInit(void)
{
		//1号孔位
		SetTextValue(22, 3, (uchar *)"无杯");
		SetTextValue(22, 4, (uchar *)"泵关");
		SetTextValue(22, 5, (uchar *)"无水");
		SetTextValue(22, 8, (uchar *)"阀关");
		AnimationPlayFrame(22, 9, 0);
	
	//2号孔位
		SetTextValue(22, 10, (uchar *)"无杯");
		SetTextValue(22, 11, (uchar *)"泵关");
		SetTextValue(22, 12, (uchar *)"无水");
		SetTextValue(22, 15, (uchar *)"阀关");
		AnimationPlayFrame(22, 16, 0);
	
	//3号孔位
		SetTextValue(22, 17, (uchar *)"无杯");
		SetTextValue(22, 18, (uchar *)"泵关");
		SetTextValue(22, 19, (uchar *)"无水");
		SetTextValue(22, 22, (uchar *)"阀关");
		AnimationPlayFrame(22, 23, 0);
}


void SensorStatusJudgment(void)
{

static u8 lastCup1State,lastCup3State,lastCup5State,
					lastPUMP1State,lastPUMP3State,lastPUMP5State,
					lastWater1SensorState,lastWater3SensorState,lastWater5SensorState,
					lastVALVE1State,lastVALVE3State,lastVALVE5State;
	//1号孔位
	if(Cup1State != lastCup1State)
	{
		lastCup1State=Cup1State;
		if(Cup1State==0x00)
			SetTextValue(22, 3, (uchar *)"无杯");
		else if(Cup1State==0x01)		
			SetTextValue(22, 3, (uchar *)"有杯");
	}
	if(PUMP1State != lastPUMP1State)
	{
		lastPUMP1State=PUMP1State;
		if(PUMP1State==0x00)
			SetTextValue(22, 4, (uchar *)"泵关");
		else if(PUMP1State==0x01)		
			SetTextValue(22, 4, (uchar *)"泵开");
	}
	if(Water1SensorState != lastWater1SensorState)
	{
		lastWater1SensorState=Water1SensorState;
		if(Water1SensorState==0x00)
			SetTextValue(22, 5, (uchar *)"无水");
		else if(Water1SensorState==0x01)		
			SetTextValue(22, 5, (uchar *)"有水");
	}

	if(VALVE1State != lastVALVE1State)
	{
		lastVALVE1State=VALVE1State;
		if(VALVE1State==0x00)
			SetTextValue(22, 8, (uchar *)"阀关");
		else if(VALVE1State==0x01)		
			SetTextValue(22, 8, (uchar *)"阀开");
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

	
		//2号孔位
	if(Cup3State != lastCup3State)
	{
		lastCup3State=Cup3State;
		if(Cup3State==0x00)
			SetTextValue(22, 10, (uchar *)"无杯");
		else if(Cup3State==0x01)		
			SetTextValue(22, 10, (uchar *)"有杯");
	}
	if(PUMP3State != lastPUMP3State)
	{
		lastPUMP3State=PUMP3State;
		if(PUMP3State==0x00)
			SetTextValue(22, 11, (uchar *)"泵关");
		else if(PUMP3State==0x01)		
			SetTextValue(22, 11, (uchar *)"泵开");
	}
	if(Water3SensorState != lastWater3SensorState)
	{
		lastWater3SensorState=Water3SensorState;
		if(Water3SensorState==0x00)
			SetTextValue(22, 12, (uchar *)"无水");
		else if(Water3SensorState==0x01)		
			SetTextValue(22, 12, (uchar *)"有水");
	}

	if(VALVE3State != lastVALVE3State)
	{
		lastVALVE3State=VALVE3State;
		if(VALVE3State==0x00)
			SetTextValue(22, 15, (uchar *)"阀关");
		else if(VALVE3State==0x01)		
			SetTextValue(22, 15, (uchar *)"阀开");
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
	
	//3号孔位
	if(Cup5State != lastCup5State)
	{
		lastCup5State=Cup5State;
		if(Cup5State==0x00)
			SetTextValue(22, 17, (uchar *)"无杯");
		else if(Cup5State==0x01)		
			SetTextValue(22, 17, (uchar *)"有杯");
	}
	if(PUMP5State != lastPUMP5State)
	{
		lastPUMP5State=PUMP5State;
		if(PUMP5State==0x00)
			SetTextValue(22, 18, (uchar *)"泵关");
		else if(PUMP5State==0x01)		
			SetTextValue(22, 18, (uchar *)"泵开");
	}
	if(Water5SensorState != lastWater5SensorState)
	{
		lastWater5SensorState=Water5SensorState;
		if(Water5SensorState==0x00)
			SetTextValue(22, 19, (uchar *)"无水");
		else if(Water5SensorState==0x01)		
			SetTextValue(22, 19, (uchar *)"有水");
	}

	if(VALVE5State != lastVALVE5State)
	{
		lastVALVE5State = VALVE5State;
		if(VALVE5State == 0x00)
			SetTextValue(22, 22, (uchar *)"阀关");
		else if(VALVE5State==0x01)		
			SetTextValue(22, 22, (uchar *)"阀开");
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

	










