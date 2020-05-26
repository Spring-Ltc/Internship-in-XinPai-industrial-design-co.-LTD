 
 
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

	   
		   
//----------------------------------����˵��--------------------------------------------
//�������ܣ�	ADC1 ��ʼ��
//��ڲ�����	��
//����ֵ��		��
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/02
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------									   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure; /////

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE); /////
	

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PB0, CH8                       
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	//PB1, CH9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	//Pc5, CH15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
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

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 3;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ��� 

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




  
//	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
//	
//	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
//	 
//	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
//	
//	ADC_StartCalibration(ADC1);	 //����ADУ׼
// 
//	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
	
	
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}




//----------------------------------����˵��--------------------------------------------
//�������ܣ�	��ȡADC
//��ڲ�����	ch��ADC1ͨ����8��9��15��
//����ֵ��		ADC_GetConversionValue(ADC1)���������һ��ADC1�������ת�����
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/12
//�޸����ݣ�������ɻ�������
//--------------------------------------------------------------------------------------
u16 Get_Adc(u8 ch)   
{
//  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
//	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  		
//		
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
//	 
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

//	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}




//----------------------------------����˵��--------------------------------------------
//�������ܣ�	��ȡһ��ʱ���ڵ�ADCƽ��ֵ
//��ڲ�����	ch��ADC1ͨ����8��9��15��
//            time��ʱ�䳤��  
//����ֵ��		temp_val/times
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/12
//�޸����ݣ�������ɻ�������
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



//----------------------------------����˵��--------------------------------------------
//�������ܣ�	��ȡADC�ɼ�����ѹֵ��������AP1,AP2,AP3����
//��ڲ�����	
//����ֵ��		
//--------------------------------------------------------------------------------------
//���ߣ�Zhou_Taoyuan	
//�޸����ڣ�2020/05/16
//�޸����ݣ�����ɼ���־
//--------------------------------------------------------------------------------------
void AirPressureTask(void)
{
	if(HMITtoAPTFlag == 1)
	{
		adcx1=ADC_ConvertedValue[0];          //��ȡADCģ��ֵ
		AP1=(3400 - (int)adcx1) / 34.25;         //��ADCģ��ֵת������ѹֵ
		adcx2=ADC_ConvertedValue[1];
		AP2=(3400 - (int)adcx2) / 34.25;
		adcx3=ADC_ConvertedValue[2];
		AP3=(3400 - (int)adcx3) / 34.25;
		APTtoHMITFlag = 1;
	}
}
























