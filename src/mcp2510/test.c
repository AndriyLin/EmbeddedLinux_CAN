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
char *buf;
int dev;
void sig_usr()//接收到信号后执行的函数
{	
	int count,i;
	signal(SIGIO,sig_usr);	//继续接收信号
	printf("----receive signal1------\n");
	count=read(dev,buf,8); //读取接收到的数据
	if(count==8)
	{
		printf("receive 8 Bytes\n");
		for(i = 0; i < 8; i++)
			printf("buf[%d] is %x\n",i,buf[i]);
	}
	else printf("read failed! \n");		
}
int main()
{
	int j,count,i;	
	int oflag;
	char charexit;
	struct sigaction sigact,oldact;
	char *TXdata=(char *)malloc(11*sizeof(char));
	buf=(char *)malloc(111*sizeof(char));
	//输入发送数据
	TXdata[0] =  0x03;
	TXdata[1] =  0x00;
	TXdata[2] =  0x08;
	TXdata[3] =  0x08 ;
	TXdata[4] =  0x07 ;
	TXdata[5] =  0x06 ;
	TXdata[6] =  0x05 ;
	TXdata[7] =  0x04 ;
	TXdata[8] = 0x03;
	TXdata[9] =  0x02 ;
	TXdata[10]=  0x01 ;	
	signal(SIGIO,sig_usr);//等待信号
	dev=open(DEVICE_NAME,O_RDWR);
	if(dev>=0)
	{
		fcntl(dev,F_SETOWN,getpid());//将用户进程号写到设备文件中，让驱动发送信号到
		//用户进程
		oflag=fcntl(dev,F_GETFL);
		fcntl(dev,F_SETFL,oflag|FASYNC);

		write(dev,TXdata,11);//发送数据
		scanf("%c",&charexit);
		while(charexit != 'q')scanf("%c",&charexit);//接收’q’退出
	}
	else printf("Open  falsed !\n");
	close(dev);
	free(buf);
	free(TXdata);
	return 0; 
}
