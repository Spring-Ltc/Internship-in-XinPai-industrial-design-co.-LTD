
//	��������STCϵ�е�����EEPROM��д����

#include "config.h"
#include "eeprom.h"


//========================================================================
// ����: void	ISP_Disable(void)
// ����: ��ֹ����ISP/IAP.
// ����: non.
// ����: non.
// �汾: V1.0, 2012-10-22
//========================================================================
void	DisableEEPROM(void)
{
	ISP_CONTR = 0;			//��ֹISP/IAP����
	ISP_CMD   = 0;			//ȥ��ISP/IAP����
	ISP_TRIG  = 0;			//��ֹISP/IAP�����󴥷�
	ISP_ADDRH = 0xff;		//��0��ַ���ֽ�
	ISP_ADDRL = 0xff;		//��0��ַ���ֽڣ�ָ���EEPROM������ֹ�����
}

//========================================================================
// ����: void EEPROM_read_n(u16 EE_address,u8 *DataAddress,u16 number)
// ����: ��ָ��EEPROM�׵�ַ����n���ֽڷ�ָ���Ļ���.
// ����: EE_address:  ����EEPROM���׵�ַ.
//       DataAddress: �������ݷŻ�����׵�ַ.
//       number:      �������ֽڳ���.
// ����: non.
// �汾: V1.0, 2012-10-22
//========================================================================

u8 EEPROM_read_n(u16 DataAddress)//������
{
	u8 dat;
	EA = 1;		//��ֹ�ж�
	ISP_CONTR = (ISP_EN + ISP_WAIT_FREQUENCY);	//���õȴ�ʱ�䣬����ISP/IAP��������һ�ξ͹�
	ISP_READ();									//���ֽڶ���������ı�ʱ����������������
//	do
//	{
	//  IAP_CMD = CMD_READ;             //����IAP����
		ISP_ADDRH = DataAddress / 256;		//�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
		ISP_ADDRL = DataAddress % 256;		//�͵�ַ���ֽ�
	
	  ISP_CMD=1;//д�ֽ�ģʽ
		ISP_TRIG = 0x5a;                //д��������(0x5a)
    ISP_TRIG = 0xa5;                //д��������(0xa5)
	//	ISP_TRIG();							//����5AH������A5H��ISP/IAP�����Ĵ�����ÿ�ζ���Ҫ���
											//����A5H��ISP/IAP������������������
											//CPU�ȴ�IAP��ɺ󣬲Ż����ִ�г���
		_nop_();
		dat  = ISP_DATA;			//���������������ȴ�ISP/IAP/EEPROM�������
//		EE_address++;
//	DataAddress++;
//	}while(--number);
    
	DisableEEPROM();
	
	
 // EA = 1;		//���������ж�
	return dat;  
}


/******************** ������������ *****************/
//========================================================================
// ����: void EEPROM_SectorErase(u16 EE_address)
// ����: ��ָ����ַ��EEPROM��������.
// ����: EE_address:  Ҫ����������EEPROM�ĵ�ַ.
// ����: non.
// �汾: V1.0, 2013-5-10
//========================================================================
void EEPROM_SectorErase(u16 EE_address)//����һ������
{
	EA = 0;		//��ֹ�ж�
											//ֻ������������û���ֽڲ�����512�ֽ�/������
											//����������һ���ֽڵ�ַ����������ַ��
	ISP_ADDRH = EE_address / 256;			//��������ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
	ISP_ADDRL = EE_address % 256;			//��������ַ���ֽ�
	ISP_CONTR = (ISP_EN + ISP_WAIT_FREQUENCY);	//���õȴ�ʱ�䣬����ISP/IAP��������һ�ξ͹�
	ISP_ERASE();							//������������������ı�ʱ����������������
	ISP_TRIG();
	_nop_();
	DisableEEPROM();
	EA = 1;		//���������ж�
}

//========================================================================
// ����: void EEPROM_write_n(u16 EE_address,u8 *DataAddress,u16 number)
// ����: �ѻ����n���ֽ�д��ָ���׵�ַ��EEPROM.
// ����: EE_address:  д��EEPROM���׵�ַ.
//       DataAddress: д��Դ���ݵĻ�����׵�ַ.
//       number:      д����ֽڳ���.
// ����: non.
// �汾: V1.0, 2012-10-22
//========================================================================
void  EEPROM_write_n(u16 addr,u8 dat)//д����
{
	 // EA = 1;		//��ֹ�ж�

	  ISP_CONTR = (ISP_EN + ISP_WAIT_FREQUENCY);	//���õȴ�ʱ�䣬����ISP/IAP��������һ�ξ͹�
	  ISP_WRITE();							//���ֽ�д��������ı�ʱ����������������
		ISP_ADDRH = addr / 256;		//�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
		ISP_ADDRL = addr % 256;		//�͵�ַ���ֽ�
		ISP_DATA  = dat;			//�����ݵ�ISP_DATA��ֻ�����ݸı�ʱ����������
		//ISP_TRIG();
	  ISP_CMD=2;//д�ֽ�ģʽ
		ISP_TRIG = 0x5a;                //д��������(0x5a)
    ISP_TRIG = 0xa5;                //д��������(0xa5)
		_nop_();
		DisableEEPROM();
    //EA = 0;		//���������ж�
}
