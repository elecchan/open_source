#include <all.h>


void main()
{
   P5M0 = 0;
   P5M1 = 0;
   P3M0 = 0;
   P3M1 = 0;
   UartInit();	   	//9600bps@30MHz ��ʱ��1�����ʷ�����
   Modbus_Init();  //���ö�ʱ��2����������֡ ��ʱ30MHz@1MS
   while(1)
   {

	   	Modbus_data_Decode();
		
   }
}