#ifndef SPI_H
#define SPI_H 1


#define rSPCON0 (*(volatile unsigned long *)r_SPCON0)   /*SPI control Register*/
#define rSPSTA0 (*(volatile unsigned long *)r_SPSTA0)   /*SPI status Register*/
#define rSPPIN0 (*(volatile unsigned long *)r_SPPIN0)   /* SPI pin controlRegister*/

#define rSPPRE0 (*(volatile unsigned long *)r_SPPRE0)  /*SPI Baud Rate Prescaler Register */
#define rSPTDAT0 (*(volatile unsigned long *)r_SPTDAT0) /*SPI Tx Data Register*/
#define rSPRDAT0 (*(volatile unsigned long *)r_SPRDAT0) /*SPI Rx Data Register*/

#define rGPACON (*(volatile unsigned long *)r_GPACON) /*nGCS2*/
#define rGPADAT (*(volatile unsigned long *)r_GPADAT) /*nGCS2*/

/*I/O registers*/
#define rGPECON (*(volatile unsigned long *)r_GPECON) /*Configure the pins of port E*/
#define rGPEUP  (*(volatile unsigned long *)r_GPEUP)  /*Pull-up disable register for port E*/

#define rGPGCON (*(volatile unsigned long *)r_GPGCON)    /*Configure the pins of port G*/
#define rGPGUP  (*(volatile unsigned long *)r_GPGUP)     /*Pull-up disable register for port G*/
#define rGPGDAT (*(volatile unsigned long *)r_GPGDAT)     /*The data register for port G*/


unsigned long r_SPCON0,r_SPSTA0,r_SPPIN0,r_SPPRE0,r_SPTDAT0,r_SPRDAT0;
unsigned long r_GPECON,r_GPEUP;
unsigned long r_GPACON,r_GPADAT;

unsigned long r_GPGCON,r_GPGUP,r_GPGDAT;


/************************************************************************/
/* 显然，下边才是重头戏，SPI的函数们 by Andriy                          */
/************************************************************************/

//注：此函数在can.c的init_module()中调用
int address_map(void);

//Initialize 2410 spi
//注：在can.c和can_sensor.c的can_open(...)中调用
void Init_SPI(void);

//To polling when SPI transfer is not finished
//注：顾名思义，轮转等待自己的回合，就是给下方的spi_tx_data()中用的，
//    所以其实不一定要写在这个.h里，但是它存在！
void spi_poll_done(void);

//transmit data via SPI
//注：SPI的传数据的函数，给MCP2510调用的
void spi_tx_data(unsigned char data);

#endif
