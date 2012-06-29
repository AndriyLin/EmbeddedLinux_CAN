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
int light_status = 0;
int light_display_status;

int main_door_status = EL_DOOR_CLOSE;
int sub_door_status = EL_DOOR_CLOSE;

int car_status = EL_CAR_STOP;


void show_help()
{
	printf("\n");
	printf("======================================\n");
	printf("\n");
	printf("Enter z to make car stop.\n");
	printf("Enter x to make car start.\n");
	printf("Enter c to make car run.\n");
//	printf("Enter p to print all information.\n");
	printf("\n");
	printf("======================================\n");
	printf("\n");
}

void print_all()
{
	char *car_status_map[] = 
	{
		"STOP",
		"STARTING",
		"RUNNING"
	};
	char *light_status_map[] =
	{
		"OFF", "ON"
	};
	char *door_status_map[] =
	{
		"OPEN", "CLOSED", "LOCKED"
	};

	printf("Car Status      = \033[33m%s\033[0m\n", car_status_map[car_status]);
	if (light_display_status == 1)
	{
		printf("Light Status    = \033[47;30m%s\033[0m\n", light_status_map[light_display_status]);
	}
	else
	{
		printf("Light Status    = %s\n", light_status_map[light_display_status]);
	}
	printf("MainDoor Status = \033[35m%s\033[0m\n", door_status_map[main_door_status]);
	printf("SubDoor Status  = \033[36m%s\033[0m\n", door_status_map[sub_door_status]);
}


//根据当前状态告诉LIGHT模块状态
void tell_light()
{
	char data[8];
	data[EL_BIT_TO] = EL_LIGHT;
	data[EL_BIT_FROM] = EL_MCU;
	data[EL_BIT_OP] = EL_OP_MCU_SET_LIGHT;
	data[EL_BIT_PARAM] = light_display_status;

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

		light_display_status = light_always_on;

		tell_light();
	}
}

void modify_light_semaphore(int value)
{
	light_status -= value;
	printf("ligit_semaphore = %d\n", light_status);
	printf("light_always_on = %d\n", light_always_on);
	light_display_status = (light_always_on > 0) || (light_status > 0);

	tell_light();
}

//根据当前状态告诉MAIN_DOOR其状态
void tell_main_door()
{
	char data[8];
	data[EL_BIT_TO] = EL_MAIN_DOOR;
	data[EL_BIT_FROM] = EL_MCU;
	data[EL_BIT_OP] = EL_OP_MCU_SET_MAIN_DOOR;
	data[EL_BIT_PARAM] = main_door_status;

	send(dev, data); 
}



void onSubDoor(char op, char param);



void onMainDoor(char op, char param)
{
	int change_table[3][3][3] = 
	{
		// EL_CAR_STOP
		{
			// EL_DOOR_OPEN
			{ 0, 1, 0 },
			// EL_DOOR_CLOSE
			{ 1, 0, 1 }, 
			// EL_DOOR_LOCKED
			{ 0, 1, 0 }
		},
		// EL_CAR_STARTING
		{
			// EL_DOOR_OPEN
			{ 0, 1, 0 },
			// EL_DOOR_CLOSE
			{ 0, 0, 1 }, 
			// EL_DOOR_LOCKED
			{ 0, 1, 0 }
		},
		// EL_CAR_RUNNING
		{
			// EL_DOOR_OPEN
			{ 0, 1, 0 },
			// EL_DOOR_CLOSE
			{ 0, 0, 1 }, 
			// EL_DOOR_LOCKED
			{ 0, 1, 0 }
		}
	};

	int old_main_door_state = main_door_status;
	if (change_table[car_status][main_door_status][param])
	{
		main_door_status = param;

		if (old_main_door_state == EL_DOOR_LOCKED)
		{
			// 从lock变成close
			if (sub_door_status == EL_DOOR_LOCKED)
			{
				onSubDoor(EL_OP_SUB_DOOR_SET, EL_DOOR_CLOSE);
			}
		}

		if (main_door_status == EL_DOOR_LOCKED)
		{
			// 主门从close变成了locked, 直接调用副门的onXX，毕竟若是副门不是在closed的状态，它也变不了locked。
			// 从lssssssss 来看，状态的转换会被直接过滤而不会有误解
			onSubDoor(EL_OP_SUB_DOOR_SET, EL_DOOR_LOCKED);
		}
	}
	else
	{
		printf("unable to change status\n");
		return ;
	}

	//两个门的状态更新完看下灯的状态有没有改变
	// closed -> open -> closed
	if (main_door_status == EL_DOOR_OPEN ||
			old_main_door_state == EL_DOOR_OPEN)
	{
		modify_light_semaphore(main_door_status - old_main_door_state);
	}
}

//根据当前状态告诉SUB_DOOR其状态
void tell_sub_door()
{
	char data[8];
	data[EL_BIT_TO] = EL_SUB_DOOR;
	data[EL_BIT_FROM] = EL_MCU;
	data[EL_BIT_OP] = EL_OP_MCU_SET_SUB_DOOR;
	data[EL_BIT_PARAM] = sub_door_status;

	send(dev, data); 
}

void onSubDoor(char op, char param)
{
	int change_table[3][3][3] = 
	{
		// EL_CAR_STOP
		{
			// EL_DOOR_OPEN
			{ 0, 1, 0 },
			// EL_DOOR_CLOSE
			{ 1, 0, 1 }, 
			// EL_DOOR_LOCKED
			{ 0, 1, 0 }
		},
		// EL_CAR_STARTING
		{
			// EL_DOOR_OPEN
			{ 0, 1, 0 },
			// EL_DOOR_CLOSE
			{ 0, 0, 1 }, 
			// EL_DOOR_LOCKED
			{ 0, 1, 0 }
		},
		// EL_CAR_RUNNING
		{
			// EL_DOOR_OPEN
			{ 0, 1, 0 },
			// EL_DOOR_CLOSE
			{ 0, 0, 1 }, 
			// EL_DOOR_LOCKED
			{ 0, 1, 0 }
		}
	};

	int old_sub_door_state = sub_door_status;
	if (change_table[car_status][sub_door_status][param])
	{
		sub_door_status = param;
	}
	else
	{
		printf("unable to change status\n");
	}

	//两个门的状态更新完看下灯的状态有没有改变
	// closed -> open -> closed
	if (sub_door_status == EL_DOOR_OPEN ||
			old_sub_door_state == EL_DOOR_OPEN)
	{
		modify_light_semaphore(sub_door_status - old_sub_door_state);
	}
}

void sig_usr()//接收到信号后执行的函数
{	
	int count = 0;
	char* data = buf;

	printf("IN SIG_USR()\n");

	signal(SIGIO,sig_usr);	//继续接收信号
	printf("----receive signal, in sig_usr()------\n");
	count = read(dev, buf, 8);
	if (count != 8)
	{
		printf("read count = %d, not 8, discarded!\n", count);
		return;
	}

	printf("before if\n");
	if (data[EL_BIT_TO] == EL_MCU)
	{
		printf("in if\n");
		switch (data[EL_BIT_FROM])
		{
			case EL_LIGHT:
				onLight(data[EL_BIT_OP], data[EL_BIT_PARAM]);
				break;
			case EL_MAIN_DOOR:
				onMainDoor(data[EL_BIT_OP], data[EL_BIT_PARAM]);
				break;
			case EL_SUB_DOOR:
				onSubDoor(data[EL_BIT_OP], data[EL_BIT_PARAM]);
				break;

			default:
				break;
		}
		system("clear");
		show_help();
		print_all();
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
//		printf("signal success\n");
	}

	dev = open(DEVICE_NAME, O_RDWR);
	if(dev>=0)
	{
		int char_exit = '\0';
		unsigned char to_mode = OP_NORMAL;
		unsigned char mode = -1;
		int oflag = -1;

		ioctl(dev, IOCTL_GET_MODE, &mode);
		if (mode != to_mode)
		{
			//设置为to_mode模式
			if (ioctl(dev, IOCTL_MOD_SET, &to_mode) == 1)
			{
				//printf("set mode %d, ioctl success\n", to_mode);
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
		print_all();
		while((char_exit = getchar()) != 'q')
		{
			if (/*char_exit == '\n' || */char_exit == '\r')
			{
				continue;
			}
			else
			{
				system("clear");
				show_help();

				switch (char_exit)
				{
					case EL_CHAR_CAR_STOP:
						car_status = EL_CAR_STOP;
						break;

					case EL_CHAR_CAR_START:
						car_status = EL_CAR_STARTING;
						break;

					case EL_CHAR_CAR_RUN:
						car_status = EL_CAR_RUNNING;
						break;

					case EL_CHAR_PRINT:
		//				print_all();
						break;

					default:
						break;
				}
				print_all();
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
