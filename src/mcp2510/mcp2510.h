#ifndef MCP2510_H
#define MCP2510_H 1

/********************mcp2510 head file***********************************************
 This file defines mcp2510 chip info,including reigisters,buffers,and commands.
************************************************************************************/

//Mcp2510 register address
#define U8 unsigned char
#define U32 unsigned int

/********************MCP2510 SPI  commands defines begin*****************************
 All of these commands are sent to MCP2510 via SPI interface.
 All of these commands are executed in MCP2510.
 All of these commands valid only when the nCS of MCP2510 are low level in whole exe-
 cutive time.
************************************************************************************/

/********************write command***************************************************
 Write information into MCP2510 register,and address.
 Formate is CMD_WRITE,address(one byte length),data(one byte length).
 Where the address is one in MCP2510 address map.
************************************************************************************/
#define CMD_WRITE	0x02    //00000010

/********************read command****************************************************
 Read information from MCP2510 register,or address.
 Formate is CMD_READ,address(one byte length).
 The command is sent on SI pin into MCP2510,and data will return on the next Tscl of 
 SO of MCP2510.
************************************************************************************/
#define CMD_READ	0x03    //00000011

/********************reset command***************************************************
 Reset the MCP2510.The effect is same as lowe level of nreset pin.	 
************************************************************************************/
#define CMD_RESET	0xc0    //11000000

/********************read status command*********************************************
 Read status bit of receive and transmition.
 Format is CMD_READS.
 The value(one byte length) of register will appear on SO pin after the command is c-
 ompleting sending on SI pin.
 Once the SCK and nCS of MCP2510 is also valid after the first appearance,the value wil-
 l repeat again.
************************************************************************************/
#define CMD_READS	0xa0    //10100000

/********************bit modify command**********************************************
 Modify some bit(s) of control or status registers.
 Format is CMD_BIT_MODIFY,address(of register),mask byte,data byte.
 Where mask byte tells which bit(s) will modified and data byte tells that the bit(s) 
 will be changed into what value.
************************************************************************************/
#define CMD_MODIFY	0x05    //00000101

/********************request to send TX buffer0**************************************
 Send message in Tx buffer0 out.
************************************************************************************/
#define CMD_RTS0	0x81    //10000001

/********************request to send TX buffer1**************************************
 Send message in Tx buffer1 out.
************************************************************************************/
#define CMD_RTS1	0x82    //10000010

/********************request to send TX buffer2**************************************
 Send message in Tx buffer2 out.
************************************************************************************/
#define CMD_RTS2	0x84    //10000100

/********************MCP2510 SPI  commands defines end******************************/


/********************CAN register address and format define begin**************************************/
/********************CAN status register*********************************************
 |OPMOD2|OPMOD1|OPMOD0|RESERVED|ICOD2|ICOD1|ICOD0|RESERVED|
 OPMOD[2...0]=000,means Normal Operate Mode.
 OPMOD[2...0]=001,means Sleep Mode.
 OPMOD[2...0]=010,means Loopback Mode.
 OPMOD[2...0]=011,means Listen Only Mode.
 OPMOD[2...0]=100,means Configuration Mode.
 In loopback mode,the MCP2510 can receive message from itself,and the filter and mask
 works,buf the message will not sent to bus.No error flags and acknoledge signals.

 ICOD[2...0]=000,means no Interrupt.
 ICOD[2...0]=001,means Error Interrupt.
 ICOD[2...0]=010,means WakeUp Interrupt.
 ICOD[2...0]=011,means TXB0 Interrupt.
 ICOD[2...0]=100,means TXB1 Interrupt.
 ICOD[2...0]=101,means TXB2 Interrupt.
 ICOD[2...0]=110,means RXB0 Interrupt.
 ICOD[2...0]=111,means RXB1 Interrupt. 
************************************************************************************/
#define CANSTAT		0x0E     //CAN Status Register
#define OP_MSK		0xE0     //
#define OP_NORMAL   0x00     //Normal operate mode
#define OP_SLEEP    0x01     //Sleep mode
#define OP_LOOPBACK  0x02    //Loopback mode
#define OP_LISTEN_ONLY 0x03  //Listen Only mode
#define OP_CONFIG    0x04    //Configuration mode
#define ICOD_MSK    0x0E
#define ICOD_NO     0x00  //no interrupt
#define ICOD_ERR    0x01  //Error interrupt
#define ICOD_WAKEUP 0x02  //wakeup interrupt
#define ICOD_TXB0   0x03  //TXB0 interrupt
#define ICOD_TXB1   0x04  //TXB1 interrupt
#define ICOD_TXB2   0x05  //TXB2 interrupt
#define ICOD_RXB0   0x06  //RXB0 interrupt
#define ICOD_RXB1   0x07  //RXB1 interrupt

/********************CAN Control Register********************************************
 |REQOP2|REQOP1|REQOP0|ABAT|Reserved|CLKEN|CLKPRE1|CLKPRE0|
 REQOP[2...0]=000 means Set Normal Operation Mode.
 REQOP[2...0]=001 means Set Sleep Mode.
 REQOP[2...0]=010 means Set Loopback Mode.
 REQOP[2...0]=011 means Set Listenonly Mode.
 REQOP[2...0]=100 means Set Configuration Mode.
 
 ABAT=1 means request abort all pending transmit buffers.
 ABAT=0 means terminate request to abort all transmissions.

 CLKEN=0 means disable CLOCKOUT PIN(in hign impdence).
 CLKEN=1 means enable CLOCKOUT PIN.

 CLKPRE[1...0] used for CLOCKOUT PIN prescale.
 CLKPRE[1...0]=00,freqence of CLOCKOUT PIN is sysclk/1.
 CLKPRE[1...0]=01,freqence of CLOCKOUT PIN is sysclk/2.
 CLKPRE[1...0]=10,freqence of CLOCKOUT PIN is sysclk/4.
 CLKPRE[1...0]=11,freqence of CLOCKOUT PIN is sysclk/8.	 
************************************************************************************/
#define CANCTRL		0x0F     //CAN Control Register
#define REQOP_MSK   0xE0
#define ABAT        0x10
#define CLKEN       0x04
#define CLKPRE_MSK  0x03

/********************RXnBUF PIN control and status***********************************
 |RESERVED|RESERVED|B1BFS|B0BFS|B1BFE|B0BFE|B1BFM|B0BFM|
 B1BFS is effective only B1BFE=1 and B1BFM=0
 B0BFS is effective only B0BFE=1 and B0BFM=0
 B1BFS=1,means valid message loaded into receive buffer 1.	
 B0BFS=1,means valid message loaded into receive buffer 0.
								
 B1BFE=1,means enable nRX1BF pin.
 B1BFE=0,means disable nRX1BF pin.
 B0BFE=1,means enable nRX0BF pin.
 B0BFE=0,means disable nRX0BF pin.

 B1BFM=1,means nRX1BF pin used for interrupt when valid message loaded into RX1 buffer.
 B1BFM=0,means nRX1BF pin used for digital output.
 B0BFM=1,means nRX0BF pin used for interrupt when valid message loaded into RX0 buffer.
 B0BFM=0,means nRX0BF pin used for digital output.
 
 When enabled and in digital output mode,the BnNFS and nRXnBF are same state.  
************************************************************************************/
#define BFPCTRL		0X0C     
#define B1BFS       0x20
#define B0BFS       0x10
#define B1BFE       0x08
#define B0BFE       0x04
#define B1BFM       0x02
#define B0BFM       0x01

/********************TXRTSCTRL regiter***********************************************
 |RESERVED|RESERVED|B2RTS|B1RTS|B0RTS|B2RTSM|B1RTSM|B0RTSM|
 BnRTS only valid when BnRTSM=0,when MCP2510 is in digital input mode.
 B2RTS=1 means request send message in TX buffer 2.
 B1RTS=1 means request send message in TX buffer 1.
 B0RTS=1 means request send message in TX buffer 0.

 B2RTSM=1 means nTX2RTS pin is for send request of transmit buffer2.
 B1RTSM=1 means nTX1RTS pin is for send request of transmit buffer1.
 B0RTSM=1 means nTX0RTS pin is for send request of transmit buffer0.

 BxRTSM=0 means nTXxRTS pin is in digital input mode.

 Only BnRTSM can be write ,and BnRTS can be read only.
************************************************************************************/
#define TXRTSCTRL	0X0D     
#define B2RTS 0x20
#define B1RTS 0x10
#define B0RTS 0x08
#define B2RTSM 0x04
#define B1RTSM 0x02
#define B0RTSM 0x01

/********************Configuration Register 1****************************************
 |SJW1|SJW0|BRP5|BRP4|BRP3|BRP2|BRP1|BRP0|
 SJW is Synchronization Jump Width.
 real SJW=Tq*(SJW[1...0]+1).

 BRP is Baud Rate Prescal.
 real BRP=BRP[5...0]+1,which means Tq=2*(real BRP)*1/freqence of OSC.
************************************************************************************/
#define CNF1		0x2A   
#define SJW_MSK     0xC0
#define BRP_MSK     0x3F  

/********************Configuration Register 2****************************************
 |BTLMODE|SAM|PHSEG12|PHSEG11|PHSEG10|PRSEG2|PRSEG1|PRSEG0|
 BTLMODE is for phase segment 2 bit time length.
 BTLMODE=1 means length of phase segment 2 is determinted by PHSEG22 to PHSEG20 of C-
 NF3.
 BTLMODE=0 means legth of phase segment 2 is greater of PHSEG1 and Information Proce-
 ssing Time(2Tq in MCP2510).
 SAM=1 means sample three times,and SAM=0 means sample once.
 PHSEG1[2...0] is phase segment 1 length.
 Real phase segment 1 length=Tq*(PHSEG1[2...0]+1).
 PRSEG[2...0] is Propagation Segment Length.
 Real Propagation Segment Length=Tq*(PRSEG[2...0]+1).
************************************************************************************/
#define CNF2		0x29   
#define BTLMODE     0x80
#define SAM         0x40
#define PHSEG_MSK   0x38
#define PRSEG_MSK   0x07  

/************************************************************************************
 |RESERVED|WAKFIL|RESERVED|RESERVED|RESERVED|PHSEG22|PHSEG21|PHSEG20|
 WAKFIL means for wake-up filter.
 WAKFIL=0 Wake-up filter disabled,and WAKFIL=1 wake-up filter enabled.
 PHSEG2[2...0] is phase segment 2 length.
 Real phase segment 2 length=Tq*(PHSEG2[2...0]+1).
************************************************************************************/
#define CNF3		0x28
#define WAKFIL 0x40
#define PHSEG2_MSK 0x07

/********************CAN Interrup Enable Register************************************
 |MERRE|WAKIE|ERRIE|TX2IE|TX1IE|TX0IE|RX1IE|RX0IE|
 MERRE=1,means interrupt on error during message reception and transmission.
 WAKIE=1,means interrup on BUS activity(Wakeup).
 ERRIE=1,means EFLG error condition changed.
 TX2IE=1,means interrupt on TX2 becoming empty.
 TX1IE=1,means interrupt on TX1 becoming empty.
 TX0IE=1,means interrupt on TX2 becoming empty.

 RX1IE=1,means interrupt on messaged received in RX1 buffer.
 RX0IE=1,means interrupt on messaged received in RX0 buffer.
************************************************************************************/
#define CANINTE		0x2B      //CAN Interput Enable Register
#define MERRE 0X80
#define WAKIE 0X40
#define ERRIE 0X20
#define TX2IE 0X10
#define TX1IE 0X08
#define TX0IE 0X04
#define RX1IE 0X02
#define RX0IE 0X01

/********************Interrupt flag**************************************************
 |MERRF|WAKIF|ERRIF|TX2IF|TX1IF|TX0IF|RX1IF|RX0IF|
 MERRF=1,means interrupt flag on error during message reception and transmission.
 WAKIF=1,means interrup flag on BUS activity(Wakeup).
 ERRIF=1,means flag of EFLG error condition changed.
 TX2IF=1,means interrupt flag on TX2 becoming empty.
 TX1IF=1,means interrupt flag on TX1 becoming empty.
 TX0IF=1,means interrupt flag on TX2 becoming empty.

 RX1IF=1,means interrupt flag on messaged received in RX1 buffer.
 RX0IF=1,means interrupt flag on messaged received in RX0 buffer.
************************************************************************************/
#define CANINTF		0x2C
#define MERRF 0X80
#define WAKIF 0X40
#define ERRIF 0X20
#define TX2IF 0X10
#define TX1IF 0X08
#define TX0IF 0X04
#define RX1IF 0X02
#define RX0IF 0X01

/********************Error Flag Register*********************************************
 |RX1OVR|RX0OVR|TXBO|TXEP|RXEP|TXWAR|RXWAR|EWARN|
 RX1OVR=1,means a valid message received for RXB1 while CANINTF.RX1IF=1(RX1 buffer is
  full). Must reset by MCU.
 RX0OVR=1,means a valid message received for RXB0 while CANINTF.RX0IF=1(RX0 buffer is
  full). Must reset by MCU.
 TXBO,means buss-off error flag.Set when TEC>=255. Reset after a bus recovery sequ-
 ence.
 TXEP,transmit error-passive flag.Set when TEC>=128.Reset when TEC<128;
 RXEP,receive error-passive flag.Set when REC>=128.Reset when REC<128.
 TXWAR,transmit error warning flag.Set when TEC>=96.Reset when TEC<96;
 RXWAR,receive error warning flag.Set when REC>=96.Reset when REC<96.
 EWARN,error warning flag.Set if TEC>=96 and REC>=96.Reset when TEC<96 and REC<96.  
************************************************************************************/
#define EFLG		0x2D
#define RX1OVR 0x80
#define RX0OVR 0x40
#define TXBO 0x20
#define TXEP 0x10
#define RXEP 0x08
#define TXWAR 0x04
#define RXWAR 0x02
#define EWARN 0x01

/********************Transmit Error Counter Register********************************/
#define TEC		0x1C

/********************Receive Error Counter Register*********************************/
#define REC		0x1D

/********************Transmit Buffer 0 Control Register******************************
 |Reserved|ABTF|MLOA|TXERR|TXREQ|Reserved|TXP1|TXP0|
 ABTF:message aborted flag 1:message transmit was aborted;0:message completed transm-
 ission successfully.
 MLOA:message lost arbitration.1:message lost arbitration while being sent;0:message 
 not lost arbitration while being sent.
 TXERR:transmit error detected.1:bus error occurred while being transmittin;0:no bus 
 error occurred while being transmittin.
 TXREQ:message transmit request.1:buffer currently pending transmission(locked);0:buf-
 fer is not locked.When transmit ok,or abort ,this bit will be set 0.
 TXP[1...0]:transmit buffer priority.
 TXP[1...0]=00,means lowest message priority.
 TXP[1...0]=01,means low immediate message priority.
 TXP[1...0]=10,means high immediate message priority.
 TXP[1...0]=11,means highest message priority.	  
************************************************************************************/
#define TXB0CTRL	0x30     // Transmit Buffer 0 Control Register
#define ABTF 0x40
#define MLOA 0x20
#define TXERR 0x10
#define TXREQ 0x08
#define TXP_MSK 0x03

#define TXB1CTRL	0x40     // Transmit Buffer 1 Control Register

#define TXB2CTRL	0X50     //Transmit Buffer 2 Control Register

/********************Receive Buffer 0 Control Register******************************
 |Reserved|RXM1|RXM0|Reserved|RXRTR|BUKT|BUKT1|FILHIT0|
 RXM[1...0]:receive buffer operate mode
 RXM[1...0]=00:receive all valid messages using either standard or extended identifi-
 rs that meet their criteria.
 RXM[1...0]=01:receive valid messages using only standard identifiers that meet crit-
 eria.
 RXM[1...0]=10:receive valid messages using only extended identifiers that meet crit-
 eria.
 RXM[1...0]=11:turn mask/filter off,receive any message.

 RXRTR:receive remote transfer request.1:received remote transter request;0:No recei-
 ved remote transfer request.

 BUKT:rollover enable.1:RXB0 message will be rollover and be written into RXB1 if R-
 XB0 is full;0:rollover disabled.
 BUKT1:only used internally in MCP2510.

 FILHIT0:filter hit,indicates which acceptance filter enabled reception of message.
 1:acceptance filter 1;0:acceptance filter 0;
 If a message rollover from RXB0 to RXB1,then FILHIT0 reflects the filter(RXF0 RX fi-
 lter 0) that accepted the message that rolled over.
***********************************************************************************/
#define RXB0CTRL	0X60     //Receive Buffer 0 Control Register
#define RXM0_MSK 0x60
#define RX0RTR 0x08
#define RX0BUKT  0x04
#define RX0FILHIT0 0x01

/********************Receive Buffer 1 Control Register*******************************
 |Reserved|RXM1|RXM0|Reserved|RXRTR|FILHIT2|FILHIT1|FILHIT0|
 RXM[1...0]:receive buffer operate mode
 RXM[1...0]=00:receive all valid messages using either standard or extended identifi-
 rs that meet their criteria.
 RXM[1...0]=01:receive valid messages using only standard identifiers that meet crit-
 eria.
 RXM[1...0]=10:receive valid messages using only extended identifiers that meet crit-
 eria.
 RXM[1...0]=11:turn mask/filer off,receive any message.

 RXRTR:receive remote transfer request.1:received remote transter request;0:No recei-
 ved remote transfer request.

 FILHIT[2...0]:filter hit,indicates which acceptance filter enabled reception of message.
 FILHIT[2...0]=101:acceptance filter 5.
 FILHIT[2...0]=100:acceptance filter 4.
 FILHIT[2...0]=011:acceptance filter 3.
 FILHIT[2...0]=010:acceptance filter 2.
 FILHIT[2...0]=001:acceptance filter 1 only if BUKT set in RXB0CTRL.
 FILHIT[2...0]=000:acceptance filter 0 only if BUKT set in RXB0CTRL.
************************************************************************************/
#define RXB1CTRL	0X70     //Receive Buffer 1 Control Register
#define RXM1_MSK 0x60
#define RX1RTR 0x08
#define RX1FILHIT_MSK 0x07


/********************Acceptance Filter Mask Standard Identifier High for RXB0********
 |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3|
 The mask bits for <10:3> of standard frame identifiers of receive buffer 0.
************************************************************************************/
#define RXM0SIDH	0X20  
/********************Acceptance Filter Mask Standard Identifier Low for RXB0********
 |SID2|SID1|SID0|Reserved|Reserved|Reserved|EID17|EID16|
 The mask bits for <2:0> of standard frame identifiers of receive buffer 0.
 EID[17...16] is the mask for extended identifier portion.
************************************************************************************/
#define RXM0SIDL	0X21


/********************Acceptance Filter Mask Standard Identifier High for RBX1*******
 |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3|
 The mask bits for <10:3> of standard frame identifiers of receive buffer 1.
************************************************************************************/
#define RXM1SIDH	0X24	 
/********************Acceptance Filter Mask Standard Identifier Low for RXB1********
 |SID2|SID1|SID0|Reserved|Reserved|Reserved|EID17|EID16|
 The mask bits for <2:0> of standard frame identifiers of receive buffer 1.
 EID[17...16] is the mask for extended identifier portion.
************************************************************************************/
#define RXM1SIDL	0X25


/********************Acceptance Filter Mask Extended Identifier High for RXB0***************
 |EID15|EID14|EID13|EID12|EID11|EID10|EID9|EID8|
 Extended identifier mask for <15:8> of extended identified of RXB0.
************************************************************************************/
#define RXM0EID8	0X22  
/********************Acceptance Filter Mask Extended Identifier Low for RXB 0***************
 |EID7|EID6|EID5|EID4|EID3|EID2|EID1|EID0|
 Extended identifier mask for <7:0> of extended identified of RXB0.
************************************************************************************/
#define RXM0EID0	0X23


/********************Acceptance Filter Mask Extended Identifier High for RXB1***************
 |EID15|EID14|EID13|EID12|EID11|EID10|EID9|EID8|
 Extended identifier mask for <15:8> of extended identifier of RXB1.
************************************************************************************/
#define RXM1EID8	0X26	  
/********************Acceptance Filter Mask Extended Identifier Low for RXB 1***************
 |EID7|EID6|EID5|EID4|EID3|EID2|EID1|EID0|
 Extended identifier mask for <7:0> of extended identified of RXB1.
************************************************************************************/
#define RXM1EID0	0X27	  


/********************Acceptance Filter 0 Standard Identifier High********************
 |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3|	   
************************************************************************************/
#define RXF0SIDH	        0X00
/********************Acceptance Filter 0 Standard Identifier Low********************
 |SID2|SID1|SID0|Reserved|EXIDE|Reserved|EID17|EID16|	   
************************************************************************************/
#define RXF0SIDL		0X01


/********************Acceptance Filter 0 Extended Identifier High********************
 |EID15|EID14|EID13|EID12|EID11|EID10|EID9|EID8|	   
************************************************************************************/
#define RXF0EID8		0X02
/********************Acceptance Filter 0 Extended Identifier Low********************
 |EID7|EID6|EID5|EID4|EID3|EID2|EID1|EID0|	   
************************************************************************************/
#define RXF0EID0		0X03


/********************Acceptance Filter 1 Standard Identifier High********************
 |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3|	   
************************************************************************************/
#define RXF1SIDH		0X04
/********************Acceptance Filter 1 Standard Identifier Low ********************
 |SID2|SID1|SID0|Reserved|EXIDE|Reserved|EID17|EID16|	   
************************************************************************************/
#define RXF1SIDL		0X05


/********************Acceptance Filter 1 Extended Identifier High********************
 |EID15|EID14|EID13|EID12|EID11|EID10|EID9|EID8|	   
************************************************************************************/
#define RXF1EID8		0X06
/********************Acceptance Filter 1 Extended Identifier Low********************
 |EID7|EID6|EID5|EID4|EID3|EID2|EID1|EID0|	   
************************************************************************************/
#define RXF1EID0		0X07


/********************Acceptance Filter 2 Standard Identifier High********************
 |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3|	   
************************************************************************************/
#define RXF2SIDH		0X08
/********************Acceptance Filter 2 Standard Identifier Low********************
 |SID2|SID1|SID0|Reserved|EXIDE|Reserved|EID17|EID16|	   
************************************************************************************/
#define RXF2SIDL		0X09


/********************Acceptance Filter 2 Extended Identifier High********************
 |EID15|EID14|EID13|EID12|EID11|EID10|EID9|EID8|	   
************************************************************************************/
#define RXF2EID8		0X0A
/********************Acceptance Filter 2 Extended Identifier Low********************
 |EID7|EID6|EID5|EID4|EID3|EID2|EID1|EID0|	   
************************************************************************************/
#define RXF2EID0		0X0B


/********************Acceptance Filter 3 Standard Identifier High********************
 |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3|	   
************************************************************************************/
#define RXF3SIDH		0X10
/********************Acceptance Filter 3 Standard Identifier Low********************
 |SID2|SID1|SID0|Reserved|EXIDE|Reserved|EID17|EID16|	   
************************************************************************************/
#define RXF3SIDL		0X11


/********************Acceptance Filter 3 Extended Identifier High********************
 |EID15|EID14|EID13|EID12|EID11|EID10|EID9|EID8|	   
************************************************************************************/
#define RXF3EID8		0X12
/********************Acceptance Filter 3 Extended Identifier Low********************
 |EID7|EID6|EID5|EID4|EID3|EID2|EID1|EID0|	   
************************************************************************************/
#define RXF3EID0		0X13


/********************Acceptance Filter 4 Standard Identifier High********************
 |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3|	   
************************************************************************************/
#define RXF4SIDH		0X14
/********************Acceptance Filter 4 Standard Identifier Low********************
 |SID2|SID1|SID0|Reserved|EXIDE|Reserved|EID17|EID16|	   
************************************************************************************/
#define RXF4SIDL		0X15


/********************Acceptance Filter 4 Extended Identifier High********************
 |EID15|EID14|EID13|EID12|EID11|EID10|EID9|EID8|	   
************************************************************************************/
#define RXF4EID8		0X16
/********************Acceptance Filter 4 Extended Identifier Low********************
 |EID7|EID6|EID5|EID4|EID3|EID2|EID1|EID0|	   
************************************************************************************/
#define RXF4EID0		0X17


/********************Acceptance Filter 5 Standard Identifier High********************
 |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3|	   
************************************************************************************/
#define RXF5SIDH		0X18
/********************Acceptance Filter 5 Standard Identifier Low********************
 |SID2|SID1|SID0|Reserved|EXIDE|Reserved|EID17|EID16|	   
************************************************************************************/
#define RXF5SIDL		0X19


/********************Acceptance Filter 5 Extended Identifier High********************
 |EID15|EID14|EID13|EID12|EID11|EID10|EID9|EID8|	   
************************************************************************************/
#define RXF5EID8		0X1A
/********************Acceptance Filter 5 Extended Identifier Low********************
 |EID7|EID6|EID5|EID4|EID3|EID2|EID1|EID0|	   
************************************************************************************/
#define RXF5EID0		0X1B



/********************Transmit Buffer 0 Standard Identifier High**********************
 |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3| 
************************************************************************************/
#define TXB0SIDH		0X31
/********************Transmit Buffer 0 Standard Identifier Low**********************
 |SID2|SID1|SID0|Reserved|EXIDE|Reserved|EID17|EID16| 
************************************************************************************/
#define TXB0SIDL		0X32
/********************Transmit Buffer 0 Extended Identifier High**********************
 |EID15|EID14|EID13|EID12|EID11|EID10|EID9|EID8| 
************************************************************************************/
#define TXB0EIDH		0X33
/********************Transmit Buffer 0 Extended Identifier LOW**********************
 |EID7|EID6|EID5|EID4|EID3|EID2|EID1|EID0| 
************************************************************************************/
#define TXB0EIDL		0X34
/********************Transmit Buffer 0 Data Length Code******************************
 |¡ª|RTR|¡ª|¡ª|DLC3|DLC2|DLC1|DLC0| 
************************************************************************************/
#define TXB0DLC		0X35
/********************Transmit Buffer 0 Data ****************************************
 from 0x36 to 0x3D
************************************************************************************/
#define TXB0D0		0X36
#define TXB0D1		0X37
#define TXB0D2		0X38
#define TXB0D3		0X39
#define TXB0D4		0X3A
#define TXB0D5		0X3B
#define TXB0D6		0X3C
#define TXB0D7		0X3D



//****		TRANSMIT BUFFER 1 ..
/********************Transmit Buffer 1 Standard Identifier High**********************
 |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3| 
************************************************************************************/
#define TXB1SIDH		0X41
/********************Transmit Buffer 1 Standard Identifier Low**********************
 |SID2|SID1|SID0|Reserved|EXIDE|Reserved|EID17|EID16| 
************************************************************************************/
#define TXB1SIDL		0X42
/********************Transmit Buffer 1 Extended Identifier High**********************
 |EID15|EID14|EID13|EID12|EID11|EID10|EID9|EID8| 
************************************************************************************/
#define TXB1EIDH		0X43
/********************Transmit Buffer 1 Extended Identifier LOW**********************
 |EID7|EID6|EID5|EID4|EID3|EID2|EID1|EID0| 
************************************************************************************/
#define TXB1EIDL		0X44
/********************Transmit Buffer 1 Data Length Code******************************
 |¡ª|RTR|¡ª|¡ª|DLC3|DLC2|DLC1|DLC0| 
************************************************************************************/
#define TXB1DLC		0X45
/********************Transmit Buffer 1 Data ****************************************
 from 0x46 to 0x4D
************************************************************************************/
#define TXB1D0		0X46	
#define TXB1D1		0X47	
#define TXB1D2		0X48   
#define TXB1D3		0X49  
#define TXB1D4		0X4A   
#define TXB1D5		0X4B   
#define TXB1D6		0X4C   
#define TXB1D7		0X4D



//****		TRANSMIT BUFFER 2 ..
/********************Transmit Buffer 2 Standard Identifier High**********************
 |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3| 
************************************************************************************/
#define TXB2SIDH		0X51
/********************Transmit Buffer 2 Standard Identifier Low**********************
 |SID2|SID1|SID0|Reserved|EXIDE|Reserved|EID17|EID16| 
************************************************************************************/
#define TXB2SIDL		0X52
/********************Transmit Buffer 2 Extended Identifier High**********************
 |EID15|EID14|EID13|EID12|EID11|EID10|EID9|EID8| 
************************************************************************************/
#define TXB2EIDH		0X53
/********************Transmit Buffer 0 Extended Identifier LOW**********************
 |EID7|EID6|EID5|EID4|EID3|EID2|EID1|EID0| 
************************************************************************************/
#define TXB2EIDL		0X54
/********************Transmit Buffer 2 Data Length Code******************************
 |¡ª|RTR|¡ª|¡ª|DLC3|DLC2|DLC1|DLC0| 
************************************************************************************/
#define TXB2DLC		0X55
/********************Transmit Buffer 2 Data ****************************************
 from 0x56 to 0x5D
************************************************************************************/
#define TXB2D0		0X56
#define TXB2D1		0X57
#define TXB2D2		0X58
#define TXB2D3		0X59 
#define TXB2D4		0X5A 
#define TXB2D5		0X5B 
#define TXB2D6		0X5C 
#define TXB2D7		0X5D  


//****		RECEIVE BUFFER 0 ...
/********************Receive Buffer 0 Standard Identifier High**********************
 |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3| 
************************************************************************************/
#define RXB0SIDH	0X61
/********************Receive Buffer 0 Standard Identifier Low**********************
 |SID2|SID1|SID0|SRR|IDE|¡ª|EID17|EID16|
 SRR: Standard Frame Remote Transmit Request Bit (valid only if IDE bit = ¡®0¡¯).1 =
 Standard Frame Remote Transmit Request Received.0 = Standard Data Frame Recieved.
 IDE: Extended Identifier Flag.This bit indicates whether the received message was a 
 Standard or an Extended Frame.1 = Received message was an Extended Frame.0 = Receiv-
 ed message was a Standard Frame.
************************************************************************************/
#define RXB0SIDL		0X62
/********************Receive Buffer 0 Extended Identifier Mid**********************
 |EID15|EID14|EID13|EID12|EID11|EID10|EID9|EID8| 
************************************************************************************/
#define RXB0EIDH		0X63
/********************Receive Buffer 0 Extended Identifier Low**********************
 |EID7|EID6|EID5|EID4|EID3|EID2|EID1|EID0| 
************************************************************************************/
#define RXB0EIDL		0X64
/********************Receive Buffer 0 Data Length Code**********************
 |¡ª|RTR|RB1|RB0|DLC3|DLC2|DLC1|DLC0|
 RTR: Extended Frame Remote Transmission Request Bit (valid only when RXBnSIDL.IDE=1).
 1 = Extended Frame Remote Transmit Request Received.0= Extended Data Frame Received.
 RB1: Reserved Bit 1.
 RB0: Reserved Bit 0.	
************************************************************************************/
#define RXB0DLC		0X65
/********************Receive Buffer 0 Data ******************************************
 from 0x66 to 0x6D
************************************************************************************/
#define RXB0D0		0X66  
#define RXB0D1		0X67 
#define RXB0D2		0X68 
#define RXB0D3		0X69 
#define RXB0D4		0X6A 
#define RXB0D5		0X6B 
#define RXB0D6		0X6C 
#define RXB0D7		0X6D



//****		RECEIVE BUFFER 1 ...
/********************Receive Buffer 1 Standard Identifier High**********************
 |SID10|SID9|SID8|SID7|SID6|SID5|SID4|SID3| 
************************************************************************************/
#define RXB1SIDH	0X71
/********************Receive Buffer 1 Standard Identifier Low**********************
 |SID2|SID1|SID0|SRR|IDE|¡ª|EID17|EID16|
 SRR: Standard Frame Remote Transmit Request Bit (valid only if IDE bit = ¡®0¡¯).1 =
 Standard Frame Remote Transmit Request Received.0 = Standard Data Frame Recieved.
 IDE: Extended Identifier Flag.This bit indicates whether the received message was a 
 Standard or an Extended Frame.1 = Received message was an Extended Frame.0 = Receiv-
 ed message was a Standard Frame.
************************************************************************************/
#define RXB1SIDL		0X72
/********************Receive Buffer 1 Extended Identifier Mid**********************
 |EID15|EID14|EID13|EID12|EID11|EID10|EID9|EID8| 
************************************************************************************/
#define RXB1EIDH		0X73
/********************Receive Buffer 1 Extended Identifier Low**********************
 |EID7|EID6|EID5|EID4|EID3|EID2|EID1|EID0| 
************************************************************************************/
#define RXB1EIDL		0X74
/********************Receive Buffer 1 Data Length Code**********************
 |¡ª|RTR|RB1|RB0|DLC3|DLC2|DLC1|DLC0|
 RTR: Extended Frame Remote Transmission Request Bit (valid only when RXBnSIDL.IDE=1).
 1 = Extended Frame Remote Transmit Request Received.0= Extended Data Frame Received.
 RB1: Reserved Bit 1.
 RB0: Reserved Bit 0.	
************************************************************************************/
#define RXB1DLC		0X75
/********************Receive Buffer 1 Data ******************************************
 from 0x76 to 0x7D
************************************************************************************/
#define RXB1D0		0X76 
#define RXB1D1		0X77 
#define RXB1D2		0X78 
#define RXB1D3		0X79 
#define RXB1D4		0X7A
#define RXB1D5		0X7B 
#define RXB1D6		0X7C 
#define RXB1D7		0X7D



//these are old defines by fan
/*
//bandrate
#define SJW1            0x00
#define SJW2            0x40
#define SJW3            0x80
#define SJW4            0xC0

#define BTLMODE_CNF3    0x80

#define SEG1            0x00
#define SEG2            0x01
#define SEG3            0x02
#define SEG4            0x03
#define SEG5            0x04
#define SEG6            0x05
#define SEG7            0x06
#define SEG8            0x07

#define BRP1            0x00
#define BRP2            0x01
#define BRP3            0x02
#define BRP4            0x03
#define BRP5            0x04
#define BRP6            0x05
#define BRP7            0x06
#define BRP8            0x07  
*/

/*#define IOCTL_MAGIC  't'

typedef struct
{
        unsigned long size;
        unsigned char buff[128];
}__attribute__((packed))  ioctl_info;

#define IOCTL_test1 _IO(IOCTL_MAGIC,0)
#define IOCTL_test2 _IO(IOCTL_MAGIC,1)
#define IOCTL_test3 _IO(IOCTL_MAGIC,2)
#define IOCTL_test4 _IO(IOCTL_MAGIC,3)
*/

#define MCP2510_CanRevBuffer    128     //this is in board RAM.^M
/*********Can data strucutre*********************************************************^M
 This structure used to store message received or transmit message in RAM of board,n-^M
 ot in MCP2510.^M
************************************************************************************/
typedef struct {
        unsigned int id;        //message identifiers 
        unsigned char data[8];          //message data
        unsigned char dlc;              //message length(0 to 8)
        unsigned char IsExt;    //1:it is a extended frame;0:it is a standard frame.
        unsigned char rxRTR;    //1:it is a remote request;0:it is not a remote request.
	unsigned char padbyte;
}CanData, *PCanData;

#define TXBUFLEN 8
#define RXBUFLEN 8

/***************Transmit buffer and receive buffer seperatedly**********/
struct {
	CanData TXdata[TXBUFLEN];       /*Transmit buffer,can contain 8 message*/
	int head; //valid message head.
	int count; //valid message count.
}TXbuffer;

struct RXbuffer{
	CanData RXdata[8];       /*Receive  buffer,can contain 8 message*/
	int head;//valid message head
	int count; //
}RXbuffer;

/*typedef struct {
        CanData MCP2510_Candata[MCP2510_CanRevBuffer];                          	int nCanRevpos;
        int nCanReadpos;
        int loopbackmode;
        wait_queue_head_t wq;
        spinlock_t lock;
}Mcp2510_DEV;
*/

void enable2510(void);
void disable2510(void);
unsigned char Read_Instr_2510(unsigned char R_ADD);
//Implement Bit Modify Instruction of mcp210
void BitModify_Instr_2510(U8 M_ADD, U8 M_MASK, U8 M_DATA);
 //Implement RESET instruction of mcp2510
void Reset_Instr_2510(void);
/********************set Communication parameter********************************************^M
 baud presacle parameter.^M
 BRP[5...0] in CNF1 is for baud rate prescal.^M
 Tq=2*(BRP[5...0]+1)*Tosc.^M
^M
 Based on the fosc=16M^M
 |sync|propagation|phase segment 1|phase segment 2|^M
 sync=1Tq;propagateion time=2Tq;phase segment 1=7Tq;phase segment 2=6Tq.^M
 Nominal Bit Cycle=(1+2+7+6)Tq=16Tq;^M
 Baud rate prescal=0x04h.Tq=2*(prescal+1)Tosc=10Tosc.^M
 So,Nominal Bit Rate of this set will be 100K bit per second. ^M
************************************************************************************/
void MCP2510_SetCommPara(int bit_rate,int tmp_sample);
/********************Set Configuration Mode******************************************^M
 This will allow mcp2510 enter into CONFIGURATION mode.^M
 Configuration mode is automatically selected after powerup or a reset, or can be en-^M
 tered from any other mode by setting the CANCTRL.REQOP bits to ¡®100¡¯.^M
 Configuration mode is the only mode where the following registers are modifiable:^M
 CNF1, CNF2, CNF3^M
 TXRTSCTRL^M
 Acceptance Filter Registers^M
 Acceptance Mask Registers^M
************************************************************************************/
void MCP2510_SetConfigMode(void);
void MCP2510_SetSleepMode(void);

unsigned char ReadStatus_Instr_2510(void);
void MCP2510_SetLoopbackMode(void);
void MCP2510_SetListenOnlyMode(void);
void MCP2510_SetNormalMode(void);
/********************Parse MCP2510 status returned by Read Status instruction********^M
        bit 0.CANINTF.RX0IF^M
        bit 1.CANINTF.RX1IF^M
        bit 2.TXB0CNTRL.TXREQ ^M
        bit 3.CANINTF.TX0IF ^M
        bit 4.TXB1CNTRL.TXREQ ^M
        bit 5.CANINTF.TX1IF ^M
        bit 6.TXB2CNTRL.TXREQ ^M
        bit 7.CANINTF.TX2IF ^M
************************************************************************************/
int Parse_Status(char ret,unsigned care,unsigned wichTXBCNTRL);
//Initialize mcp2510
void Init_MCP2510(void);
//Use TXB 0 Send standard frame message to CAN bus^M
//j:which tx data in TXdata[10];j=0,...9.^M
//k:which tx buffer should be used;k=0,1,2;
int can_data_send(int j,int k);
//Read out received standard frame message in MCP2510^M
//when poll successfully or interrupt on received a message,read the message from RXB0
int can_data_receive(int which);
void Test_can_bus(void);

//WHEN READING AND WRITING SAME BUFFER
//WHEN USER WHAT TO READ THE rxdata[i],and the driver what to write it ,then it will happen.
//or then reading one txdata[i],and write by user ,also will happen this .

struct semaphore rx_mutex;
struct semaphore tx_mutex;

#define IOCTL_MOD_SET 0
#define IOCTL_GET_MODE 1
#define IOCTL_GET_CANSTAT 2

#endif

