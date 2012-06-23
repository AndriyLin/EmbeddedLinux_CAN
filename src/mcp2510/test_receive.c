#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>

#include "mcp2510.h"

#define DEVICE_NAME "/dev/candev"


char buf[111];
int dev;

typedef void (*sighandler_t)(int);

void sig_usr()//接收到信号后执行的函数
{	
	int count = 0;
	int i = 0;

	signal(SIGIO,sig_usr);	//继续接收信号
	printf("----receive signal1------\n");
	count = read(dev,buf,8); //读取接收到的数据
	if(count == 8)
	{
		printf("receive 8 Bytes\n");
		for(i = 0; i < 8; i++)
		{
			printf("buf[%d] is %x\n",i,buf[i]);
		}
	}
	else
	{
		printf("read failed! \n");		
	}
}

int main()
{
	sighandler_t prev_handler = NULL;

// 	//输入发送数据
// 	char TXdata[11];
// 	TXdata[0] =  0x03;
// 	TXdata[1] =  0x00;
// 	TXdata[2] =  0x08;
// 	TXdata[3] =  0x08;
// 	TXdata[4] =  0x07;
// 	TXdata[5] =  0x06;
// 	TXdata[6] =  0x05;
// 	TXdata[7] =  0x04;
// 	TXdata[8] =  0x03;
// 	TXdata[9] =  0x02;
// 	TXdata[10]=  0x01;

	prev_handler = signal(SIGIO, sig_usr);//等待信号
	dev = open(DEVICE_NAME, O_RDWR);
	if(dev>=0)
	{
		int oflag;
		int char_exit = '\0';

		fcntl(dev, F_SETOWN, getpid());//将用户进程号写到设备文件中，让驱动发送信号到用户进程
		oflag = fcntl(dev, F_GETFL);
		fcntl(dev, F_SETFL, oflag|FASYNC);

		char_exit = getchar();
		while(char_exit != 'q')
		{
			if (char_exit == '\n' || char_exit == '\r')
			{
				continue;
			}
			
// 			write(dev, TXdata, 11);//发送数据
			char_exit = getchar();
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
