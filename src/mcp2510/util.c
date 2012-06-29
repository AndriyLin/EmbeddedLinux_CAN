#include "final.h"
#include <unistd.h>

//data数组的长度为8！
void send(int dev, char* data)
{
	int index = 0;

	//输入发送数据
	char TXdata[16];
	TXdata[0] =  0x0f;	//to be modified
	TXdata[1] =  0x0f;
	TXdata[2] =  0x0e;
	TXdata[3] =  0x00;

	//4 - 11, real data
	for(index=0; index<8; index++)
	{
		TXdata[4+index] = data[index];
	}

	//dlc
	TXdata[12]=  0x08;
	//isExt
	TXdata[13]=  0x00;
	//rxRTR
	TXdata[14]=  0x00;
	//padbyte
	TXdata[15]=  0x00;

	index = write(dev, TXdata, 16);	//发送数据
	printf("write result is %d\n", index);
}


