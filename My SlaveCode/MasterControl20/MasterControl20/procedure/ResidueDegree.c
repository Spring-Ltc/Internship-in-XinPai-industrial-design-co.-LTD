#include	"ResidueDegree.h"


/**********************************************************************************/
//名称：u16 DataProcessing（u16  SurplusCount ）
//功能：EEPROM杯子剩余触发减一
//参数：u16  SurplusCount：拿走杯子触发标志杯子计数（抽液一次完成）
//返回值：将值写入EEPROMEEPROM_write_n(0x0000)和EEPROM_write_n(0x0001）；
//--------------------------------
//作者：XiongXiaoping 	
//修改日期：2020/05/09
//修改内容：数据使用后触发用了一次（杯子），减1后存入缓冲数组，并存到EEPROM
//----------------------------
/**********************************************************************************/
u16 DataProcessing(u16  SurplusCount )
{
	  u8 SurplusCount1=0,SurplusCount2=0;	//定义两个数据缓存区
	  u16 ReadValue=0;	//读eeprom值缓存区
	  ReadValue=EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000);	//读取EEPROM存储的值
	  ReadValue-=SurplusCount;	//EEPROM存储的值减去抽液次数（即拿走杯子触发计数器）
	  SurplusCount1 = ReadValue%256;	//高位八位存储
	  SurplusCount2 = ReadValue/256;	//低八位
	  EEPROM_SectorErase(0x0000);		//擦除扇区
		EEPROM_write_n(0x0000,SurplusCount1);	//低八位写入
		_nop_();
		EEPROM_write_n(0x0001,SurplusCount2);	//高八位写入
    _nop_();
	  return 0;	//写入成功
}
/**********************************************************************************/


/**********************************************************************************/
//名称：void MasterControlEE()
//功能：修改剩余杯子次数函数
//参数：帧头+包长+识别码+功能码(0x55数据流)+数据段+效验码+帧尾  
//      EEPROM_SectorErase:删除扇区函数
//      EEPROM_write_n:写入一个字节函数
//      _nop_():延时
//      SlaveControlEE()：发送函数数据调用
//返回值：void
//--------------------------------
//作者：XiongXiaoping 	
//修改日期：2020/05/07
//修改内容：
//----------------------------
/**********************************************************************************/
void MasterControlEE()
{
	u16 DoubleTempvalue;
			if((MasterDataBuff[21]*256+MasterDataBuff[22])<0xfffa)	
				{
					DoubleTempvalue = (MasterDataBuff[21]*256+(MasterDataBuff[22]+0x03));
					EEPROM_SectorErase(0x0000);	
					EEPROM_write_n(0x0000,(DoubleTempvalue%256));//低八位写入一个扇区单元
					_nop_();
					EEPROM_write_n(0x0001,(DoubleTempvalue/256));//高八位写入一个扇区单元
					_nop_();
			   
				}
				else if((MasterDataBuff[21]*256+MasterDataBuff[22])>=0xfffa)	
				{
					EEPROM_SectorErase(0x0000);	
					EEPROM_write_n(0x0000,MasterDataBuff[22]);//低八位写入一个扇区单元
					_nop_();
					EEPROM_write_n(0x0001,MasterDataBuff[21]);//高八位写入一个扇区单元
					_nop_();
				}
				
//				{
//					 SlaveControlEE();//调用要发送的数组
//			      for(y=0;y<=11;y++)
//						{
//							UART_Send_Byte(TX1_Buffer[y]);//打印剩余杯子次数
//						}
//				}

}
/**********************************************************************************/
/**********************************************************************************/
//名称：Confine()
//功能：触发（拿走）杯子减一次剩余杯子
//参数：u16  SurplusCount：拿走杯子触发标志杯子计数（抽液一次完成）
//返回值：将值写入EEPROMEEPROM_write_n(0x0000)和EEPROM_write_n(0x0001）；
//--------------------------------
//作者：XiongXiaoping 	
//修改日期：2020/05/09
//修改内容：数据使用后触发用了一次（杯子），减1后存入缓冲数组，并存到EEPROM
//----------------------------
/**********************************************************************************/
void Confine()
{
if((EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000))<=0x03)   //读取剩余杯子次数
				{
					DataProcessing(SurplusCount);//将计数的杯子使用次数传给函数写入EEPROM
					SurplusCount=0;//写入完成后，清零方便下次写入
					if((EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000))<=0x00) //在剩余3次时是否判断这次已经使用完了
						{
						
							//RX1_Buffer[26]=0x00; //清空数组
							Cup1Eff=0;  //标志置为0
						//	RX1_Buffer[27]=0x00;  //清空数组
							Cup3Eff=0;  //标志置为0
						//	RX1_Buffer[28]=0x00; //清空数组
							Cup5Eff=0;  //标志置为0
							
						}						
				}					
	  else	if((EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000))<=0x02)   //读取剩余杯子次数
			{
				//RX1_Buffer[28]=0x00;
				Cup5Eff=0;
				DataProcessing(SurplusCount);//将计数的杯子使用次数传给函数写入EEPROM
			  SurplusCount=0;//写入完成后，清零方便下次写入
				if((EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000))<=0x00) //在剩余2次时是否判断这次已经使用完了
					{
					
						//RX1_Buffer[26]=0x00; //清空数组
						Cup1Eff=0;  //标志置为0
						//RX1_Buffer[27]=0x00;  //清空数组
						Cup3Eff=0;  //标志置为0
					//	RX1_Buffer[28]=0x00; //清空数组
						Cup5Eff=0;  //标志置为0
						
					}						
			}
			else if	((EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000))<=0x01)
			{
			//	RX1_Buffer[27]=0x00;
				Cup3Eff=0;
				//RX1_Buffer[28]=0x00;
				Cup5Eff=0;
				DataProcessing(SurplusCount);//将计数的杯子使用次数传给函数写入EEPROM
			  SurplusCount=0;//写入完成后，清零方便下次写入	
				if((EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000))<=0x00)//在剩余1次时是否判断这次已经使用完了
		   	{
			
				//RX1_Buffer[26]=0x00;
				Cup1Eff=0;
				//RX1_Buffer[27]=0x00;
				Cup3Eff=0;
				//RX1_Buffer[28]=0x00;
				Cup5Eff=0;
				
			  }		
			}
			else if((EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000))<=0x00)
			{
			
				//RX1_Buffer[26]=0x00;
				Cup1Eff=0;
				//RX1_Buffer[27]=0x00;
				Cup3Eff=0;
				//RX1_Buffer[28]=0x00;
				Cup5Eff=0;
				
			}		
			else
			{
				DataProcessing(SurplusCount);//将计数的杯子使用次数传给函数写入EEPROM
			  SurplusCount=0;//写入完成后，清零方便下次写入	
			}		
	}			
	/********************************************************************/		