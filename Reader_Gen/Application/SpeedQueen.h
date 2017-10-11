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
@addtogroup machine
@{
	@name Speedqueen Header (Speedqueen.h)
	@{
*/

// Macros & Defines
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
#define ACA_A4_STARTPAD_FLASHING	15
#define ACA_A4_LOCKCYCLE_ENABLED	1

#define ACA_VENDING_PAYMENT_PACKET	0x6A		//Vending Payment = 0x6A
#define ACA_VENDING_PAYMENT_BYTES	6			//Number of bytes in Data Field = 6

#define MIMIC_QUANTUM_LOCK_ACTIVE	0x20

#define REGULAR_VEND	1
#define TOPOFF_VEND		2

//#define CASH_CARD		1
//#define REMOVE_CARD	2

#define READER_CONTROL_VEND_PRICE	0x10
#define VEND_DISPLAY_ENABLE		0x00
#define VEND_DISPLAY_DISABLE	0x01
#define ACA_BAUD_9600			0x02
#define ACA_BAUD_57600			0x10
#define ACA_BAUD_115200			0x20

//Machine status

#define STX 	0x02
#define ACK		0x06
#define INV		0x09
#define NAK		0x15

//ACA Command and data codes

#define PROGRAMMING_DATA					0x20			//Program Packet Base Value
#define PROGRAMMING_DATA_TOPLOAD			0x21			//Program Packet ID Topload
#define PROGRAMMING_DATA_FRONTLOAD			0x22			//Program Packet ID Frontload
#define PROGRAMMING_DATA_DRYER				0x29			//Program Packet ID Dryer


//#define CASH_CARD_INSERTED			0x40
//#define CARD_REMOVED				0x41
//#define	CASH_CARD_TOPOFF			0x42

#define START_PAD_PRESSED			0x01    //PPOS Start Pad Pressed to Confirm Selection
#define START_PAD_PRESSED_TOPOFF	0x47	//Deduct TopOff command
//#define MACHINE_START_COMMAND		0x48
#define ADD_TIME_COMMAND			0x49
#define CARD_ERROR_PACKET			0x50
#define DISPLAY_REQUEST				0x60
#define AUDIO_BEEP_REQUEST			0x61
#define MANUAL_MODE_ENTRY			0x62
#define PRODUCTION_TEST_CYCLE		0x63

//temp display msg def
#define DISPLAY_REQUEST_CARD        0x70
#define DISPLAY_REQUEST_ERROR       0x71
#define DISPLAY_REQUEST_TIME        0x72
#define DISPLAY_REQUEST_OUT         0x73

#define CASH_CARD_REMOVED			0x99
//extern U8 OP;
//#endif

//ACA packet data length

//#define ADD_TIME_PACKET_SIZE		1
//#define CARD_REMOVED_SIZE			1
#define CARD_ERROR_PACKET_SIZE		2
#define AUDIO_BEEP_REQUEST_SIZE		2
//#define STATUS_REQUEST_SIZE			2
#define CASH_CARD_REMOVED_SIZE		5
//#define VEND_PRICE_SIZE				4
//#define CASH_CARD_SIZE				5
#define DISPLAY_REQUEST_BYTES		8
//#define CASH_CARD_TOPOFF_SIZE		6
//#define MACHINE_STATUS_SIZE			11
//#define PROGRAMMING_DATA_SIZE		43
#define	AUDIT_DATA_PACKET_SIZE		1
#define	ACA_MAX_PACKET_SIZE			60
#define QTL_PROGRAMMING_DATA_SIZE	50
#define QFL_PROGRAMMING_DATA_SIZE	37
//#define QWE_PROGRAMMING_DATA_SIZE	38
#define QDT_PROGRAMMING_DATA_SIZE	37

//Machine Type
#define INVALID_MACHINE			0x00

//Machine Status 0x71 PRIMARY MODES
#define MACHINE_READY_MODE		0x01
//#define MACHINE_PARTIAL_MODE	0x02
#define MACHINE_START_MODE		0x04

#define MACHINE_RUN_MODE		0x08
#define MACHINE_DOORLOCK_MODE	0x09

//Machine Status 0x71 SECONDARY MODES
//#define MACHINE_MIMIC_LOCK_ACTIVE	0x20

//Card Status
//#define CARD_OUT				0x00
//#define CARD_IN					0x80

#define NUMRETRIES				8

extern u16 ucCardBalance;
extern u16 cardId;
extern double vendPrice;
extern u8 ucTimerCount;
u8 ucCardStatus;
//#define APP_GET_ASSC()    
/** @} */

// Implementation

void getSQAuditData(void);
bool sendSQDataPacket(u8 *buf);
bool getSQDataPacket(u8 *buf);

bool isACA(void);
//receive Initialization Packet 0x74
bool getSQACAInitData(void);               //receive Initialization Packet 0x74
// write setup data to the appropriate location in memory
void getSQSetupData(void);
//STATUS REQUEST PACKET 0x70
bool SQACAMachineStatusSequence(void);		//STATUS REQUEST PACKET 0x70 SQACAMachineStatusSequence
//receive Status Packet 0x71
bool getSQACAStatusData(void);				//getSQACAStatusData receive Status Packet 0x71
//send Vend Price Packet 0x72
bool sendSQACAVendPrice(void);				//send Vend Price Packet 0x72
//send Audit Data Packet 0x30
bool SQACAAuditDataSequence(void);
//receive Audit Data Packet 0x31
bool getSQACAAuditData(void);
bool waitSQStartKey(void);
bool isSQCycleRunning(void);

bool sendSQCardErrorCode(u8 cardErrorCode);
bool sendSQAudioBeepRequest(u8 beepLength);

//Send machine setup at every power up if reader has setup and machine is connected
bool getSQReaderMachineSetup(void);

//send Mimic Quantum Vending 0x64
bool sendSQACAMimicQuantumVending(void);

bool waitForMachineStartKey();
bool isMachineCycleRunning();

//0x73 
void SQACAInitializationSequence(void);
//Sequence of 70,71,72
void SQACAMachineStatusCommSequence(void);
//Mimic Quantum Interface = 0x64
bool sendSQACAVendingTransaction(u8 vendType);
//Connected to the network, machine is online or offline
bool isAssociated(void);


/** @} */

#endif /* SPEEDQUEEN_H_ */