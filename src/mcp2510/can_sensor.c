#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/config.h>
#include<linux/init.h>

#include<linux/fs.h>
#include<linux/errno.h>
#include<linux/types.h>
#include<linux/fcntl.h>
#include<linux/devfs_fs_kernel.h> 
#include<asm/io.h>
#include<asm/uaccess.h>

#include<linux/time.h>
#include<linux/timer.h>

#include<linux/signal.h>
#include<linux/sched.h>

#include<linux/interrupt.h>
#include <linux/blkdev.h>

#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/wait.h>
#include <asm/irq.h>
#include <linux/delay.h>

#include "spi.h"
#include "mcp2510.h"
#include "spi.c"
#include "mcp2510.c"

#define INT_DEV_NAME  "candev"
#define INT_DEV_MAJOR 240
#define SIG_MYINT 33
#define TIME_STEP  (1000)

// IRQ_EINT2 may be useless, try IRQ_EINT1 instead


struct fasync_struct *can_async_queue;
static struct timer_list *int_timer=NULL;
void inttimer_register(struct timer_list *pdata,unsigned long timeover);

//interrupt handler for power1 disconnect
//interrupt handler for power2 disconnect
//interrupt handler for cam1 disconnect
//interrupt handler for cam2 disconnect

//triggered the interrupt on rise edge.
//注：我不知道这是干什么的...
void pwr1_dic_interrupt(int irq,void *d,struct pt_regs *regs)
{	
	unsigned char buffer,tmp;

	disable_irq(IRQ_EINT1);

	//kill signal SIGIO to application

	//because it is in local ,there is no intervene	

	SRCPND &= (~0x00000002);
	INTPND = INTPND;

	enable_irq(IRQ_EINT1);
}

//注：应该是timer计时结束后进行的操作
void inttimer_timeover(unsigned long arg)
{
	//unsigned char buffer;
	printk("\n===============In timer--ing==================\n");

	//	buffer = ReadStatus_Instr_2510();

	//	printk("Now can state is %x\n",buffer);

	/*	if((buffer&RX0IF)>0)
	{
	can_data_receive(0);
	BitModify_Instr_2510(CANINTF, RX0IF, 0);
	kill_fasync(&can_async_queue,SIGIO,POLL_OUT);
	}
	else if((buffer&RX1IF)>0)
	{
	can_data_receive(1);
	BitModify_Instr_2510(CANINTF, RX1IF, 0);
	kill_fasync(&can_async_queue,SIGIO,POLL_OUT);
	}
	*/
	inttimer_register(int_timer,TIME_STEP);

}

//注：应该是timer的注册
void inttimer_register(struct timer_list *pdata,unsigned long timeover)
{
	init_timer(pdata);
	pdata->expires=jiffies+timeover;
	pdata->data=(unsigned long)pdata;
	pdata->function=inttimer_timeover;
	add_timer(pdata);
}

/************************************************************************/
/* 以下各个函数都是为了下边的这个can_sensor_dev_fops准备的              */
/************************************************************************/

//注：是CAN总线的fops里需要定义好的函数，可是这个can_sensor.c是所谓的"CAN sensor manager Module"
//在can_sensor_dev_fops里只有can_sensor_open，所以会不会调用这个函数呢？不好说
int can_open(struct inode *inode,struct file *filp)
{
	int flags;

	MOD_INC_USE_COUNT;

	local_irq_save(flags);
	Init_SPI();
	Init_MCP2510();
	local_irq_restore(flags);

	/********************set timer******************************************************/
	int_timer=kmalloc(sizeof(struct timer_list),GFP_KERNEL);
	if(int_timer==NULL)return -1;
	memset(int_timer,0,sizeof(struct timer_list));
	inttimer_register(int_timer,TIME_STEP);

	return 0;
}

static int can_ioctl(struct inode *inode,struct file *filp,unsigned int cmd,unsigned long arg)
{
	unsigned char tmp;

	switch(cmd)
	{
	case IOCTL_MOD_SET:
		copy_from_user(&tmp,(unsigned char *)arg,1);
		printk("============IOCTL MODE SET===============\n");
		switch(tmp){
		case OP_NORMAL:
			MCP2510_SetNormalMode();
			return 1;
		case OP_LISTEN_ONLY:
			MCP2510_SetListenOnlyMode();
			return 1;
		case OP_LOOPBACK:
			MCP2510_SetLoopbackMode();
			return 1;
		case OP_SLEEP:
			MCP2510_SetSleepMode();
			return 1;
		case OP_CONFIG:
			MCP2510_SetConfigMode();
			return 1;
		default:
			return -1;
		}
		break;
	case IOCTL_GET_MODE:
		if(!arg)
			return -1;

		printk("============IOCTL GET MODE===============\n");
		tmp= Read_Instr_2510(CANSTAT);
		tmp=((tmp&0xe0)>>1);
		copy_to_user((unsigned char *)arg,&tmp,1);
		break;
	case IOCTL_GET_CANSTAT:
		if(!arg)
			return -1;

		printk("============IOCTL GET CANSTAT===============\n");
		tmp= ReadStatus_Instr_2510();
		copy_to_user((unsigned char*)arg,&tmp,1);
		break;
	default:
		break;	
	}
	return 0;
}

static ssize_t can_read(struct file *filp,char *buf,size_t count,loff_t *f_pos)
{
	ssize_t ret=0;

	printk("in can_read() in can_sensor.c\n");
	down_interruptible(&rx_mutex);

	if(RXbuffer.count>0) //valid message
	{
		copy_to_user(buf,(char *)(RXbuffer.RXdata+RXbuffer.head),16);			
		RXbuffer.head=(RXbuffer.head+1)%RXBUFLEN;
		RXbuffer.count--;
		ret=16;
	}
	else 
	{
		ret=-1;;
	}

	up(&rx_mutex);

	return ret;
}


static ssize_t can_write(struct file *filp,const char *buf,size_t count,loff_t *f_pos)
{
	int ret=0;

	down_interruptible(&tx_mutex);

	if(TXbuffer.count<TXBUFLEN) 
		TXbuffer.count++;
	else return -1;

	copy_from_user(TXbuffer.TXdata+(TXbuffer.head+TXbuffer.count-1)%TXBUFLEN,(PCanData *)buf,16);

	printk("In can_write:TXdata tail is %d\n",TXbuffer.TXdata[(TXbuffer.head+TXbuffer.count-1)%TXBUFLEN].dlc);

	//TXdata[0].dlc=count;

	//Transmit data to Bus	
	ret=can_data_send(TXbuffer.head,0);

	TXbuffer.head=(TXbuffer.head+1)%TXBUFLEN;
	TXbuffer.count--;

	up(&tx_mutex);

	return ret;
}


int can_fasync(int fd, struct file *filp, int mode)
{
	return fasync_helper(fd,filp,mode,&can_async_queue);
}


int can_release(struct inode *inode,struct file *filp)
{
	MOD_DEC_USE_COUNT;

	/********************disable mcp2510************************************************/
	disable2510();
	/********************set mcp2510 into reset mode************************************/
	Reset_Instr_2510();
	/********************close the interrupt of ENT2*************************************
	In case the glitch of EINT1 intervene the system.
	************************************************************************************/
	disable_irq(IRQ_EINT1);  

	can_fasync(-1,filp,0);
	return 0;
}


//test
void test(void)
{
	unsigned char buffer;
	spi_tx_data(0xff);
	buffer= rSPRDAT0;
	printk("\n====buffer %x======\n",buffer);
	spi_tx_data(0x34);
	buffer= rSPRDAT0;
	printk("\n====buffer %x======\n",buffer);
}

/************************************************************************/
/* 以上各个函数都是为了下边的这个can_sensor_dev_fops准备的              */
/************************************************************************/

struct file_operations can_sensor_dev_fops=
{
read  :can_sensor_read,
write :can_sensor_write,
ioctl  :can_sensor_ioctl,
open :can_sensor_open,
release :can_sensor_release,
fasync :can_sensor_fasync,	

};

//注：初始化module，insmod时
int init_module(void)
{
	unsigned int result; 
	int flags;

	/********************set timer******************************************************/
	/*	int_timer=kmalloc(sizeof(struct timer_list),GFP_KERNEL);
	if(int_timer==NULL)return -1;
	memset(int_timer,0,sizeof(struct timer_list));
	inttimer_register(int_timer,TIME_STEP);
	*/

	/********************set irq ofr EINT1**********************************************/
	local_irq_save(flags);
	set_external_irq(IRQ_EINT1, EXT_FALLING_EDGE, GPIO_PULLUP_DIS);
	disable_irq(IRQ_EINT1);
	enable_irq(IRQ_EINT1);	
	local_irq_restore(flags);
	result = request_irq(IRQ_EINT1,&can_interrupt,SA_INTERRUPT,"can",NULL);
	if (result)
	{
		printk("Can't get assigned irq %d,result=%d\n",IRQ_EINT1,result);
		return result;
	}

	devfs_register(NULL,INT_DEV_NAME,DEVFS_FL_AUTO_DEVNUM,INT_DEV_MAJOR,
		1,S_IFCHR|S_IRUGO|S_IWUGO,&candev_fops,NULL);	

	//result=register_chrdev(INT_DEV_MAJOR,INT_DEV_NAME,&candev_fops); 
	//if(result<0)printk("insmod failed\n");
	//else printk("insmod successfully %d\n",result);

	/********************do address map from physical address to virtual address*********/
	address_map();	

	//init the mutex semaphore
	init_MUTEX(&rx_mutex);
	init_MUTEX(&tx_mutex);

	return 0;
}

//注：清理掉，rmmod
void cleanup_module(void)
{
	disable_irq(IRQ_EINT1);
	free_irq(IRQ_EINT1, NULL);

	if(int_timer!=NULL)
	{		
		if(del_timer(int_timer)==1)printk("---close timer-----\n");
		kfree(int_timer);
	}

	unregister_chrdev(INT_DEV_MAJOR,INT_DEV_NAME);
	printk("rmmod successfully\n");
}


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Luoyigui @Yuqian electronics science technology com. ltd.");
MODULE_DESCRIPTION("CAN sensor manager Module");
