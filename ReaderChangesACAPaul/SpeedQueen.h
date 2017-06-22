/*
 * SpeedQueen.h
 *
 * Created: 5/6/2013 6:51:00 AM
 *  Author: Paul
 */ 


#ifndef SPEEDQUEEN_H_
#define SPEEDQUEEN_H_

#include "display.h"

#define MDC_SERIES		1
#define QUANTUM_SERIES	2
#define ACA_SERIES		3

#define MDC_STATUS_REQUEST			0x10
#define QUANTUM_STATUS_REQUEST		0x16

#define MDC_STATUS_RESPONSE			0x11
#define QUANTUM_STATUS_RESPONSE		0x17

#define MDC_STATUS_DATA_SIZE		11
#define QUANTUM_STATUS_DATA_SIZE	18

#define REGULAR_VEND	1
#define TOPOFF_VEND		2

#define CASH_CARD		1
#define REMOVE_CARD		2

#define  READER_CONTROL_VEND_PRICE	0x10
#define MACHINE_CONTROL_VEND_PRICE	0x20

#ifdef ACA_MACHINE
#define VEND_DISPLAY_ENABLE		0x00
#define VEND_DISPLAY_DISABLE	0x01

#define ACA_BAUD_9600			0x02
#define ACA_BAUD_57600			0x10
#define  ACA_BAUD_115200		0x20

#define ACA_STATUS_REQUEST_SIZE		1
#define ACA_VEND_PRICE_SIZE			10

#define ACA_STATUS_REQUEST		0x70
#define ACA_STATUS_PACKET		0x71
#define ACA_VEND_PRICE_PACKET	0x72
#define ACA_INIT_REQUEST		0x73
#define ACA_INIT_PACKET			0x74
#define ACA_VEND_BREAKDOWN_PACKET	0x75

#define ACA_ERROR_REQUEST		0x1B
#define ACA_PASS_THROUGH_CARD_SN	0x1C

#define ACA_MIMIC_QUANTUM_INTERFACE_PACKET	0x64
#define ACA_FLASH_INSERT_CARD_PACKET		0x65
#define ACA_PAYMENT_SYSTEM_VEND				0x6A //card reader to machine controller
#define ACA_LUCKY_CYCLE_VEND				0x6B //machine controller to card reader
#define ACA_DIAGNOSTIC_COMMANDS				0x6D
#define ACA_SPECIAL_COMMANDS				0x6F

#define ACA_GET_MACHINE_STATUS		0x80
#define ACA_GET_FULL_AUDIT_DATA		0x90
#define ACA_GET_GLOBAL_PROGRAM		0xA0
#define ACA_GET_SPECIAL_VEND_PRICE	0xB0
#define ACA_SET_GLOBAL_PROGRAMMING	0xC0
#define ACA_SET_MACHINE_CYCLE_PROGRAM	0xC0 //0xCX
#define ACA_SET_SPECIAL_VEND_PRICE	0xD0
#define ACA_INVALID_DATA_PACKET		0x05
#endif // ACA_MACHINE

bool isQuantum(void);
bool isMDC(void);
bool sendSQData(u8 *buf);
bool getSQData(u8 *buf);
bool getSQInitData(void);
void getSQSetupData(void);
bool getSQMachineStatus(void);
bool getSQStatusData(void);
bool sendSQVendPrice(void);
bool waitSQStartKey(void);
bool isSQCycleRunning(void);
bool sendSQCashCardInserted(void);
bool sendSQCashCardInsetedForTopoff(void);
bool sendSQCashCardRemoved(void);
bool sendSQCardRemoved(void);
bool sendSQStart(void);
bool sendSQAddTime(void);
bool sendSQCardErrorCode(u8 cardErrorCode);
bool sendSQAudioBeepRequest(u8 beepLength);
bool sendSQDisplayCommand( u8 led1, u8 led2,u8 led3, u8 led4, u8 duration);
bool displaySQReaderVersion(void);
bool displaySQCardError(void);
bool cardRemoved(u8 cardType);
bool cardInserted(u8 cardType);

bool setupMachine(void);
bool startMachineCycle(void);

void initMachine(void);
bool waitForMachineStartKey();
bool isMachineCycleRunning();
void MachineStatus(void);

#endif /* SPEEDQUEEN_H_ */