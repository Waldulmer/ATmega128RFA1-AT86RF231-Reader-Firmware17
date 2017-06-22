/*
 * SpeedQueen.h
 *
 * Created: 5/6/2013 6:51:00 AM
 *  Author: Paul
 */ 
/** \file
 *  \brief Header file for SpeedQueen.c.
 */


#ifndef SPEEDQUEEN_H_
#define SPEEDQUEEN_H_

#include "display.h"
/**
@addtogroup pposreader
@{
	@name Speedqueen related functions
	@{
*/

//#define MDC_SERIES		1
//#define QUANTUM_SERIES	2
#define ACA_SERIES		3

#define ACA_STATUS_REQUEST_PACKET	0x70		//PS: Status Request
#define ACA_STATUS_REQUEST_BYTES	1

#define ACA_STATUS_RESPONSE_PACKET	0x71		//MC: Machine Status Packet, Payment System Driven Vending
#define ACA_STATUS_RESPONSE_BYTES	26

#define ACA_VEND_PRICE_PACKET		0x72		//PS: Vend Price Packet
#define ACA_VEND_PRICE_BYTES		10			
//define INIT
#define ACA_INIT_REQUEST_PACKET		0x73		//PS: Payment System Initiation Packet 
#define ACA_INIT_REQUEST_BYTES		9

#define ACA_INIT_RESPONSE_PACKET	0x74		//MC: Machine Control Initiation Packet  
#define ACA_INIT_RESPONSE_BYTES		31
//define AUDIT
#define AUDIT_DATA_REQUEST_PACKET	0x30		//MC: Audit Data Packet
#define	AUDIT_DATA_REQUEST_BYTES	1

#define ACA_AUDIT_RESPONSE_TLWPACKET	0x31	// (TLW Audit Data)
#define ACA_AUDIT_RESPONSE_FLWPACKET	0x32	// (FLW Audit Data)
#define ACA_AUDIT_RESPONSE_DRPACKET		0x39	// (Dryer Audit Data)
#define ACA_AUDIT_RESPONSE_BYTES	47

#define ACA_VEND_BREAKDOWN_PACKET	0x75

#define ACA_A4_MIMIC_QUANTUM_PACKET	0x64		//Mimic Quantum Interface = 0x64
#define ACA_A4_MIMIC_QUANTUM_BYTES	3			//Number of bytes in Data Field = 3
#define ACA_A4_STARTPAD_FLASHING	30
#define ACA_A4_LOCKCYCLE_ENABLED	1

#define ACA_VENDING_PAYMENT_PACKET	0x6A		//Vending Payment = 0x6A
#define ACA_VENDING_PAYMENT_BYTES	6			//Number of bytes in Data Field = 6

#define Mimic_Quantum_Lock_Active	0x20

#define REGULAR_VEND	1
#define TOPOFF_VEND		2

#define CASH_CARD		1
#define REMOVE_CARD		2

#define READER_CONTROL_VEND_PRICE	0x10
#define VEND_DISPLAY_ENABLE		0x00
#define VEND_DISPLAY_DISABLE	0x01
#define ACA_BAUD_9600			0x02
#define ACA_BAUD_57600			0x10
#define ACA_BAUD_115200			0x20

//bool isQuantum(void);
//bool isMDC(void);
bool sendSQDataPacket(u8 *buf);
bool getSQDataPacket(u8 *buf);

bool isACA(void);
//receive Initialization Packet 0x74
bool getSQACAInitData(void);               //receive Initialization Packet 0x74
// write setup data to the appropriate location in memory
void getSQSetupData(void);
//STATUS REQUEST PACKET 0x70
bool SQACAMachineStatusSequence(void);         ////STATUS REQUEST PACKET 0x70 SQACAMachineStatusSequence
//receive Status Packet 0x71
bool getSQACAStatusData(void);				//getSQACAStatusData receive Status Packet 0x71
//send Vend Price Packet 0x72
bool sendSQACAVendPrice(void);             //send Vend Price Packet 0x72
//send Audit Data Packet 0x30
bool SQACAAuditDataSequence(void);
//receive Audit Data Packet 0x31
bool getSQACAAuditData(void);
bool waitSQStartKey(void);
bool isSQCycleRunning(void);
//bool sendSQCashCardInserted(void);			//PPOS
//bool sendSQCashCardInsetedForTopoff(void);	//PPOS
//bool sendSQCashCardRemoved(void);				//PPOS
//bool sendSQCardRemoved(void);					//PPOS
//bool sendSQStart(void);						//PPOS
//bool sendSQAddTime(void);						//PPOS
bool sendSQCardErrorCode(u8 cardErrorCode);
bool sendSQAudioBeepRequest(u8 beepLength);
bool sendSQDisplayCommand(u8 led1, u8 led2, u8 led3, u8 led4, u8 led5, u8 led6, u8 duration);
//bool displaySQReaderVersion(void);	//PPOS
bool displaySQCardError(void);
//bool cardRemoved(u8 cardType);		//PPOS
//bool cardInserted(u8 cardType);		//PPOS

//Send machine setup at every power up if reader has setup and machine is connected
bool getSQReaderMachineSetup(void);

//bool startMachineCycle(void);			//PPOS
//send Mimic Quantum Vending 0x64
bool sendSQACAMimicQuantumVending(void);

bool waitForMachineStartKey();
bool isMachineCycleRunning();

//0x73 
void SQACAInitializationSequence(void);
//Sequence of 70,71,72
void SQACAMachineStatusCommSequence(void);
//Mimic Quantum Interface = 0x64
bool sendSQACAVendingTransactions(void);
/** @} */
/** @} */

#endif /* SPEEDQUEEN_H_ */