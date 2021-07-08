#include <all.h>

unsigned char xdata Modbus_Read_Buffer[256] = 0;  //数据接收缓冲区
unsigned char xdata Modbus_Write_Buffer[256] = 0; //数据寄存器
unsigned char xdata Modbus_Send[256] = 0;//数据发送数组
unsigned char xdata Modbus_CRC[20]= {0};

unsigned int Modbus_Read_Value = 0;	//接收字节计数
unsigned int Modbus_Write_Value = 0;  //发送字节计数				    
unsigned char Modbus_Read_Complete = 0; //帧接收完成标志位
unsigned int CRC_Reg = 0xffff;
unsigned char CRC_Value = 0;
unsigned int Modbus_CRC_Cecode(unsigned char *puchMsg, unsigned int usDataLen);	//CRC校验
void Modbus_Init();	//帧检测定时器
void Modbus_data_Decode();


void Modbus_Init()//利用定时器2来计算数据帧 定时1MS   30MHz@1ms
{
	AUXR |= 0x04;//定时器2用作定时，不分频
	IE2 |= 0x04;//允许定时器2中断
	T2L = 0xd0;		//设置定时初值
	T2H = 0x8a;		//设置定时初值
	EA = 1;  //开总中断
	AUXR |= 0x10;
}



//****************************************************************************
//函数名：Modbus_CRC_Cecode
//函数作用：用于CRC_16的校验计算
//输入参数1：要计算数据的序列名称
//输入参数2：要计算数据的字节长度
//输出参数：计算好的CRC值，16位数据
//****************************************************************************
unsigned int Modbus_CRC_Cecode(unsigned char *puchMsg, unsigned int usDataLen)
{
	
	 unsigned int i,j,CRC_Reg1,Check1;
	 CRC_Reg1 = 0xFFFF;
	 for(i=0;i<usDataLen;i++)
	 {
		CRC_Reg1 = CRC_Reg1 ^ (unsigned int)*(puchMsg + i);
		for(j=0;j<8;j++)
		{
			Check1 = CRC_Reg1 & 0x0001;
			CRC_Reg1 >>= 1;
			if(Check1==0x0001)
			{
				CRC_Reg1 ^= 0xA001;
			}
		}
	 }
	 
	 return CRC_Reg1;
}


//****************************************************************************
//函数名：Modbus_03
//函数作用：用来执行MODBUS协议中03码的功能
//输入参数：无
//输出参数：无
//备注：寄存器地址 0X00~0XFF;
//****************************************************************************
void Modbus_03(void)
{
	unsigned int i;
	unsigned int CRC = 0;
	P55 = ~P55;//调试板的状态指示灯

	Modbus_Read_Value = 0;//接收数据清零，很重要，根据MODBUS指令放在需要的位置，影响很大
//	for(i = 0; i < 256; i++)//调试用给内部寄存器 赋值   (使用时可以注释)
//	{
//		Modbus_Write_Buffer[i] = i;	
//	}			

	Modbus_Send[0] = Modbus_Read_Buffer[0];
	Modbus_Send[1] = Modbus_Read_Buffer[1];
	Modbus_Send[2] = (Modbus_Read_Buffer[5] * 2);

 	for(i = 3; i < (Modbus_Send[2]+3); i++)	 
	{

		Modbus_Send[i] = Modbus_Write_Buffer[(i + Modbus_Read_Buffer[3] - 3)];	 
	}
	CRC = Modbus_CRC_Cecode(Modbus_Send,(Modbus_Send[2]+3));
	Modbus_Send[(Modbus_Send[2] + 3)] = CRC & 0x00ff;	  							
 	Modbus_Send[(Modbus_Send[2] + 4)] = CRC >> 8;	  					  
 	UART_Byte_Value = (Modbus_Send[2] + 5);//启用发送前要设置发送字节长度，每次发送前都要设置
	while(!UART_OFF);//发送数据前检测一下串口是否繁忙
	UART_OFF = 0;//启用发送前要把发送结束标志位清零，每次发送前都要设置
 	SBUF = Modbus_Send[0];

	 for(i = 0; i < 256; i++)//清空发送缓存区数据
	{
		Modbus_Read_Buffer[i] = 0;;	
	}
	Modbus_CRC[0] = 0;
	Modbus_CRC[1] = 0;	
}

void Modbus_data_Decode()
{
	 	 
	if(Modbus_Read_Complete)
	{		 
		 Modbus_Read_Complete = 0;
	
		 if((Modbus_CRC[1] == Modbus_Read_Buffer[Modbus_Read_Value-1]) && (Modbus_CRC[0] == Modbus_Read_Buffer[Modbus_Read_Value-2]))

		 {
			 //这里可以开始分析数据 程序进入这里就证明CRC检验已经通过了
			 if(Modbus_Read_Buffer[0] == (unsigned char)MODBUS_ADD)
			 {
			  	switch(Modbus_Read_Buffer[1])
				 {
				 	case 03: Modbus_03(); break;
				 }
			 }
			 else
			 {
			 	  Modbus_Read_Value = 0;//接收数据清零，很重要，根据MODBUS指令放在需要的位置，影响很大
			 }
		 }
		 
	 }
}

/* 串口接收监控，由空闲时间判定帧结束，需在定时中断中调用，ms-定时间隔 */
void UartRxMonitor(unsigned char ms)
{
    static unsigned char cntbkp = 0;
    static unsigned char idletmr = 0;

    if (Modbus_Read_Value > 0)  //接收计数器大于零时，监控总线空闲时间
    {
        if (cntbkp != Modbus_Read_Value)  //接收计数器改变，即刚接收到数据时，清零空闲计时
        {
            cntbkp = Modbus_Read_Value;
            idletmr = 0;
        }
        else                   //接收计数器未改变，即总线空闲时，累积空闲时间
        {
            if (idletmr < 3)  //空闲计时小于3ms时，持续累加
            {
                idletmr += ms;
                if (idletmr >= 3)  //空闲时间达到3ms时，即判定为一帧接收完毕
                {
					Modbus_Read_Complete = 1;  //设置帧接收完成标志
					CRC_Reg = 0xffff;
                }
            }
        }
    }
    else
    {
        cntbkp = 0;
    }
}



void Timer2_Routine() interrupt 12
{
	UartRxMonitor(1);
	
}

