#include "spi.h"

//IO-remap address function of S3c2410
int address_map(void)
{
	//SPI registers
	r_SPCON0 = ioremap(0x59000000,4);
	r_SPSTA0 = ioremap(0x59000004,4);
	r_SPPIN0 = ioremap(0x59000008,4);
	r_SPPRE0 =  ioremap(0x5900000C,4);
	r_SPTDAT0 = ioremap(0x59000010,4);
	r_SPRDAT0 = ioremap(0x59000014,4);


	//I/O registers
	r_GPACON=ioremap(0x56000000,4);
	r_GPADAT=ioremap(0x56000004,4);	

	r_GPECON = ioremap(0x56000040,4);
	r_GPEUP  = ioremap(0x56000048,4);		

	r_GPGCON = ioremap(0x56000060,4);
	r_GPGUP = ioremap(0x56000068,4);
	r_GPGDAT = ioremap(0x56000064,4);

	return 0;
}

//Initialize 2410 spi
void Init_SPI(void)
{
	int i;

	rSPPRE0 = 0xff;  //2410 SPI_BAUD; 

	//0011000
	/****************rSPCON0 0x59000000**********************************************
	|SMOD[1]|SMOD[0]|SCKEN|MASTER_OR_SLAVE|CLOCK_PORITY|CLOCK_PHASE_SELECT|Tx_Auto_-
	Garbage_Data_mode_enable|
	SMOD[1...0]=00,POLLING MODE.
	SMOD[1...0]=01,INTERRUPT MODE.
	SMOD[1...0]=10,POLLING MODE.
	SMOD[1...0]=11,reserved.
	MASTER_OR_SLAVE=1:Master,0:Slave.
	********************************************************************************/
	rSPCON0 = 0x18;  //polling mode is used here!
	for(i = 0 ; i < 10 ; i++)
	{
		rSPTDAT0 = 0xff;
	}

	rGPECON |=   0x0a800000;
	rGPECON &= (~0x05400000);
	rGPEUP |= 0x3800;

	//GPG2----->CS
	rGPGCON |=   0x00000010;
	rGPGCON &= (~0x00000020);
	rGPGUP &= (~0x0004);
	rGPGDAT |=0x0004;                //Unselect the chip
}

//To polling when SPI transfer is not finished
void spi_poll_done(void)
{
	int nCount=0;

	while(!(rSPSTA0 & 0x01) )
	{
		nCount++;
		if(nCount>=5000)
		{
			printk("SPI state poll failed\n");
			break;
		}
	}	
}

//transmit data via SPI
void spi_tx_data(unsigned char data)
{	 
	spi_poll_done();
	rSPTDAT0 = data;	//transmit data
	spi_poll_done();	
}
