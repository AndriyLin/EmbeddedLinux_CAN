#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>

#include "mcp2510.h"
#include "final.h"

//#define DEVICE_NAME "/dev/candev"
#define DEVICE_NAME "/dev/tmpdev"


char buf[111];
int dev;


void sig_usr()//接收到信号后执行的函数
{	
	int count = 0;
	int i = 0;

	signal(SIGIO,sig_usr);	//继续接收信号
	printf("----receive signal, in sig_usr()------\n");
	count = read(dev, buf, 8);
	printf("read count = %d \n", count);

	//TODO
}

int main()
{
	unsigned char mode = -1;
	sighandler_t prev_handler = NULL;

	prev_handler = signal(SIGIO, sig_usr);//等待信号
	if (prev_handler == SIG_ERR)
	{
		printf("returns SIG_ERR, signal failed\n");
	}
	else
	{
		printf("signal successed\n");
	}

	dev = open(DEVICE_NAME, O_RDWR);
	if(dev>=0)
	{
		int oflag;
		int char_exit = '\0';
		unsigned char to_mode = OP_NORMAL;
		unsigned char mode = -1;
		int i = 0;
		int count = 0;
		
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
