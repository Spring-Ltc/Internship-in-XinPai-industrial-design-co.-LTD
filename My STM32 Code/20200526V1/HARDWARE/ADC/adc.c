 
 
 #include "adc.h"
 #include "delay.h"
 #include "hmi_handle.h"

#include "stm32f10x_dma.h"

#include "stm32f10x_adc.h"

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

__IO uint16_t ADC_ConvertedValue[3];


u16 adcx1 ,adcx2 ,adcx3;
int AP1, AP2, AP3;

int APTtoHMITFlag = 0;

	   
		   
//----------------------------------函数说明--------------------------------------------
//函数功能：	ADC1 初始化
//入口参数：	无
//返回值：		无
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/02
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------									   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure; /////

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE); /////
	

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PB0, CH8                       
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	//PB1, CH9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	//Pc5, CH15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	DMA_DeInit(DMA1_Channel1); 
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//ADC??
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertedValue; 
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 
	DMA_InitStructure.DMA_BufferSize =3;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord ; 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;   
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular  ; 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High ;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel1, ENABLE);

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 3;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器 

	/*??ADC??,?PCLK2?8??,?9MHz*/
    RCC_ADCCLKConfig(RCC_PCLK2_Div8); 

    /*??ADC1???0?55.    5?????,???1 */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);
    /*??ADC1???0?55.    5?????,???2 */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_55Cycles5);
    /*??ADC1???0?55.    5?????,???2 */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 3, ADC_SampleTime_55Cycles5);
    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /*??????? */   
    ADC_ResetCalibration(ADC1);
    /*??????????? */
    while(ADC_GetResetCalibrationStatus(ADC1));

    /* ADC?? */
    ADC_StartCalibration(ADC1);
    /* ??????*/
    while(ADC_GetCalibrationStatus(ADC1));

    /* ??????????,????????ADC?? */ 
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);




  
//	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
//	
//	ADC_ResetCalibration(ADC1);	//使能复位校准  
//	 
//	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
//	
//	ADC_StartCalibration(ADC1);	 //开启AD校准
// 
//	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
	
	
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}




//----------------------------------函数说明--------------------------------------------
//函数功能：	获取ADC
//入口参数：	ch：ADC1通道（8，9，15）
//返回值：		ADC_GetConversionValue(ADC1)：返回最近一次ADC1规则组的转换结果
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/12
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
u16 Get_Adc(u8 ch)   
{
//  	//设置指定ADC的规则组通道，一个序列，采样时间
//	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  		
//		
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
//	 
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

//	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}




//----------------------------------函数说明--------------------------------------------
//函数功能：	获取一段时间内的ADC平均值
//入口参数：	ch：ADC1通道（8，9，15）
//            time：时间长度  
//返回值：		temp_val/times
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/12
//修改内容：初次完成基本功能
//--------------------------------------------------------------------------------------
u16 Get_Adc_Average(u8 ch,u8 times)
{
//	u32 temp_val=0;
//	u8 t;
//	for(t=0;t<times;t++)
//	{
//		temp_val+=Get_Adc(ch);
//		delay_ms(5);
//	}
//	return temp_val/times;
} 	 



//----------------------------------函数说明--------------------------------------------
//函数功能：	获取ADC采集的气压值，并放入AP1,AP2,AP3数组
//入口参数：	
//返回值：		
//--------------------------------------------------------------------------------------
//作者：Zhou_Taoyuan	
//修改日期：2020/05/16
//修改内容：加入采集标志
//--------------------------------------------------------------------------------------
void AirPressureTask(void)
{
	if(HMITtoAPTFlag == 1)
	{
		adcx1=ADC_ConvertedValue[0];          //获取ADC模拟值
		AP1=(3400 - (int)adcx1) / 34.25;         //将ADC模拟值转换成气压值
		adcx2=ADC_ConvertedValue[1];
		AP2=(3400 - (int)adcx2) / 34.25;
		adcx3=ADC_ConvertedValue[2];
		AP3=(3400 - (int)adcx3) / 34.25;
		APTtoHMITFlag = 1;
	}
}
























