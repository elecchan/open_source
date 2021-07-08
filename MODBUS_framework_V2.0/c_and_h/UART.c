#include <all.h>


unsigned char UART_Byte_Value = 1;  //发送字节计数
unsigned char UART_OFF = 1;//发送结束标志符
void UartInit(void);		//9600bps@30MHz

void UartInit(void)		//9600bps@30MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xf3;		//设定定时初值
	TH1 = 0xFc;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	EA = 1;
	ES = 1;
}


void UART1_Rountine(void) interrupt 4	//串口1
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






