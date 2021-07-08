#ifndef UART
#define UART


extern unsigned char UART_Byte_Value;  //发送字节计数
extern unsigned char UART_OFF;//发送结束标志符

extern void UartInit(void);		//9600bps@30MHz





#endif