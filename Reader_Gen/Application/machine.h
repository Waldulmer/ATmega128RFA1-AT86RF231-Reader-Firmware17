/** \file
*  \brief Header file for machine.c.
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


void getSQAuditData(void);

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

//#define AUDIT_DATA_REQUEST			0x30
//#define AUDIT_DATA_TOPLOAD			0x31
//#define AUDIT_DATA_FRONTLOAD		0x32
//#define AUDIT_DATA_WASHER_EXTRACTOR	0x34
//#define AUDIT_DATA_TUMBLER			0x38
//#define AUDIT_DATA_DRYER			0x39	

#define CASH_CARD_INSERTED			0x40
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

#define ADD_TIME_PACKET_SIZE		1
#define CARD_REMOVED_SIZE			1
#define CARD_ERROR_PACKET_SIZE		2
#define AUDIO_BEEP_REQUEST_SIZE		2
//#define STATUS_REQUEST_SIZE			2
#define CASH_CARD_REMOVED_SIZE		5
//#define VEND_PRICE_SIZE				4
#define CASH_CARD_SIZE				5
#define DISPLAY_REQUEST_BYTES		8
#define CASH_CARD_TOPOFF_SIZE		6
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

//Washer keypad data
#define WASHER_START			0x02
#define WASHER_DELICATE			0x04
#define WASHER_NORMAL			0x08
#define WASHER_PERM_PRESS		0x40
#define WASHER_HEAVY			0x80

//Dryer/Tumbler keypad data
#define DRYER_MEDIUM_TEMP		0x01
#define DRYER_DELICATE			0x02
#define DRYER_START				0x04
#define DRYER_HIGH_TEMP			0x10
#define DRYER_LOW_TEMP			0x20

//ACA Topload Washer Cycle Modifier Definitions
#define LIGHT				0x00		//LIGHT/A
#define MEDIUM				0x01		//MEDIUM/B/Extra Wash
#define HEAVY				0x02		//Heavy/C/Extra Rinse
#define SMALLLOAD			0x03
//Machine command to reader
//#define NORMAL_STATE			0x00
//#define DEDUCT_CYCLE_VEND		0x46
//#define DEDUCT_TOPOFF_VEND		0x47


//Machine Status 0x71 PRIMARY MODES
//Machine Status 0x71 PRIMARY MODES
#define MACHINE_READY_MODE		0x01
#define MACHINE_PARTIAL_MODE	0x02
#define MACHINE_START_MODE		0x04
#define MACHINE_PFSTART_MODE	0x05
#define MACHINE_RUN_MODE		0x08
#define MACHINE_DOORLOCK_MODE	0x09
#define MACHINE_UNLOCK_MODE		0x0A
#define MACHINE_PAUSE_MODE		0x0B
#define MACHINE_END_MODE		0x0C
#define MACHINE_ERROR_MODE		0x80
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

u8 ucMachineWait;

/** @} */
/** @} */

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
	u8 ControlConfig;						//MACHINE CONFIGURATION BYTE 0x74	
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


u8 sendMdcPacket(u8 *ucDataBuf);


#endif //Machine type 

void SetDefaultMachineSetup(void);

void WaitForMachine(void);

//u8 SerialSendMachineData(u8 ucCommandType);		//PPOS

void InitTimerZero(void);
void StartTimerZero(void);
void StopTimerZero(void);
//void SetMachineSetupData(void);		//PPOS

//void ProcessRadioData(void); //PPOS

/** @} */

#endif // MACHINE_H
