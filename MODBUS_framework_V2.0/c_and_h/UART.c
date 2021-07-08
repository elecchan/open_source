#include <all.h>


unsigned char UART_Byte_Value = 1;  //�����ֽڼ���
unsigned char UART_OFF = 1;//���ͽ�����־��
void UartInit(void);		//9600bps@30MHz

void UartInit(void)		//9600bps@30MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
	TL1 = 0xf3;		//�趨��ʱ��ֵ
	TH1 = 0xFc;		//�趨��ʱ��ֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1
	EA = 1;
	ES = 1;
}


void UART1_Rountine(void) interrupt 4	//����1
{
	if(RI)
	{	
	 	unsigned char  j,Check,Data,CRC_H,CRC_L;
			
		Modbus_CRC[1] = Modbus_CRC[2];
		Modbus_CRC[0] = Modbus_CRC[3];
		Modbus_CRC[3] = Modbus_CRC[5];
		Modbus_CRC[2] = Modbus_CRC[4];
		Modbus_Read_Buffer[Modbus_Read_Value++] = SBUF;
		Data = SBUF;
		CRC_Reg = CRC_Reg ^ Data;
		for(j=0;j<8;j++)
		{
			Check = CRC_Reg & 0x0001;
			CRC_Reg >>= 1;
			if(Check==0x0001)
			{
				CRC_Reg ^= 0xA001;
			}
		
		}
		CRC_H = CRC_Reg >> 8;
		CRC_L = CRC_Reg & 0x00ff;
		Modbus_CRC[5] = CRC_L;
		Modbus_CRC[4] = CRC_H;	    

		RI = 0;		
	}
	if(TI)
		{			
			static unsigned char Send_Value = 1;
			TI = 0;
			if(!UART_OFF)
			{
				 if(Send_Value < UART_Byte_Value)
				{
				 	SBUF = Modbus_Send[Send_Value++];
				}
				else
				{
					 UART_Byte_Value = 0;
					 Send_Value = 1;
					 UART_OFF = 1; 
				}
			}
			
		}	
}






