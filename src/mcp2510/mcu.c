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

int light_always_on = FALSE;
int light_status = EL_LIGHT_OFF;

int main_door_status = EL_DOOR_CLOSE;
int sub_door_status = EL_DOOR_CLOSE;


//to send message to LIGHT MODULE to set light on/off
void to_set_light()
{
	char data[8];
	data[EL_BIT_TO] = EL_LIGHT;
	data[EL_BIT_FROM] = EL_MCU;
	data[EL_BIT_OP] = EL_OP_MCU_SET_LIGHT_ONOFF;
	if (light_always_on)
	{
		data[EL_BIT_PARAM] = EL_LIGHT_ON;
	}
	else
	{
		data[EL_BIT_PARAM] = light_status;
	}

	send(dev, data); 
}


//处理从Light来的信息
void onLight(char op, char param)
{
	if (op == EL_OP_LIGHT_SET_ALWAYS_ON)
	{
		//改变always_on的设置，取反
		light_always_on = light_always_on ? FALSE : TRUE;
		printf("LIGHT MODULE change LIGHT_ALWAYS_ON to %d\n", light_always_on);

		to_set_light();
	}
}

void onMainDoor(char op, char param)
{
	//TODO
}

void onSubDoor(char op, char param)
{
	//TODO
}

void sig_usr()//接收到信号后执行的函数
{	
	int count = 0;
	char* data = buf;

	signal(SIGIO,sig_usr);	//继续接收信号
	printf("----receive signal, in sig_usr()------\n");
	count = read(dev, buf, 8);
	printf("read count = %d \n", count);

	if (data[EL_BIT_TO] != EL_MCU)
	{
		//not sent to MCU, discard it
		return;
	}

	if (data[EL_BIT_FROM] == EL_LIGHT)
	{
		onLight(data[EL_BIT_OP], data[EL_BIT_PARAM]);
	}
	else if (data[EL_BIT_FROM] == EL_MAIN_DOOR)
	{
		onMainDoor(data[EL_BIT_OP], data[EL_BIT_PARAM]);
	}
	else if (data[EL_BIT_FROM]== EL_SUB_DOOR)
	{
		onSubDoor(data[EL_BIT_OP], data[EL_BIT_PARAM]);
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
