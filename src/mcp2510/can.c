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


struct fasync_struct *can_async_queue;
static struct timer_list *int_timer=NULL;
void inttimer_register(struct timer_list *pdata,unsigned long timeover);

//mcp2510 send an interrupt to s3c2410,and the reason caused interrupt should analysis.
//triggered the interrupt on low level edge.NOW WE USE INTERRUPT 2
void can_interrupt(int irq,void *d,struct pt_regs *regs)
{	
	unsigned char buffer,tmp;

	disable_irq(IRQ_EINT2);
       
	//Write_2510(CANINTE, 0x00); //close interrupt in mcp2510 //it is not true ,and 
	//will induce message lost. luo 20110401

	printk("in can interrupting---1--\n");
	
	buffer = Read_Instr_2510(CANINTF);	
/*	
#define MERRF 0X80
#define WAKIF 0X40
#define ERRIF 0X20
#define TX2IF 0X10
#define TX1IF 0X08
#define TX0IF 0X04
#define RX1IF 0X02
#define RX0IF 0X01
*/
	if((buffer&MERRF) >0)
	{
		printk("mcp2510 message error\n");
		tmp=Read_Instr_2510(TXB0CTRL);
		if((tmp&TXERR)>0)
			printk("TX0 transmit error\n");
		if((tmp&MLOA)>0)
			printk("TX0 transmit lost arbitration\n");
		tmp=Read_Instr_2510(TXB1CTRL);
		if((tmp&TXERR)>0)
			printk("TX1 transmit error\n");
		if((tmp&MLOA)>0)
			printk("TX1 transmit lost arbitration\n");
		tmp=Read_Instr_2510(TXB2CTRL);
		if((tmp&TXERR)>0)
			printk("TX2 transmit error\n");
		if((tmp&MLOA)>0)
			printk("TX2 transmit lost arbitration\n");
		BitModify_Instr_2510(CANINTF, MERRF, 0);		
	}
    if((buffer&WAKIF) >0)
	{
		printk("mcp2510 wake up\n");
		BitModify_Instr_2510(CANINTF, WAKIF, 0);		
	}
	if((buffer&ERRIF) >0)
	{
		tmp=Read_Instr_2510(EFLG);
		printk("mcp2510 ERROR and EFLG is %X\n",tmp);
		if((tmp&RX1OVR)>0) printk("RX1 overflow\n");
		if((tmp&RX0OVR)>0) printk("RX0 overflow\n");
		if((tmp&TXBO)>0) printk("Transmit Error Count reach 255,Bus-off error \n");
		if((tmp&TXEP)>0) printk("Transmit Error Count >=128\n");
		if((tmp&RXEP)>0) printk("Receive Error Count >=128\n");
		if((tmp&TXWAR)>0) printk("Transmit Error Count >=96\n");
		if((tmp&RXWAR)>0) printk("Receive Error Count >=96\n");
		if((tmp&EWARN)>0) printk("Receive Error Count >=96 or Transmit Error Count >=96\n");
		BitModify_Instr_2510(CANINTF, ERRIF, 0);	
	}
	if((buffer&TX2IF) >0)
	{
		printk("mcp2510 TX2 BUFFER BECOMING EMPTY\n");	
		BitModify_Instr_2510(CANINTF, TX2IF, 0);	
	}
	if((buffer&TX1IF) >0)
	{
		printk("mcp2510 TX1 BUFFER BECOMING EMPTY\n");	
		BitModify_Instr_2510(CANINTF, TX1IF, 0);	
	}
	if((buffer&TX0IF) >0)
	{
		printk("mcp2510 TX0 BUFFER BECOMING EMPTY\n");	
		BitModify_Instr_2510(CANINTF, TX0IF, 0);	
	}
	if((buffer&RX1IF)>0)
	{
		printk("mcp2510 RX1 BUFFER BECOMING FULL\n");	
		can_data_receive(1);
		BitModify_Instr_2510(CANINTF, RX1IF, 0);
		kill_fasync(&can_async_queue,SIGIO,POLL_OUT);
	}
    if((buffer&RX0IF)>0)
	{
		printk("mcp2510 RX0 BUFFER BECOMING FULL\n");	
		can_data_receive(0);
		BitModify_Instr_2510(CANINTF, RX0IF, 0);
		kill_fasync(&can_async_queue,SIGIO,POLL_OUT);
	}  	
	
	SRCPND &= (~0x00000002);    
   	INTPND = INTPND;
	
	enable_irq(IRQ_EINT2);	   			

}

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

void inttimer_register(struct timer_list *pdata,unsigned long timeover)
{
	init_timer(pdata);
	pdata->expires=jiffies+timeover;
	pdata->data=(unsigned long)pdata;
	pdata->function=inttimer_timeover;
	add_timer(pdata);
}


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

   /*
	spi_tx_data(0xff);
	spi_tx_data(0xff);
	spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
	*/
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

//read the head data from RXbuffer 
static ssize_t can_read(struct file *filp,char *buf,size_t count,loff_t *f_pos)
{
 //add your own code
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
/********************close the interrupt of ENT1*************************************
 In case the glitch of EINT2 intervene the system.
************************************************************************************/
        disable_irq(IRQ_EINT2);  

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


struct file_operations candev_fops=
{
	read  :can_read,
	write :can_write,
	ioctl  :can_ioctl,
	open :can_open,
	release :can_release,
	fasync :can_fasync,	

};

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

/********************set irq ofr EINT2**********************************************/
   	local_irq_save(flags);
	set_external_irq(IRQ_EINT2, EXT_FALLING_EDGE, GPIO_PULLUP_DIS);
	disable_irq(IRQ_EINT2);
        enable_irq(IRQ_EINT2);	
	local_irq_restore(flags);
	result = request_irq(IRQ_EINT2,&can_interrupt,SA_INTERRUPT,"can",NULL);
	if (result)
	{
	    printk("Can't get assigned irq %d,result=%d\n",IRQ_EINT2,result);
	    return result;
	}

	devfs_register(NULL,INT_DEV_NAME,DEVFS_FL_AUTO_DEVNUM,INT_DEV_MAJOR,
			1,S_IFCHR|S_IRUGO|S_IWUGO,&candev_fops,NULL);	

	//result=register_chrdev(INT_DEV_MAJOR,INT_DEV_NAME,&candev_fops); 
	//if(result<0)printk("insmod failed\n");
        //else printk("insmod successfully %d\n",result);

/********************do address map from phsical address to virtual address*********/
	address_map();	

	//init the mutex semphare
	init_MUTEX(&rx_mutex);
	init_MUTEX(&tx_mutex);
        
	return 0;
}


void cleanup_module(void)
{
	disable_irq(IRQ_EINT2);
	free_irq(IRQ_EINT2, NULL);

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
MODULE_DESCRIPTION("CAN Module");

