#include <all.h>


void main()
{
   P5M0 = 0;
   P5M1 = 0;
   P3M0 = 0;
   P3M1 = 0;
   UartInit();	   	//9600bps@30MHz 定时器1波特率发生器
   Modbus_Init();  //利用定时器2来计算数据帧 定时30MHz@1MS
   while(1)
   {

	   	Modbus_data_Decode();
		
   }
}