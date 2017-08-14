/** \file
*  \brief Header file for ha_timer.c.
*/

#ifndef MACHINE_H
#define MACHINE_H

#include "rum_types.h"
/**
@addtogroup pposreader
@{
@name Machine related Programming Data
@{
*/

#define MACHINE_WAIT            0x00
#define MACHINE_TIME_OUT        0x01
#define MACHINE_WAIT_TIMEOUT    9000   //In ms, the time period to wait for a machine to power up. PPOS: attempt to cut down wait time
//#define MACHINE_WAIT_TIMEOUT    10000   //In ms, the time period to wait for a machine to power up

#if (DEVICE_CONNECTED == ACA_MACHINE)

#include "SpeedQueen.h"



u8 ucMachineWait;

/** @} */
/** @} */

#endif //Machine type 

//void SetDefaultMachineSetup(void);

void WaitForMachine(void);


void InitTimerZero(void);
void StartTimerZero(void);
void StopTimerZero(void);




/** @} */

#endif // MACHINE_H
