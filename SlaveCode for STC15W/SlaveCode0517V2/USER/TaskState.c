




#include "TaskState.h"
#include "GPIO.h"



void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;	
	
	/*控制继电器工作引脚初始化*/
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7;//VALVE5 4 3 2 1   R GLED1
	GPIO_InitStructure.Mode = GPIO_OUT_PP	;	// 推挽输出		 
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);
	
	
	GPIO_InitStructure.Pin  = GPIO_Pin_5;// CUP4
	GPIO_InitStructure.Mode = GPIO_PullUp	;	//上拉准双向口 	 
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);	
	

	
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_5; //CUP1/CUP3/CUP5
	GPIO_InitStructure.Mode =GPIO_PullUp ;//上拉准双向口	 	 
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7; // RLED3 /R5/G5/ R GLED2
	GPIO_InitStructure.Mode = GPIO_OUT_PP	;	//推挽输出 
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	
	
	
 	GPIO_InitStructure.Pin  = GPIO_Pin_7;// //PUMP
 	GPIO_InitStructure.Mode = GPIO_OUT_PP	;	//推挽输出 		 
 	GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);	
	 
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_1;//   WATERSTATUSE 2/1
 	GPIO_InitStructure.Mode = GPIO_PullUp	;//上拉准双向口		 
 	GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);	
	
  GPIO_InitStructure.Pin  = GPIO_Pin_6|GPIO_Pin_7; // UART1
	GPIO_InitStructure.Mode = GPIO_PullUp	;	//上拉准双向口  
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