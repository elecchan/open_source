#ifndef MODBUS_Drive 
#define	MODBUS_Drive


#define MODBUS_ADD 1      //地址

extern void Modbus_Init();
extern void Modbus_Read_Drive();
extern void Modbus_data_Decode();


extern unsigned char xdata Modbus_Read_Buffer[256];	//接收数组
extern unsigned char xdata Modbus_Write_Buffer[256];//寄存器
extern unsigned char xdata Modbus_Send[256];//数据发送数组
extern unsigned char xdata Modbus_CRC[20];
extern unsigned char Modbus_Read_Complete;
extern unsigned int Modbus_Read_Value;
extern unsigned int Modbus_Write_Value;
extern unsigned char Modbus_Read_Complete;
extern unsigned char CRC_Value;
extern unsigned int CRC_Reg;

#endif