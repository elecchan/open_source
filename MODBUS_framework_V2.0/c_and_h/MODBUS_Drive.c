#include <all.h>

unsigned char xdata Modbus_Read_Buffer[256] = 0;  //���ݽ��ջ�����
unsigned char xdata Modbus_Write_Buffer[256] = 0; //���ݼĴ���
unsigned char xdata Modbus_Send[256] = 0;//���ݷ�������
unsigned char xdata Modbus_CRC[20]= {0};

unsigned int Modbus_Read_Value = 0;	//�����ֽڼ���
unsigned int Modbus_Write_Value = 0;  //�����ֽڼ���				    
unsigned char Modbus_Read_Complete = 0; //֡������ɱ�־λ
unsigned int CRC_Reg = 0xffff;
unsigned char CRC_Value = 0;
unsigned int Modbus_CRC_Cecode(unsigned char *puchMsg, unsigned int usDataLen);	//CRCУ��
void Modbus_Init();	//֡��ⶨʱ��
void Modbus_data_Decode();


void Modbus_Init()//���ö�ʱ��2����������֡ ��ʱ1MS   30MHz@1ms
{
	AUXR |= 0x04;//��ʱ��2������ʱ������Ƶ
	IE2 |= 0x04;//����ʱ��2�ж�
	T2L = 0xd0;		//���ö�ʱ��ֵ
	T2H = 0x8a;		//���ö�ʱ��ֵ
	EA = 1;  //�����ж�
	AUXR |= 0x10;
}



//****************************************************************************
//��������Modbus_CRC_Cecode
//�������ã�����CRC_16��У�����
//�������1��Ҫ�������ݵ���������
//�������2��Ҫ�������ݵ��ֽڳ���
//�������������õ�CRCֵ��16λ����
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
//��������Modbus_03
//�������ã�����ִ��MODBUSЭ����03��Ĺ���
//�����������
//�����������
//��ע���Ĵ�����ַ 0X00~0XFF;
//****************************************************************************
void Modbus_03(void)
{
	unsigned int i;
	unsigned int CRC = 0;
	P55 = ~P55;//���԰��״ָ̬ʾ��

	Modbus_Read_Value = 0;//�����������㣬����Ҫ������MODBUSָ�������Ҫ��λ�ã�Ӱ��ܴ�
//	for(i = 0; i < 256; i++)//�����ø��ڲ��Ĵ��� ��ֵ   (ʹ��ʱ����ע��)
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
 	UART_Byte_Value = (Modbus_Send[2] + 5);//���÷���ǰҪ���÷����ֽڳ��ȣ�ÿ�η���ǰ��Ҫ����
	while(!UART_OFF);//��������ǰ���һ�´����Ƿ�æ
	UART_OFF = 0;//���÷���ǰҪ�ѷ��ͽ�����־λ���㣬ÿ�η���ǰ��Ҫ����
 	SBUF = Modbus_Send[0];

	 for(i = 0; i < 256; i++)//��շ��ͻ���������
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
			 //������Կ�ʼ�������� ������������֤��CRC�����Ѿ�ͨ����
			 if(Modbus_Read_Buffer[0] == (unsigned char)MODBUS_ADD)
			 {
			  	switch(Modbus_Read_Buffer[1])
				 {
				 	case 03: Modbus_03(); break;
				 }
			 }
			 else
			 {
			 	  Modbus_Read_Value = 0;//�����������㣬����Ҫ������MODBUSָ�������Ҫ��λ�ã�Ӱ��ܴ�
			 }
		 }
		 
	 }
}

/* ���ڽ��ռ�أ��ɿ���ʱ���ж�֡���������ڶ�ʱ�ж��е��ã�ms-��ʱ��� */
void UartRxMonitor(unsigned char ms)
{
    static unsigned char cntbkp = 0;
    static unsigned char idletmr = 0;

    if (Modbus_Read_Value > 0)  //���ռ�����������ʱ��������߿���ʱ��
    {
        if (cntbkp != Modbus_Read_Value)  //���ռ������ı䣬���ս��յ�����ʱ��������м�ʱ
        {
            cntbkp = Modbus_Read_Value;
            idletmr = 0;
        }
        else                   //���ռ�����δ�ı䣬�����߿���ʱ���ۻ�����ʱ��
        {
            if (idletmr < 3)  //���м�ʱС��3msʱ�������ۼ�
            {
                idletmr += ms;
                if (idletmr >= 3)  //����ʱ��ﵽ3msʱ�����ж�Ϊһ֡�������
                {
					Modbus_Read_Complete = 1;  //����֡������ɱ�־
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

