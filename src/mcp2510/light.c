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
	//TODO
	printf("in show_light_on()\n");
}

void show_light_off()
{
	//TODO
	printf("in show_light_off()\n");
}

void sig_usr()//接收到信号后执行的函数
{	
	int count = 0;
	char* data = buf;

	signal(SIGIO,sig_usr);	//继续接收信号
	printf("----receive signal, in sig_usr()------\n");
	count = read(dev, buf, 8);
	printf("read count = %d \n", count);

	if (data[EL_BIT_TO] != EL_LIGHT)
	{
		//not sent to LIGHT
		return;
	}

	if (data[EL_BIT_FROM] == EL_MCU && data[EL_BIT_OP] == EL_OP_MCU_SET_LIGHT_ONOFF)
	{
		switch (data[EL_BIT_PARAM])
		{
		case EL_LIGHT_ON:
			show_light_on();
			break;
			
		case EL_LIGHT_OFF:
			show_light_off();
			break;

		default:
			break;
		}
	}
}

void show_help()
{
	printf("to make light always on or not, enter \'o\'\n");
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
		char data[8];

		ioctl(dev, IOCTL_GET_MODE, &mode);
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

		show_help();
		while((char_exit = getchar()) != 'q')
		{
			if (char_exit == '\n' || char_exit == '\r')
			{
				continue;
			}

			switch(char_exit)
			{
			{
			case 'o':
				//Set always on or set not always on
				data[EL_BIT_TO] = EL_MCU;
				data[EL_BIT_FROM] = EL_LIGHT;
				data[EL_BIT_OP] = EL_OP_LIGHT_SET_ALWAYS_ON;
				data[EL_BIT_PARAM] = 0;	//useless

				send(dev, data); 
				break;
			}

			default:
				break;
			}

			show_help();
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
