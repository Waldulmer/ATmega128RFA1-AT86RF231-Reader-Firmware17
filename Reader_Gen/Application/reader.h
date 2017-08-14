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

/**
@addtogroup pposreader
@{
@name Reader typedefs (reader.h)
@{
*/

typedef struct{
    bool FirstRun       : 1;
    bool ReaderSetup    : 1;
    bool ValidateSetup  : 1;
	bool EnableOfflineTransaction	: 1;
	bool MaxNumTransReached			: 1;
	bool OfflineTransactionExist	: 1;
    bool Busy			: 1;
	bool ReaderNameSetup            : 1; 
	bool ReaderLabelSetup			: 1;  
	bool showModifiedVendPrice		: 1;
	u16   : 6;
} __attribute__((packed)) structReaderFlag;
structReaderFlag ReaderStateFlag;

typedef struct{
    bool sendingOfflineTransactions     : 1;
	bool waitingForBalance				: 1;
	u8   : 6;
} __attribute__((packed)) structTimeoutFlag;
structTimeoutFlag timeout;

typedef struct {
	
	bool deviceIsOffline	: 1; // 0 - running, 1 - offline
	u8						: 7; //remaining bit flags
} __attribute__((packed)) structDeviceFlag;

structDeviceFlag deviceFlag;

typedef struct  
{
	u8 deviceType[2];
	structDeviceFlag flags;
	
}__attribute__((packed)) structDeviceStatus;

structDeviceStatus deviceStatus;

typedef struct {
	
	u32 customerId;
    u16 manufacturerId;
	u16 locationId;
	u8	maxOfflineTransaction;
	u8  numOfSavedTransactions;
	u8 machineDescription[MACHINE_NAME_SIZE+1];
	u8 machineLabel[MACHINE_LABEL_SIZE+1];		
}	__attribute__((packed)) structReaderSetup;

structReaderSetup ReaderSetup;

typedef struct{
    u8  Type;
	unsigned long	ID;
    u16 Pin;
	u16	Value;
	u8 	CardState;
}	__attribute__((packed)) structCurrentAccount;

structCurrentAccount CurrentAccount;

typedef struct  
{
	u8 hours;
	u8 minutes;
	u8 seconds;
}__attribute__((packed)) structTime;

typedef struct
{
	u8 month;
	u8 day;
	u16 year;
}__attribute__((packed)) structDate;

typedef struct {
	
	u16 LocationId;
	u8 MachineId[2];	//machine type involved in transaction, i.e. washer, dryer
	u16 ManufactureId;	//gen2, MDC, etc.
	u8 CycleType;		//machine cycle type
	u16 vendPrice;		//cost of cycle at transaction time
	u32 CardId;			//serial number of card involved in transaction
	u8 Date[4];			//date of transaction based on time received from server. BCD (mm,dd,yyyy).
	u8 Time[3];			//number of seconds from 12AM; 12:00:00AM == 0 seconds, 11:59:59PM == 86399 seconds.
	u8 isOffline;
}  __attribute__((packed)) structTransaction;




/**
@addtogroup pposreader
@{

@section operation ACA Card Reader

The Payment System and Machine Control exchange critical information in the Initialization Communication sequence.
The Payment System will inform the Machine Control during the first communication sequence which Vending Option it is designed to operate under.
In the Payment System Driven Vend Price Option, the Payment System generates all vend prices used in vending. The Payment System must perform an Initialization Communication sequence ten seconds after power-up.
The Payment System will continually send the current vend prices to the Machine Control. The Payment System communicates
asynchronously with the Machine Control via the SCI port of the Machine Control?s microcontroller. Information is sent
between the Payment System and the Machine Control in variable length packets. The final packet in the sequence will be the Vend Price Packet from the Payment System to
the Machine Control. This packet sends the Payment System’s current vend price to the Machine Control. This vend
price will be shown on the display, and used in a vending sequence.
The communication will occur with a variation of ACK/NAK protocol. Validation of packet data will be done by the use of a Block Check Count (BCC). The
speed of communication by default is 9600 baud using 8 data bits with 1 start bit and 1 stop bit.
*/
/**
@name Machine Control to Payment System Driven Vending
@{
The following structure describes the Machine Status packet used in Payment System Driven Vending.
The Payment System must respond to the Machine Status Packet by sending a Vend Price Packet.
*/
typedef struct{
	//Machine Status packet = 0x71
	u8 	MachineType[2];					//MachineType[0] = 33,34,29, MachineType[1] = 3
	u8 	CycleType;						//normal, delicate,
	u8  CycleModifier;					//Cycle Modifier Selections
	u8 	CmdToReader;					//Command to Payment System
	u8 	MachineStatus[2];				//Machine Status
	u8 	unused1[2];
	u8 	RemainingCycleMinutes;			//Remaining Cycle Time
	u8 	RemainingCycleSeconds;
	u8	RemainingVend[2];				//Required/Remaining Vend
	u8	VendenteredTotal[2];			//Vend Entered Total (all sources)
	u8	VendenteredforCycleModifier[2];
	u8	VendenteredforTopOff[2];		//Vend Entered for Top-Off
	u8	NumberofCoins1;
	u8	NumberofCoins2;
	u8 	KeypadData[2];
	u8  CurrentKey[2];					//Current Key Selections
	u8  unused2;
	

} __attribute__((packed)) structMachineStatus;
/** @} */
structMachineStatus SQACAMachineStatus;

/**
@name TOPLOAD WASHER PROGRAMMING DATA PACKET
@{
When a valid Programming Command is sent into the Payment System, the reader will initiate a programming
sequence by sending the Programming Data packet.
The Machine Control will now determine the validity of this data. The Machine Control will first check Product
Bytes #1, Product Byte #2, and Product Byte #3 to ensure that the programming data it has just received matches the
type of machine that it is installed in. However, the control will make no restrictions based on these values. See
Section 3.7.2 for details on the values used for these bytes.) Finally, the control will check each individual
programming byte, and ensure that all programming bytes are within their specified range. (See Section 6 for details
on the acceptable range of values for these bytes.)
If all data is valid (all bytes in range), the Machine Control will now program these parameters into its own nonvolatile
memory and respond with an ACK. If any part of the data is invalid, the Machine control will return an INV
byte, display an error message, and will discard the programming information it just received.

The following structure describes the full programming Data Packet for Toploaders. It allows
full access to the global and machine cycle programming parameters in the machine control.
*/
typedef struct{
	// 0x21 (TLW Prog)
	u8 ProductByte[5];				//Topload Washer Programming Data Packet
	u8 VendPrice1[2];
	u8 VendPrice2[2];
	u8 VendPrice3[2];
	u8 VendPrice4[2];
	u8 VendPrice5[2];
	u8 VendPrice6[2];
	u8 VendPrice7[2];
	u8 VendPrice8[2];
	u8 VendPrice9[2];
	u8 MediumCycle_VendPrice[2];
	u8 HeavyCycle_VendPrice[2];
	u8 MediumCycle_option;
	u8 HeavyCycle_option;
	u8 MediumCycle_extraWashTime;
	u8 MediumCycle_extraRinseTime;
	u8 HeavyCycle_extraWashTime;
	u8 HeavyCycle_extraRinseTime;
	u8 NormalCycle_washAgitateTime;
	u8 NormalCycle_rinseAgitateTime;
	u8 NormalCycle_extraRinseAgitateTime;
	u8 NormalCycle_finalSpinTime;
	u8 PermPressCycle_washAgitateTime;
	u8 PermPressCycle_rinseAgitateTime;
	u8 PermPressCycle_extraRinseAgitateTime;
	u8 PermPressCycle_finalSpinTime;
	u8 DelicateCycle_washAgitateTime;
	u8 DelicateCycle_rinseAgitateTime;
	u8 DelicateCycle_extraRinseAgitateTime;
	u8 DelicateCycle_finalSpinTime;
	u8 DefaultCycle;
	u8 DefaultCycleModifier;
	u8 WarmRinse;
	u8 AudioSetting;
	u8 ControlConfig;						//0x74 MACHINE CONFIGURATION BYTES #1 - #5
	u8 ControlSN;						// ID Serial Number
} __attribute__((packed)) structToploadProg;
/** @} */

structToploadProg SQACAToploadProgramming;
/**
@name FRONTLOAD WASHER PROGRAMMING DATA PACKET
@{

The following structure describes the full programming Data Packet for Frontloaders. It allows
full access to the global and machine cycle programming parameters in the machine control.
*/
typedef struct{
	// (FLW Prog)
	u8 ProductByte[5];				//Frontload Washer Programming Data Packet
	u8 VendPrice1[2];
	u8 VendPrice2[2];
	u8 VendPrice3[2];
	u8 VendPrice4[2];
	u8 VendPrice5[2];
	u8 VendPrice6[2];
	u8 VendPrice7[2];
	u8 VendPrice8[2];
	u8 VendPrice9[2];
	u8 MediumCycle_VendPrice[2];
	u8 HeavyCycle_VendPrice[2];
	u8 MediumCycle_option;
	u8 HeavyCycle_option;
	u8 MediumCycle_extraWashTime;
	u8 MediumCycle_extraRinseTime;
	u8 HeavyCycle_extraWashTime;
	u8 HeavyCycle_extraRinseTime;
	u8 DefaultCycle;
	u8 DefaultCycleModifier;
	u8 AudioSetting;
	u8 ControlConfig;						//MACHINE CONFIGURATION BYTE 0x74
	u8 ControlSN;						// Serial Number
} __attribute__((packed)) structFrontloadProg;
/** @} */
structFrontloadProg SQACAFrontloadProgramming;

/**
@name DRYER PROGRAMMING DATA Packet
@{
The following structure describes the full programming Data Packet for Dryers. It allows
full access to the global and machine cycle programming parameters in the machine control.
*/
typedef struct{
	//(0x29 for Dryer Prog)
	u8 ProductByte[5];				//Dryer Washer Programming Data Packet
	u8 HeatVendPrice1[2];
	u8 HeatVendPrice2[2];
	u8 HeatVendPrice3[2];
	u8 HeatVendPrice4[2];
	u8 NoHeatVendPrice[2];
	u8 PaymSTopoffOn;
	u8 PaymSTopoffPrice[2];
	u8 PaymSTopoffMinutes;
	u8 PaymSTopoffSeconds;
	u8 Coin1TopoffMinutes;
	u8 Coin1TopoffSeconds;
	u8 Coin2TopoffMinutes;
	u8 Coin2TopoffSeconds;
	u8 HeatCycleMinutes;
	u8 HeatCycleSeconds;
	u8 NoHeatCycleMinutes;
	u8 NoHeatCycleSeconds;
	u8 HighCoolDownTime;
	u8 MediumCoolDownTime;
	u8 LowCoolDownTime;
	u8 DelicateCoolDownTime;
	u8 HighTempSetting;
	u8 MediumTempSetting;
	u8 LowTempSetting;
	u8 DelicateTempSetting;
	u8 DefaultCycle;
	u8 AudioSetting;
	u8 AudioEnable1;
	u8 AudioEnable2;
	u8 DisplaySetting1;
	u8 DisplaySetting2;
	u8 ControlConfig;					//MACHINE CONFIGURATION BYTE 0x74
	u8 ControlSN;						// Serial Number
} __attribute__((packed)) structDryerProg;
/** @} */
structDryerProg SQACADryerProgramming;

typedef struct{
	u8 ProductByte[5];
	u8 ControlSN[4];
	u8 ControlSVN;
	u8 MachineCyclesCounter[2];
	u8 CycleModifierCounter[2];
	u8 AvgFillTime[2];
	u8 AvgDrainTime[2];
	u8 VendedTotal[4];
	u8 VendedTotalBaseVendPrice[4];
	u8 VendedTotalCycleModifiers[4];
	u8 VendedTotalPS[4];
	u8 VendedTotalPSBaseVendPrice[4];
	u8 VendedTotalPSCycleModifiers[4];
	u8 VendedTotalPSDiscounted[4];

} __attribute__((packed)) structMachineAudit;

structMachineAudit SQACAAuditData;

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
/** @} */
#endif /* READER_H_ */