#ifndef MODBUS_Drive 
#define	MODBUS_Drive


#define MODBUS_ADD 1      //��ַ

extern void Modbus_Init();
extern void Modbus_Read_Drive();
extern void Modbus_data_Decode();


extern unsigned char xdata Modbus_Read_Buffer[256];	//��������
extern unsigned char xdata Modbus_Write_Buffer[256];//�Ĵ���
extern unsigned char xdata Modbus_Send[256];//���ݷ�������
extern unsigned char xdata Modbus_CRC[20];
extern unsigned char Modbus_Read_Complete;
extern unsigned int Modbus_Read_Value;
extern unsigned int Modbus_Write_Value;
extern unsigned char Modbus_Read_Complete;
extern unsigned char CRC_Value;
extern unsigned int CRC_Reg;

#endif