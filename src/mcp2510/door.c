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


void show_main_door(char status)
{
	printf("in show_main_door()\n");

	printf("MainDoor State = ");

	switch (status)
	{
	case EL_DOOR_OPEN:
		printf("OPEN");
		break;
	case EL_DOOR_CLOSE:
		printf("CLOSED");
		break;
	case EL_DOOR_LOCKED:
		printf("LOCKED");
		break;
	}

	printf("\n");
}

void show_sub_door(char status)
{
	printf("in show_sub_door()\n");

	printf("SubDoor State  = ");

	switch (status)
	{
	case EL_DOOR_OPEN:
		printf("OPEN");
		break;
	case EL_DOOR_CLOSE:
		printf("CLOSED");
		break;
	case EL_DOOR_LOCKED:
		printf("LOCKED");
		break;
	}

	printf("\n");
}

void sig_usr()//接收到信号后执行的函数
{	
	int count = 0;
	char* data = buf;

	signal(SIGIO,sig_usr);	//继续接收信号
	printf("----receive signal, in sig_usr()------\n");
	count = read(dev, buf, 8);
	printf("read count = %d \n", count);

	if (data[EL_BIT_TO] == EL_MAIN_DOOR)
	{
		if (data[EL_BIT_FROM] == EL_MCU && data[EL_BIT_OP] == EL_OP_MCU_SET_MAIN_DOOR)
		{
			show_main_door(data[EL_BIT_PARAM]);
		}
	}

	if (data[EL_BIT_TO] == EL_SUB_DOOR)
	{
		if (data[EL_BIT_FROM] == EL_MCU && data[EL_BIT_OP] == EL_OP_MCU_SET_SUB_DOOR)
		{
			show_sub_door(data[EL_BIT_PARAM]);
		}
	}
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

		while((char_exit = getchar()) != 'q')
		{
			if (char_exit == '\n' || char_exit == '\r')
			{
				continue;
			}
			else
			{
				data[EL_BIT_TO] = EL_MCU;
				switch (char_exit)
				{
				case EL_CHAR_MAIN_DOOR_OPEN:
					data[EL_BIT_FROM] = EL_MAIN_DOOR;
					data[EL_BIT_OP] = EL_OP_MAIN_DOOR_SET;
					data[EL_BIT_PARAM] = EL_DOOR_OPEN;
					send(dev, data);
					break;

				case EL_CHAR_MAIN_DOOR_CLOSE:
					data[EL_BIT_FROM] = EL_MAIN_DOOR;
					data[EL_BIT_OP] = EL_OP_MAIN_DOOR_SET;
					data[EL_BIT_PARAM] = EL_DOOR_CLOSE;
					send(dev, data);
					break;
				
				case EL_CHAR_MAIN_DOOR_LOCK:
					data[EL_BIT_FROM] = EL_MAIN_DOOR;
					data[EL_BIT_OP] = EL_OP_MAIN_DOOR_SET;
					data[EL_BIT_PARAM] = EL_DOOR_LOCKED;
					send(dev, data);
					break;

				case EL_CHAR_SUB_DOOR_OPEN:
					data[EL_BIT_FROM] = EL_SUB_DOOR;
					data[EL_BIT_OP] = EL_OP_SUB_DOOR_SET;
					data[EL_BIT_PARAM] = EL_DOOR_OPEN;
					send(dev, data);
					break;

				case EL_CHAR_SUB_DOOR_CLOSE:
					data[EL_BIT_FROM] = EL_SUB_DOOR;
					data[EL_BIT_OP] = EL_OP_SUB_DOOR_SET;
					data[EL_BIT_PARAM] = EL_DOOR_CLOSE;
					send(dev, data);
					break;

				case EL_CHAR_SUB_DOOR_LOCK:
					data[EL_BIT_FROM] = EL_SUB_DOOR;
					data[EL_BIT_OP] = EL_OP_SUB_DOOR_SET;
					data[EL_BIT_PARAM] = EL_DOOR_LOCKED;
					send(dev, data);
					break;

				default:
					break;
				}
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
