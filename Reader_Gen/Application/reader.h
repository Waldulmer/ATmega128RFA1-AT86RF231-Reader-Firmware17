/*
 * reader.h
 *
 * Created: 6/15/2011 9:56:05 PM
 *  Author: Paul
 */ 
/** \file
 *  \brief Header file for reader.c.
 */


#ifndef READER_H_
#define READER_H_

#include "rum_types.h"
#include "eeprom_map.h"
#include "UART/serial.h"
#include "ppos_types.h"

/**

@addtogroup subgroup
@{
@name Reader Header (reader.h)
@{
*/

void initReader(void);
void getDate(structDate *date);
void getTime(structTime *time);
u8 storeOfflineTransaction(u32 cardNum);

u8 sendStoredTransaction(void);
bool sendBOWCCTransaction(structTransaction *transRecord);

static inline void timeoutWaitingForOLTReponse(void)
{
	
	timeout.sendingOfflineTransactions = true;
}

//Get structure for vend transaction
void getSQVend(structTransaction *vend);
/** @} */
#define MACHINE_WAIT            0x00
#define MACHINE_TIME_OUT        0x01
#define MACHINE_WAIT_TIMEOUT    9000   //In ms, the time period to wait for a machine to power up. PPOS: attempt to cut down wait time

#if (DEVICE_CONNECTED == ACA_MACHINE)
#include "SpeedQueen.h"
u8 ucMachineWait;
#endif //Machine type

void WaitForMachine(void);
void InitTimerZero(void);
void StartTimerZero(void);
void StopTimerZero(void);

/** @} */
#endif /* READER_H_ */