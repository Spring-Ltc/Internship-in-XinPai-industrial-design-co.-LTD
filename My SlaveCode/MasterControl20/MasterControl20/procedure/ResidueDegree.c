#include	"ResidueDegree.h"


/**********************************************************************************/
//���ƣ�u16 DataProcessing��u16  SurplusCount ��
//���ܣ�EEPROM����ʣ�ഥ����һ
//������u16  SurplusCount�����߱��Ӵ�����־���Ӽ�������Һһ����ɣ�
//����ֵ����ֵд��EEPROMEEPROM_write_n(0x0000)��EEPROM_write_n(0x0001����
//--------------------------------
//���ߣ�XiongXiaoping 	
//�޸����ڣ�2020/05/09
//�޸����ݣ�����ʹ�ú󴥷�����һ�Σ����ӣ�����1����뻺�����飬���浽EEPROM
//----------------------------
/**********************************************************************************/
u16 DataProcessing(u16  SurplusCount )
{
	  u8 SurplusCount1=0,SurplusCount2=0;	//�����������ݻ�����
	  u16 ReadValue=0;	//��eepromֵ������
	  ReadValue=EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000);	//��ȡEEPROM�洢��ֵ
	  ReadValue-=SurplusCount;	//EEPROM�洢��ֵ��ȥ��Һ�����������߱��Ӵ�����������
	  SurplusCount1 = ReadValue%256;	//��λ��λ�洢
	  SurplusCount2 = ReadValue/256;	//�Ͱ�λ
	  EEPROM_SectorErase(0x0000);		//��������
		EEPROM_write_n(0x0000,SurplusCount1);	//�Ͱ�λд��
		_nop_();
		EEPROM_write_n(0x0001,SurplusCount2);	//�߰�λд��
    _nop_();
	  return 0;	//д��ɹ�
}
/**********************************************************************************/


/**********************************************************************************/
//���ƣ�void MasterControlEE()
//���ܣ��޸�ʣ�౭�Ӵ�������
//������֡ͷ+����+ʶ����+������(0x55������)+���ݶ�+Ч����+֡β  
//      EEPROM_SectorErase:ɾ����������
//      EEPROM_write_n:д��һ���ֽں���
//      _nop_():��ʱ
//      SlaveControlEE()�����ͺ������ݵ���
//����ֵ��void
//--------------------------------
//���ߣ�XiongXiaoping 	
//�޸����ڣ�2020/05/07
//�޸����ݣ�
//----------------------------
/**********************************************************************************/
void MasterControlEE()
{
	u16 DoubleTempvalue;
			if((MasterDataBuff[21]*256+MasterDataBuff[22])<0xfffa)	
				{
					DoubleTempvalue = (MasterDataBuff[21]*256+(MasterDataBuff[22]+0x03));
					EEPROM_SectorErase(0x0000);	
					EEPROM_write_n(0x0000,(DoubleTempvalue%256));//�Ͱ�λд��һ��������Ԫ
					_nop_();
					EEPROM_write_n(0x0001,(DoubleTempvalue/256));//�߰�λд��һ��������Ԫ
					_nop_();
			   
				}
				else if((MasterDataBuff[21]*256+MasterDataBuff[22])>=0xfffa)	
				{
					EEPROM_SectorErase(0x0000);	
					EEPROM_write_n(0x0000,MasterDataBuff[22]);//�Ͱ�λд��һ��������Ԫ
					_nop_();
					EEPROM_write_n(0x0001,MasterDataBuff[21]);//�߰�λд��һ��������Ԫ
					_nop_();
				}
				
//				{
//					 SlaveControlEE();//����Ҫ���͵�����
//			      for(y=0;y<=11;y++)
//						{
//							UART_Send_Byte(TX1_Buffer[y]);//��ӡʣ�౭�Ӵ���
//						}
//				}

}
/**********************************************************************************/
/**********************************************************************************/
//���ƣ�Confine()
//���ܣ����������ߣ����Ӽ�һ��ʣ�౭��
//������u16  SurplusCount�����߱��Ӵ�����־���Ӽ�������Һһ����ɣ�
//����ֵ����ֵд��EEPROMEEPROM_write_n(0x0000)��EEPROM_write_n(0x0001����
//--------------------------------
//���ߣ�XiongXiaoping 	
//�޸����ڣ�2020/05/09
//�޸����ݣ�����ʹ�ú󴥷�����һ�Σ����ӣ�����1����뻺�����飬���浽EEPROM
//----------------------------
/**********************************************************************************/
void Confine()
{
if((EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000))<=0x03)   //��ȡʣ�౭�Ӵ���
				{
					DataProcessing(SurplusCount);//�������ı���ʹ�ô�����������д��EEPROM
					SurplusCount=0;//д����ɺ����㷽���´�д��
					if((EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000))<=0x00) //��ʣ��3��ʱ�Ƿ��ж�����Ѿ�ʹ������
						{
						
							//RX1_Buffer[26]=0x00; //�������
							Cup1Eff=0;  //��־��Ϊ0
						//	RX1_Buffer[27]=0x00;  //�������
							Cup3Eff=0;  //��־��Ϊ0
						//	RX1_Buffer[28]=0x00; //�������
							Cup5Eff=0;  //��־��Ϊ0
							
						}						
				}					
	  else	if((EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000))<=0x02)   //��ȡʣ�౭�Ӵ���
			{
				//RX1_Buffer[28]=0x00;
				Cup5Eff=0;
				DataProcessing(SurplusCount);//�������ı���ʹ�ô�����������д��EEPROM
			  SurplusCount=0;//д����ɺ����㷽���´�д��
				if((EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000))<=0x00) //��ʣ��2��ʱ�Ƿ��ж�����Ѿ�ʹ������
					{
					
						//RX1_Buffer[26]=0x00; //�������
						Cup1Eff=0;  //��־��Ϊ0
						//RX1_Buffer[27]=0x00;  //�������
						Cup3Eff=0;  //��־��Ϊ0
					//	RX1_Buffer[28]=0x00; //�������
						Cup5Eff=0;  //��־��Ϊ0
						
					}						
			}
			else if	((EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000))<=0x01)
			{
			//	RX1_Buffer[27]=0x00;
				Cup3Eff=0;
				//RX1_Buffer[28]=0x00;
				Cup5Eff=0;
				DataProcessing(SurplusCount);//�������ı���ʹ�ô�����������д��EEPROM
			  SurplusCount=0;//д����ɺ����㷽���´�д��	
				if((EEPROM_read_n(0x0001)*256+EEPROM_read_n(0x0000))<=0x00)//��ʣ��1��ʱ�Ƿ��ж�����Ѿ�ʹ������
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
				DataProcessing(SurplusCount);//�������ı���ʹ�ô�����������д��EEPROM
			  SurplusCount=0;//д����ɺ����㷽���´�д��	
			}		
	}			
	/********************************************************************/		