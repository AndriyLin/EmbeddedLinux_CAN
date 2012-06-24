#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>

#include <asm/io.h>
#include "spi.h"
#include "mcp2510.h"


/************************************************************************/
/* 使编译通过，修改了注释，增加了分区导读和一些拼写错误的更正！ by Andriy   */
/************************************************************************/



//注：modified by lyg / Andriy，原本这个是在mcp2510.h里定义的，可是头文件里咋能定义变量呢！！！
struct semaphore rx_mutex;
struct semaphore tx_mutex;


//enable mcp2510
//use GPG2 as the chip select signal
void enable2510()
{
	rGPGDAT &=(~0x0004);
}

//disable mcp2510
void disable2510()
{
	rGPGDAT |=0x0004;
}




/***************************************************************************/
/* 注：罗YG说接下来6个SPI的函数是实现那几个SPI命令的，MCP2510通过这些和MCU交互 */
/***************************************************************************/

/********************SPI Instruction Layer begin*************************************
Implement the 6 SPI instruction of MCP2510,including READ,WRITE,READ STATUS,BIT MODIFY and RESET.
Via these instructions,MCP2510 interactive with MCU.
************************************************************************************/
//Implement WRITE instruction of mcp2510
void Write_Instr_2510(unsigned char W_ADD, unsigned char W_DATA)
{
	//	int flags;

	//local_irq_save(flags); 
	enable2510();
	udelay(10000);

	spi_tx_data(CMD_WRITE);
	spi_tx_data(W_ADD);
	spi_tx_data(W_DATA);

	disable2510();
	//local_irq_restore(flags);
}

//Implement Request To Send (RTS) Instruction of mcp2510
void RTS_Instr_2510(unsigned char which)
{
	enable2510();
	udelay(10000);

	if(which==0)
		spi_tx_data(CMD_RTS0);
	else if(which==1)
		spi_tx_data(CMD_RTS1);
	else if(which==2)
		spi_tx_data(CMD_RTS2);

	disable2510();	 	
} 

//Implement Read Status Instruction of mcp2510
unsigned char ReadStatus_Instr_2510()
{
	unsigned char buffer;

	enable2510();
	udelay(10000);	

	spi_tx_data(CMD_READS);
	spi_tx_data(0xff);
	buffer = rSPRDAT0;
	disable2510();

	return buffer;
} 

//Implement READ instruction of mcp2510 
unsigned char Read_Instr_2510(unsigned char R_ADD)
{
	unsigned char buffer;
	//	int flags;

	//local_irq_save(flags);
	enable2510();
	udelay(10000);

	spi_tx_data(CMD_READ);
	spi_tx_data(R_ADD);
	spi_tx_data(0xff); //necessary : add by luo 20110401
	buffer = rSPRDAT0;

	disable2510();
	//local_irq_restore(flags);
	return buffer;
}

//Implement Bit Modify Instruction of mcp210
void BitModify_Instr_2510(U8 M_ADD, U8 M_MASK, U8 M_DATA)
{	
	enable2510();
	udelay(10000);

	spi_tx_data(CMD_MODIFY);
	spi_tx_data(M_ADD);
	spi_tx_data(M_MASK);
	spi_tx_data(M_DATA);

	disable2510();
}

//Implement RESET instruction of mcp2510
void Reset_Instr_2510(void)
{
	unsigned char i;

	enable2510();
	udelay(10000);

	spi_tx_data(CMD_RESET);

	disable2510();

	for(i = 0; i < 128; i++); //Wait for OST   
}

/********************SPI Instruction Layer end**************************************/

/***************************************************************************/
/*                   注：这6个SPI的函数实现完毕                             */
/***************************************************************************/







//注：这里原本有注释的，但是和.h里一模一样，为了避免冗余就删掉了
void MCP2510_SetCommPara(int bit_rate,int tmp_sample)
{
	unsigned char tmp;
	unsigned char tmp_sjw=0,tmp_brp=0,tmp_phase1=0,tmp_prop=0,tmp_phase2=0;

	tmp=0;
	tmp_sjw=0x01<<6; //set sjw=01
	tmp_brp=0x04;	 //set brp=4;
	tmp=tmp_sjw+tmp_brp;
	Write_Instr_2510(CNF1,tmp);

	tmp=BTLMODE|SAM;  //set BTLMODE to 1 and SAM to 1
	tmp_phase1=(0x06<<3); //phase segment 1=Tq*(tmp_pase1+1)=7*Tq
	tmp_prop=0x01;//propagation time =Tq*(tmp_prop+1)=2*Tq
	tmp=tmp+tmp_phase1+tmp_prop;
	Write_Instr_2510(CNF2,tmp);	

	tmp=WAKFIL;
	tmp_phase2=0x05; //phase segment 2=Tq*(tmp_phase2+1)=6*Tq
	tmp=tmp+tmp_phase2;
	Write_Instr_2510(CNF3,tmp);	 	
}


//注：这里原本有注释的，但是和.h里一模一样，为了避免冗余就删掉了
void MCP2510_SetConfigMode()
{
	BitModify_Instr_2510(CANCTRL, REQOP_MSK, OP_CONFIG<<5);
}

void MCP2510_SetSleepMode()
{
	BitModify_Instr_2510(CANCTRL, REQOP_MSK, OP_SLEEP<<5); 
}

void MCP2510_SetLoopbackMode()
{
	BitModify_Instr_2510(CANCTRL, REQOP_MSK, OP_LOOPBACK<<5); 
}

void MCP2510_SetListenOnlyMode()
{
	BitModify_Instr_2510(CANCTRL, REQOP_MSK, OP_LISTEN_ONLY<<5); 
}

void MCP2510_SetNormalMode()
{
	BitModify_Instr_2510(CANCTRL, REQOP_MSK, OP_NORMAL<<5); 
}


//注：这里原本有注释的，但是和.h里一模一样，为了避免冗余就删掉了
int Parse_Status(char ret,unsigned care,unsigned wichTXBCNTRL)
{
	if((care==RX0IF)||(care==RX1IF)||(care==TX0IF)||(care==TX1IF)||(care==TX2IF))
	{
		switch(care)
		{ 
		case RX0IF://Receive Buffer 0 Full Interrupt Flag
			if((ret&0x01)>0) return 1;else return 0;
		case RX1IF://Receive Buffer 1 Full Interrupt Flag
			if((ret&0x02)>0) return 1;else return 0;
		case TX0IF://Transmit Buffer 0 Empty Interrupt Flag
			if((ret&0x08)>0) return 1;else return 0;
		case TX1IF://Transmit Buffer 1 Empty Interrupt Flag
			if((ret&0x20)>0) return 1;else return 0;
		case TX2IF://Transmit Buffer 2 Empty Interrupt Flag
			if((ret&0x80)>0) return 1;else return 0;
		default:
			return 0;
		}  
	}else if(care==TXREQ){
		switch(wichTXBCNTRL)
		{
		case TXB0CTRL:
			if((ret&0x04)>0) return 1;else return 0;//1 buffer is pending transmission
		case TXB1CTRL:
			if((ret&0x10)>0) return 1;else return 0;//1 buffer is pending transmission
		case TXB2CTRL:
			if((care&0x40)>0) return 1;else return 0;//1 buffer is pending transmission
		default:
			return 0;
		}
	}else
		return 0;
}


//Initialize mcp2510
//注：初始化过程，重要！
void Init_MCP2510(void)
{
	unsigned char buffer;	

	//set GPG2 as output


	//Reset MCP2510
	Reset_Instr_2510();


	/************************************************************************/
	/* 注：先进入Configuration模式，做一大堆初始化的设置                      */
	/************************************************************************/

	/****************initialize parameter in configuration mode begin***************/	
	//make sure it is in configuration mode
	while(1)
	{
		buffer = Read_Instr_2510(CANSTAT);
		printk("Init_MCP2510 CANSTAT register is %x\n",buffer);
		if((buffer&OP_MSK) == (OP_CONFIG<<5))
		{
			printk("In the Configure Mode\n");
			break;
		}
		//Set to Configure Mode
		MCP2510_SetConfigMode();
		udelay(10000);
	}

	//Set communication parameter
	MCP2510_SetCommPara(100,3);

	//for testing,enable CLOCKOUT PIN
	BitModify_Instr_2510(CANCTRL, CLKEN|CLKPRE_MSK, CLKEN|0x1);

	//set TXRTSCTRL to be digital input mode for pin nTXxRTS.
	//in case the message is sent because of the glitch of nTXxRTS pin.
	BitModify_Instr_2510(TXRTSCTRL, B2RTSM|B1RTSM|B0RTSM, 0);

	//set BFPCTRL to be digital output mode for pin nRX1BF and nRX0BF.
	//so as i can test the signal indicating receive a message.
	BitModify_Instr_2510(BFPCTRL, B1BFS|B0BFS|B1BFE|B0BFE|B1BFM|B0BFM, 0x06);


	//Set Interrupt, all interrupt enabled
	BitModify_Instr_2510(CANINTE, MERRE|WAKIE|ERRIE|TX2IE|TX1IE|TX0IE|RX1IE|RX0IE ,MERRE|WAKIE|ERRIE|TX2IE|TX1IE|TX0IE|RX1IE|RX0IE);

	//clear interrupt	
	Write_Instr_2510(CANINTF, 0x00);


	//set TXB0CTRL
	Write_Instr_2510(TXB0CTRL,0x00); 
	//set TXB1CTRL
	Write_Instr_2510(TXB1CTRL,0x00);
	//set TXB2CTRL
	Write_Instr_2510(TXB2CTRL,0x00);

	//set filter mask to 0, means we don't do filter
	//for RXB0
	//set RX mask 0 for RXB0 	//真的吗？？mcp2510.pdf上4.3说00是用filter，11是没有使用filter
	Write_Instr_2510(RXM0SIDH,0xff);   //00 to ff 
	Write_Instr_2510(RXM0SIDL,0xff);	//00 to ff

	//set RX filter 0,1 for RXB0
	// |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3|
	Write_Instr_2510(RXF0SIDH,0xff);
	// |SID2|SID1|SID0|Reserved|EXIDE|Reserved|EID17|EID16|
	//EXIDE=1 only extended frame;0:only standard frame.
	Write_Instr_2510(RXF0SIDL,0xe0);

	Write_Instr_2510(RXF1SIDH,0xff);
	Write_Instr_2510(RXF1SIDL,0xe0);


	//for RXB1 说得对啊
	//set RX mask 1 for RXB1 
	Write_Instr_2510(RXM1SIDH,0xff); 	//00 to ff
	Write_Instr_2510(RXM1SIDL,0x00);    

	//set RX filter 2,3,4,5 for RXB1 	  
	Write_Instr_2510(RXF2SIDH,0xff);	//ff 代表都有，改成01等其他之后应该是代表接受特定地址来的东西
	Write_Instr_2510(RXF2SIDL,0xe0);

	Write_Instr_2510(RXF3SIDH,0xff);
	Write_Instr_2510(RXF3SIDL,0xe0);

	Write_Instr_2510(RXF4SIDH,0xff);
	Write_Instr_2510(RXF4SIDL,0xe0);

	Write_Instr_2510(RXF5SIDH,0xff);
	Write_Instr_2510(RXF5SIDL,0xe0);
/*
	//for RXB1 说得对啊
	//set RX mask 1 for RXB1 
	Write_Instr_2510(RXM1SIDH,0x00);     
	Write_Instr_2510(RXM1SIDL,0x00);    

	//set RX filter 2,3,4,5 for RXB1 	  
	Write_Instr_2510(RXF2SIDH,0xff);
	Write_Instr_2510(RXF2SIDL,0xe0);

	Write_Instr_2510(RXF3SIDH,0xff);
	Write_Instr_2510(RXF3SIDL,0xe0);

	Write_Instr_2510(RXF4SIDH,0xff);
	Write_Instr_2510(RXF4SIDL,0xe0);

	Write_Instr_2510(RXF5SIDH,0xff);
	Write_Instr_2510(RXF5SIDL,0xe0);
*/

	//Set RXB0CTRL Register
	//bit 6-5: RXM<1:0>: Receive Buffer Operating Mode
	/*
		11 = Turn mask/filters off; receive any message
		10 = Receive only valid messages with extended identifiers that meet filter criteria
		01 = Receive only valid messages with standard identifiers that meet filter criteria
		00 = Receive all valid messages using either standard or extended identifiers that meet filter criteria
	*/
	BitModify_Instr_2510(RXB0CTRL, RXM0_MSK|RX0RTR|RX0BUKT | RX0FILHIT0, 0X20|RX0RTR|RX0BUKT | RX0FILHIT0);
	//Set RXB1CTRL Register
	BitModify_Instr_2510(RXB1CTRL, RXM1_MSK|RX1RTR | RX1FILHIT_MSK, 0X20|RX1RTR | RX1FILHIT_MSK);

	/****************initialize parameter in configuration mode end*****************/

	/************************************************************************/
	/* 注：Configuration模式内的一大堆初始化的设置结束                        */
	/************************************************************************/




	//Set to Normal Mode
	MCP2510_SetNormalMode();

	//TODO 注：要测试，改这里
	//for testing,set it to Loopback mode
	//MCP2510_SetLoopbackMode(); 

	//Wait into Normal Mode （注：如果是在测试，则为等待进入Loopback模式）
	while(1)
	{
		buffer = Read_Instr_2510(CANSTAT);
//		printk("Into Loopback ,CANSTAT register is %x\n",buffer);

		//TODO
		//     if((buffer&OP_MSK) == (OP_LOOPBACK<<5))
		if((buffer&OP_MSK) == (OP_NORMAL<<5))
		{
			printk("In the NORMAL Mode\n");
			//    printk("In the Loopback Mode\n");
			break;
		}
	}

	printk("Init MCP2510 OK!\n");
}


//注：这个果然就是发送消息的函数了！！！原来这里还有几行注释的，和.h里一样，就不冗余了
int can_data_send(int j,int k)
{
	unsigned char length,i;
	unsigned char send_rt;
	PCanData p=NULL;

	if(j<0 || j>9 || k<0 || k>2)
		return -1;

	p=TXbuffer.TXdata+j;

	length = p->dlc;
	//length=5;//for testing

	printk("in can_data_send:length=%d\n",length);
	if(length>8) length=8;

	Write_Instr_2510(TXB0SIDH+k*0x10,((p->id)&0x03ff)>>3);
	Write_Instr_2510(TXB0SIDL+k*0x10,((p->id)&0x07)>>5);
	Write_Instr_2510(TXB0DLC+k*0x10,length);
	if(p->rxRTR==1)
		BitModify_Instr_2510(TXB0DLC+k*0x10,0x40, 0x40);

	/*	Write_2510(TXB0D0, TXdata[3]);
	Write_2510(TXB0D1, TXdata[4]);
	Write_2510(TXB0D2, TXdata[5]);
	Write_2510(TXB0D3, TXdata[6]);
	Write_2510(TXB0D4, TXdata[7]);
	Write_2510(TXB0D5, TXdata[8]);
	Write_2510(TXB0D6, TXdata[9]);
	Write_2510(TXB0D7, TXdata[10]);
	*/	
	enable2510();	//Select the chip
	udelay(100000);

	spi_tx_data(CMD_WRITE);
	spi_tx_data(TXB0D0+k*0x10);
	for(i = 0; i<length; i++)
	{
		spi_tx_data(p->data[i]);
		mdelay(10);
	}  

	mdelay(1000);//sleep one second to let the signal going	
	disable2510(); 		//Unselect the chip

	//DEBUG: read the txb0 to 
	//get the DLC of txb0
	printk("data length is %d",0x0f&(Read_Instr_2510(TXB0DLC)));
	if((0x40&Read_Instr_2510(TXB0DLC))>0) 
		printk("this is a remote transmit request\n");

	for(i=0;i<8;i++)
	{
		printk("txb0 data buffer %d=%d\n",i,Read_Instr_2510(TXB0D0+i));
	}
	if((Read_Instr_2510(TXB0SIDL)&0x08)>0)
		printk("This is a extend frame\n");
	else
		printk("This is a standard frame\n");

	//SEND txb0
	RTS_Instr_2510(k);

	mdelay(1000);

	//check the result
	send_rt=ReadStatus_Instr_2510();
	if(Parse_Status(send_rt,TXREQ,TXB0CTRL+k*0x10)>0) //still pending transmission
		return -1; 
	else return 1; //send OK.
}


//注：这个果然就是接收数据的函数了，同上，把原本在这里的冗余的注释删掉了，见.h里的函数声明
int can_data_receive(int which)
{	
	unsigned int length,i,tmp=0;
	PCanData p=NULL;


	if(RXbuffer.count<RXBUFLEN) RXbuffer.count++;
	else return -1;

	if(which==0)
		tmp = Read_Instr_2510(RXB0DLC);
	if(which==1)
		tmp = Read_Instr_2510(RXB1DLC);

	printk("in can_data_receive:tmp=%x\n",tmp);

	p=RXbuffer.RXdata+(RXbuffer.head+RXbuffer.count-1)%RXBUFLEN;
	p->dlc=(tmp&0x0f);
	length=p->dlc;


	printk("in can_data_receive:length=%x\n",length);
	if(length>8) length=8;

	if((tmp&0x40)>0)
		p->rxRTR=1;
	else
		p->rxRTR=0;

	enable2510();	//Select the chip
	udelay(100000);

	spi_tx_data(CMD_READ);
	if(which==0)
		spi_tx_data(RXB0D0);
	if(which==1)
		spi_tx_data(RXB1D0);

	for(i = 0; i<length; i++)
	{
		//according to page22-7 of s3c2410 data-sheet, in normal mode if only want to receive data
		//and you should send dummy 0xFF data.
		spi_tx_data(0xff);
		p->data[i] = rSPRDAT0;
		printk("SPI receiving Rxdata[%d] is %x\n",i,p->data[i]);
	}  	
	disable2510(); 		//Unselect the chip	

	return 1;
}


//注：这个应该是罗YG以前写的用于测试的函数，通过“设置Loopback模式居然还是直接调用BitModify”就可以看出来
//再注：所以我觉得我们可以直接改这里
void Test_can_bus(void)
{
	unsigned char buffer;
	//int i;


	//Set Loop Mode
	BitModify_Instr_2510(CANCTRL,0xe0,0x40);

	//Read the Mode
	buffer = Read_Instr_2510(CANINTF);
	if((buffer&0x01)==0x01)printk("The CANINTF register is %x\n",buffer);
	else printk("The CANINTF register is %x\n",buffer);

	//Transmit the sample data	
	printk("Transmit the sample data\n");

	//	for(i = 0; i < 8; i++)printk("Rxdata[%d] is %c\n",i,RXdata[i]);
	/*
	send_test_frame();



	//Receive the sample data	
	while(1)
	{
	if(flag == 0xff)
	{
	for(i = 0; i < 8; i++)
	printk("Rxdata[%d] is %c\n",i,RXdata[i]);
	flag = 0x88;
	break;
	}
	}


	if(flag == 0xff)
	{
	for(i = 0; i < 8; i++)
	printk("No sending data but receive Rxdata[%d] is %c\n",i,RXdata[i]);
	flag =0x88;
	}


	send_test_frame1();
	//Receive the sample data	
	while(1)
	{
	if(flag == 0xff)
	{
	for(i = 0; i < 8; i++)
	printk("Rxdata[%d] is %c\n",i,RXdata[i]);
	flag = 0x88;
	break;
	}
	}
	*/	
}
