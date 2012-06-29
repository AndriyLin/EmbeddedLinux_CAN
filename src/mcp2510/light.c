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


void show_light_on()
{
	printf("Light is ON\n");
}

void show_light_off()
{
	printf("Light is OFF\n");
}

void sig_usr()//接收到信号后执行的函数
{	
	int count = 0;
	char* data = buf;

	signal(SIGIO,sig_usr);	//继续接收信号
	printf("----receive signal, in sig_usr()------\n");
	count = read(dev, buf, 8);
	if (count != 8)
	{
		printf("read count = %d, not 8, discarded!\n", count);
		return;
	}

	if (data[EL_BIT_TO] == EL_LIGHT)
	{
		if (data[EL_BIT_FROM] == EL_MCU && data[EL_BIT_OP] == EL_OP_MCU_SET_LIGHT)
		{
			switch (data[EL_BIT_PARAM])
			{
				case EL_LIGHT_ON:
//					show_light_on();
					show_light_off();
					break;

				case EL_LIGHT_OFF:
//					show_light_off();
					show_light_on();
					break;

				default:
					break;
			}
		} //end of EL_BIT_FROM && EL_BIT_OP
	} //end of EL_BIT_TO
}

void show_help()
{
	printf("\n");
	printf("to make light always on or not, enter o\n");
	printf("\n");
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
//		printf("signal success\n");
	}

	dev = open(DEVICE_NAME, O_RDWR);
	if(dev>=0)
	{
		int char_exit = '\0';
		unsigned char to_mode = OP_NORMAL;
		unsigned char mode = -1;
		char data[8];
		int oflag = -1;

		ioctl(dev, IOCTL_GET_MODE, &mode);
		if (mode != to_mode)
		{
			//设置为to_mode模式
			if (ioctl(dev, IOCTL_MOD_SET, &to_mode) == 1)
			{
//				printf("set mode %d, ioctl success\n", to_mode);
			}
			else
			{
				printf("set mode %d, ioctl returns not 1, so failed\n", to_mode);
			}
		}
		fcntl(dev, F_SETOWN, getpid());//将用户进程号写到设备文件中，让驱动发送信号到用户进程
		oflag = fcntl(dev, F_GETFL);
		fcntl(dev, F_SETFL, oflag|FASYNC);

		show_help();
		while((char_exit = getchar()) != 'q')
		{
			if (char_exit == '\n' || char_exit == '\r')
			{
				continue;
			}

//			system("clear");
			show_help();

			switch(char_exit)
			{
				case EL_CHAR_LIGHT_ALWAYS_ON:
					//Set always on or set not always on
					data[EL_BIT_TO] = EL_MCU;
					data[EL_BIT_FROM] = EL_LIGHT;
					data[EL_BIT_OP] = EL_OP_LIGHT_SET_ALWAYS_ON;
					data[EL_BIT_PARAM] = 0;	//useless

					send(dev, data); 
					break;

				default:
					break;
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
