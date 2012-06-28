#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>

#include "mcp2510.h"

//#define DEVICE_NAME "/dev/candev"
#define DEVICE_NAME "/dev/tmpdev"


char buf[111];
int dev;

typedef void (*sighandler_t)(int);

void sig_usr()//接收到信号后执行的函数
{	
	int count = 0;

	signal(SIGIO,sig_usr);	//继续接收信号
	printf("----receive signal, in sig_usr()------\n");
	count = read(dev, buf, 8);
	printf("read count = %d \n", count);

	//TODO
}

//data数组的长度为8！
void send(char* data)
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
	TXdata[12]=  0x06;
	//isExt
	TXdata[13]=  0x00;
	//rxRTR
	TXdata[14]=  0x00;
	//padbyte
	TXdata[15]=  0x00;

	write(dev, TXdata, 16);	//发送数据
}

int main()
{
	sighandler_t prev_handler = NULL;
	prev_handler = signal(SIGIO, sig_usr);//等待信号
	if (prev_handler == SIG_ERR)
	{
		printf("returns SIG_ERR, signal failed\n");
	}
	else
	{
		printf("signal success\n");
	}

	dev = open(DEVICE_NAME, O_RDWR);
	if(dev>=0)
	{
		int char_exit = '\0';
		unsigned char to_mode = OP_NORMAL;
		unsigned char mode = -1;
		
		printf("preparing to set loopback: %d\n", OP_LOOPBACK);
		ioctl(dev, IOCTL_GET_MODE, &mode);
		printf("current mode(to set loopback) is %d, by Andriy\n", mode);
		
		if (mode != to_mode)
		{
			//设置为to_mode模式
			if (ioctl(dev, IOCTL_MOD_SET, &to_mode) == 1)
			{
				printf("set mode %d, ioctl success\n", to_mode);
			}
			else
			{
				printf("set mode %d, ioctl returns not 1, so failed\n", to_mode);
			}
		}

		while((char_exit = getchar()) != 'q')
		{
			if (char_exit == '\n' || char_exit == '\r')
			{
				//TODO
			}
			else
			{
			}
		}
	}
	else
	{
		printf("Open failed !\n");
	}

	if (prev_handler != SIG_ERR)
	{
		//if last time it didn't return an error, restore the previous handler
		signal(SIGIO, prev_handler);
	}
	close(dev);
	return 0; 
}
