/*
* SpeedQueen.c
*
* Created: 2/15/2017  Author: Paul updated by Sabine
* changes:
*			Introduced new MachineType[1] == ACA_SERIES
*			ACA machines only: machine types 33,34,41
*			VendPrice changed from VendPrice1 to VendPrice9
*			Machine Status Communication Sequence in the Payment System Driven Vend Price option: 0x70 Status Request, ACK,	0x72 Vend Price, ACK, 0x71 Machine Status, ACK
*			Cash Card Inserted Packet, Cash Card Removed Packet are eliminated
*			Data Packets used:
*			Vending Payment = 0x6A
*			Mimic Quantum Interface = 0x64
*			send Initialization Packet 0x73
*			receive Initialization Packet 0x74
*			Vend Price Packet 0x72
*			STATUS REQUEST PACKET 0x70
*/

#include "mac.h"
#include "application.h"
#if(DEVICE_CONNECTED == ACA_MACHINE)
#include "machine.h"
#include "serial.h"
#include "SpeedQueen.h"
#include "mac_event.h"
#include "reader.h"

/**
   @addtogroup pposreader
   @{
   @name Speedqueen related functions
   @{
*/

/**
   @brief FULL PROGRAMMING
   A Programming Command may be sent to program a limited set of parameters in the Machine Control. When a
   valid Programming Command is received, the Machine Control will verify the data in the packet. If the data is
   valid, the Machine Control will write the data to the appropriate location in memory.
   If all data is valid (all bytes in range), the Machine Control will now program these parameters into its own nonvolatile
   memory and respond with an ACK. If any part of the data is invalid, the Machine control will return an INV
   byte, display an error message, and will discard the programming information it just received.

   @ingroup pposreader
*/
void getSQSetupData(void)
{
	u8 tmp[ACA_MAX_PACKET_SIZE] = {0};								//60 Bytes
	//Retrieve SQ setup info from eeprom
	switch( deviceStatus.deviceType[0])								//switch MachineType[0] = 21,22,29, MachineType[1] = 3
	{
		case PROGRAMMING_DATA_TOPLOAD:
		halGetEeprom(MACHINE_SETUP_ADDR, 21, (u8*)&tmp);			//read machine setup string 21 Bytes from eeprom PPOS170601
		//halGetEeprom(MACHINE_SETUP_ADDR, 16, (u8*)&tmp);			//read machine setup string 16 Bytes from eeprom
		vendPrice = (tmp[9] * 256 + tmp[10]) / 100.0;				//Perm Press/Warm Cycle Vend Price (cents) default
		
		SQACAToploadProgramming.ProductByte[0];
		SQACAToploadProgramming.ProductByte[1];
		SQACAToploadProgramming.ProductByte[2];
		SQACAToploadProgramming.VendPrice1[0] = tmp[1];		//Normal/Hot Cycle Vend Price (cents)
		SQACAToploadProgramming.VendPrice1[1] = 0;
		SQACAToploadProgramming.VendPrice2[0] = tmp[3]; 	//Normal/Warm Cycle Vend Price (cents)
		SQACAToploadProgramming.VendPrice2[1] = 0;
		SQACAToploadProgramming.VendPrice3[0] = tmp[5];		//Normal/Cold Cycle Vend Price (cents)
		SQACAToploadProgramming.VendPrice3[1] = 0;
		SQACAToploadProgramming.VendPrice4[0] = tmp[7];		//Perm Press/Hot Cycle Vend Price (cents)
		SQACAToploadProgramming.VendPrice4[1] = 0;
		SQACAToploadProgramming.VendPrice5[0] = tmp[9];		//Perm Press Cycle Vend Price (cents)
		SQACAToploadProgramming.VendPrice5[1] = 0;
		SQACAToploadProgramming.VendPrice6[0] = tmp[11];	//Perm Press/Cold Cycle Vend Price (cents)
		SQACAToploadProgramming.VendPrice6[1] = 0;
		SQACAToploadProgramming.VendPrice7[0] = tmp[13];	//Delicate/Hot Cycle Vend Price (cents)
		SQACAToploadProgramming.VendPrice7[1] = 0;
		SQACAToploadProgramming.VendPrice8[0] = tmp[15];	//Delicate/Warm Cycle Vend Price (cents)
		SQACAToploadProgramming.VendPrice8[1] = 0;
		SQACAToploadProgramming.VendPrice9[0] = tmp[17];	//Delicate/Cold Cycle Vend Price (cents)
		SQACAToploadProgramming.VendPrice9[1] = 0;
		SQACAToploadProgramming.MediumCycle_VendPrice[0] = tmp[19];		//Cycle Modifier Key #1 Vend Price Adder (Medium/B/Extra Wash)
		SQACAToploadProgramming.MediumCycle_VendPrice[1] = 0;			//Default: 25 (All Controls)
		SQACAToploadProgramming.HeavyCycle_VendPrice[0] = 50;			//Cycle Modifier Key #2 Vend Price Adder (Heavy/C/Extra Rinse)
		SQACAToploadProgramming.HeavyCycle_VendPrice[1] = 0;			//Default: 25 (A2) 50 (A4/A3)
		SQACAToploadProgramming.MediumCycle_option = 0;					//- bit 0: Warm Rinse (1 = Enabled) Default: 0x00
		SQACAToploadProgramming.HeavyCycle_option = 0;
		SQACAToploadProgramming.MediumCycle_extraWashTime = 0;
		SQACAToploadProgramming.MediumCycle_extraRinseTime = 0;
		SQACAToploadProgramming.HeavyCycle_extraWashTime = 0;
		SQACAToploadProgramming.HeavyCycle_extraRinseTime = 0;
		SQACAToploadProgramming.NormalCycle_washAgitateTime = 10;
		SQACAToploadProgramming.NormalCycle_rinseAgitateTime = 4;
		SQACAToploadProgramming.NormalCycle_extraRinseAgitateTime = 0;
		SQACAToploadProgramming.NormalCycle_finalSpinTime = 7;
		SQACAToploadProgramming.PermPressCycle_washAgitateTime = 10;
		SQACAToploadProgramming.PermPressCycle_extraRinseAgitateTime = 4;
		SQACAToploadProgramming.PermPressCycle_rinseAgitateTime = 0;
		SQACAToploadProgramming.PermPressCycle_finalSpinTime = 6;
		SQACAToploadProgramming.DelicateCycle_washAgitateTime = 10;
		SQACAToploadProgramming.DelicateCycle_rinseAgitateTime = 4;
		SQACAToploadProgramming.DelicateCycle_extraRinseAgitateTime = 0;
		SQACAToploadProgramming.DelicateCycle_finalSpinTime = 5;
		SQACAToploadProgramming.DefaultCycle = 5;							//Cycle value (Perm Press/Warm)
		SQACAToploadProgramming.DefaultCycleModifier = 0;					//Default: 0 = Light
		SQACAToploadProgramming.WarmRinse = 1;
		SQACAToploadProgramming.AudioSetting = 29;
		break;

		case PROGRAMMING_DATA_FRONTLOAD:
		halGetEeprom(MACHINE_SETUP_ADDR, 16, (u8*)&tmp);			//write machine setup string 16 Bytes to eeprom
		vendPrice = (tmp[9] * 256 + tmp[10]) / 100.0;				//Perm Press/Warm Cycle Vend Price (cents) default
		
		SQACAFrontloadProgramming.ProductByte[0];
		SQACAFrontloadProgramming.ProductByte[1];
		SQACAFrontloadProgramming.ProductByte[2];
		SQACAFrontloadProgramming.VendPrice1[0] = tmp[1];		//Normal/Hot Cycle Vend Price (cents)
		SQACAFrontloadProgramming.VendPrice1[1] = 0;
		SQACAFrontloadProgramming.VendPrice2[0] = tmp[3];		//Normal/Warm Cycle Vend Price (cents)
		SQACAFrontloadProgramming.VendPrice2[1] = 0;
		SQACAFrontloadProgramming.VendPrice3[0] = tmp[5];		//Normal/Cold Cycle Vend Price (cents)
		SQACAFrontloadProgramming.VendPrice3[1] = 0;
		SQACAFrontloadProgramming.VendPrice4[0] = tmp[7];		//Perm Press/Hot Cycle Vend Price (cents)
		SQACAFrontloadProgramming.VendPrice4[1] = 0;
		SQACAFrontloadProgramming.VendPrice5[0] = tmp[9];		//Perm Press Cycle Vend Price (cents)
		SQACAFrontloadProgramming.VendPrice5[1] = 0;
		SQACAFrontloadProgramming.VendPrice6[0] = tmp[11];		//Perm Press/Cold Cycle Vend Price (cents)
		SQACAFrontloadProgramming.VendPrice6[1] = 0;
		SQACAFrontloadProgramming.VendPrice7[0] = tmp[13];		//Delicate/Hot Cycle Vend Price (cents)
		SQACAFrontloadProgramming.VendPrice7[1] = 0;
		SQACAFrontloadProgramming.VendPrice8[0] = tmp[15];		//Delicate/Warm Cycle Vend Price (cents)
		SQACAFrontloadProgramming.VendPrice8[1] = 0;
		SQACAFrontloadProgramming.VendPrice9[0] = tmp[17];		//Delicate/Cold Cycle Vend Price (cents)
		SQACAFrontloadProgramming.VendPrice9[1] = 0;
		SQACAFrontloadProgramming.MediumCycle_VendPrice[0] = tmp[19];		//Cycle Modifier Key 1 Vend Price (Medium/B/Extra Wash)
		SQACAFrontloadProgramming.MediumCycle_VendPrice[1] = 0;		//
		SQACAFrontloadProgramming.HeavyCycle_VendPrice[0] = 50;		//Cycle Modifier Key 2 Vend Price (Heavy/C/Extra Rinse)
		SQACAFrontloadProgramming.HeavyCycle_VendPrice[1] = 0;
		SQACAFrontloadProgramming.MediumCycle_option =  0x02;
		SQACAFrontloadProgramming.HeavyCycle_option =  0x03;
		SQACAFrontloadProgramming.MediumCycle_extraWashTime = 2;
		SQACAFrontloadProgramming.MediumCycle_extraRinseTime = 1;
		SQACAFrontloadProgramming.HeavyCycle_extraWashTime = 4;
		SQACAFrontloadProgramming.HeavyCycle_extraRinseTime = 2;
		switch( SQACAFrontloadProgramming.ProductByte[2] )
		{
			//case A4:
			//	SQACAFrontloadProgramming.DefaultCycle = 5;
			//break;
			//case A3:
			//	SQACAFrontloadProgramming.DefaultCycle = 4;
			//break;
			//case A6:
			//	SQACAFrontloadProgramming.DefaultCycle = 4;
			//break;
			//case A2:
			//	SQACAFrontloadProgramming.DefaultCycle = 3;
			//break;
			//case A1:
			//	SQACAFrontloadProgramming.DefaultCycle = 4;
			//break;
		}
		//Cycle value (Perm Press/Warm)
		SQACAFrontloadProgramming.DefaultCycleModifier = 0;
		SQACAFrontloadProgramming.AudioSetting = 29;						//enable audio
		break;

		case PROGRAMMING_DATA_DRYER:
		halGetEeprom(MACHINE_SETUP_ADDR, 17, (u8*)&tmp);			//write machine setup string 17 Bytes for dryer to eeprom PPOS170601
		
		//vendPrice = (tmp[1] * 256 + tmp[2]) / 100.0;
		
		SQACADryerProgramming.ProductByte[0];
		SQACADryerProgramming.ProductByte[1];
		SQACADryerProgramming.ProductByte[2];
		SQACADryerProgramming.HeatVendPrice1[0] = tmp[1];			//Med Temp Cycle Vend Price (cents)
		SQACADryerProgramming.HeatVendPrice1[1] = 0;
		SQACADryerProgramming.HeatVendPrice2[0] = tmp[3];			//High Temp Cycle Vend Price (cents)
		SQACADryerProgramming.HeatVendPrice2[1] = 0;
		SQACADryerProgramming.HeatVendPrice3[0] = tmp[5];			//Low Temp Cycle Vend Price (cents)
		SQACADryerProgramming.HeatVendPrice3[1] = 0;
		SQACADryerProgramming.HeatVendPrice4[0] = tmp[7];			//Delicates Cycle Vend Price (cents)
		SQACADryerProgramming.HeatVendPrice4[1] = 0;
		SQACADryerProgramming.NoHeatVendPrice[0] = tmp[9];			//No Heat Cycle Vend Price (cents)
		SQACADryerProgramming.NoHeatVendPrice[1] = 0;
		SQACADryerProgramming.PaymSTopoffOn = tmp[15];				//Top Off Enabled, default=1
		SQACADryerProgramming.PaymSTopoffPrice[0] = tmp[11];		//Top Off Price (cents)
		SQACADryerProgramming.PaymSTopoffPrice[1] = 0;
		SQACADryerProgramming.PaymSTopoffMinutes = tmp[13];			//Top Off Time (minutes)
		SQACADryerProgramming.PaymSTopoffSeconds = 38;
		SQACADryerProgramming.Coin1TopoffMinutes = 5;
		SQACADryerProgramming.Coin1TopoffSeconds = 38;
		SQACADryerProgramming.Coin2TopoffMinutes = 22;
		SQACADryerProgramming.Coin2TopoffSeconds = 30;
		SQACADryerProgramming.HeatCycleMinutes = 45; 		
		SQACADryerProgramming.HeatCycleSeconds = 0;
		SQACADryerProgramming.NoHeatCycleMinutes = 45; 		
		SQACADryerProgramming.NoHeatCycleSeconds = 0;
		SQACADryerProgramming.HighCoolDownTime = 3;
		SQACADryerProgramming.MediumCoolDownTime = 3;
		SQACADryerProgramming.LowCoolDownTime = 3;
		SQACADryerProgramming.DelicateCoolDownTime = 3;
		SQACADryerProgramming.HighTempSetting = 160;
		SQACADryerProgramming.MediumTempSetting = 155;
		SQACADryerProgramming.DelicateTempSetting = 145;
		SQACADryerProgramming.DelicateTempSetting = 115;
		SQACADryerProgramming.DefaultCycle = 4;								//Default: Medium (4)
		SQACADryerProgramming.AudioSetting = 29;							//enable audio
		SQACADryerProgramming.AudioEnable1 = 0;
		SQACADryerProgramming.AudioEnable2 = 0;
		SQACADryerProgramming.DisplaySetting1 = 0;
		SQACADryerProgramming.DisplaySetting2 = 1;
		break;
	}
}


/**
	@brief Machine Status Communication
	The Payment System must initiate a Machine Status Communication Sequence a minimum of once every 500 msec, and a
	maximum of once every 250 msec, so that critical machine status information can be passed to the Payment System.
	Payment System Driven Vend Prices are to be used, and the final packet in the sequence will be the Vend Price Packet from the Payment System to
	the Machine Control. This packet sends the Payment Systems current vend price to the Machine Control. This vend
	price will be shown on the display, and used in a vending sequence. The Payment System will initiate the communication sequence by sending the Status Request Command Packet.
	Assuming the control received the packet correctly, it will respond with an ACK, followed by the Machine Status
	Packet. Assuming the Payment System received the packet correctly, it will then respond with an ACK, and then with the
	Vend Price Packet. The Machine Control will respond with an ACK, and the Payment System and Machine Control will terminate the
	communication sequence. The Machine Control will accept the vend price passed in this packet as the current vend
	price to be written to the display, and used in a vending transaction.
	The Machine Status Communication Sequence in the Payment System Driven Vend Price option must be completed as shown.
	PS: <0x70 – Status Request> <ACK> <0x72 – Vend Price>
	MC: <ACK> <0x71 – Machine Status> <ACK>

	@ingroup pposreader
*/
bool SQACAMachineStatusSequence(void)
{
	//ACA Status Request Command Packet.
	u8 temp[2] = {0};
	
	if( SQACAMachineStatus.MachineType[1] == ACA_SERIES)
	{
		temp[0] = ACA_STATUS_REQUEST_BYTES;			//Number of Data Bytes = 1
		temp[1] = ACA_STATUS_REQUEST_PACKET;		//STATUS REQUEST PACKET 0x70
	}
	
	if( sendSQDataPacket(temp) )
	{
		if ( getSQACAStatusData() )
		{
			if (ReaderStateFlag.ReaderSetup == READER_SETUP_DONE)
			{
				_delay_ms(3);	 //wait for at least 1ms to allow machine time to process
				sendSQACAVendPrice();
			}
			return true;
		}
	}
	return false;
}

/**
   The Machine Status Packet must be performed every 250msec to 500msec during operation.
*/
bool getSQACAStatusData(void)
{
	if( SQACAMachineStatus.MachineType[1] == ACA_SERIES)
	{
		u8 buf[ACA_STATUS_RESPONSE_BYTES] = {0};
		
		buf[0] = ACA_STATUS_RESPONSE_BYTES;
		if( getSQDataPacket(buf) && buf[0] == ACA_STATUS_RESPONSE_PACKET)
		{
			SQACAMachineStatus.MachineType[0] 	= (buf[1] + PROGRAMMING_DATA);
			
			SQACAMachineStatus.CycleType	   			= buf[2];
			SQACAMachineStatus.CycleModifier			= buf[3];
			SQACAMachineStatus.CmdToReader	 			= buf[4];
			SQACAMachineStatus.MachineStatus[0]			= buf[5];
			SQACAMachineStatus.MachineStatus[1]			= buf[6];
			SQACAMachineStatus.unused1[0] 				= buf[7];
			SQACAMachineStatus.unused1[1] 				= buf[8];
			SQACAMachineStatus.RemainingCycleMinutes	= buf[9];
			SQACAMachineStatus.RemainingCycleSeconds	= buf[10];
			SQACAMachineStatus.RemainingVend[0]			= buf[11];
			SQACAMachineStatus.RemainingVend[1]			= buf[12];
			SQACAMachineStatus.VendenteredTotal[0]		= buf[13];
			SQACAMachineStatus.VendenteredTotal[1]		= buf[14];
			SQACAMachineStatus.VendenteredforCycleModifier[0]		= buf[15];
			SQACAMachineStatus.VendenteredforCycleModifier[1]		= buf[16];
			SQACAMachineStatus.VendenteredforTopOff[0]	= buf[17];
			SQACAMachineStatus.VendenteredforTopOff[1]	= buf[18];
			SQACAMachineStatus.NumberofCoins1			= buf[19];
			SQACAMachineStatus.NumberofCoins2			= buf[20];
			SQACAMachineStatus.KeypadData[0]			= buf[21];
			SQACAMachineStatus.KeypadData[1]  			= buf[22];
			SQACAMachineStatus.CurrentKey[0]			= buf[23];
			SQACAMachineStatus.CurrentKey[1]			= buf[24];
			SQACAMachineStatus.unused2					= buf[25];
			
			return true;
		}
	}
	
	return false;
}

/**
   @brief Vend Price Packet is sent to the Machine Control
   The Payment System sends the Machine Control the active vend price for a machine cycle.
*/
bool sendSQACAVendPrice(void)
{
	u8 temp[11]= {0};
	
	if( SQACAMachineStatus.MachineType[1] == ACA_SERIES)
	{
		switch( deviceStatus.deviceType[0] )
		{
			case PROGRAMMING_DATA_TOPLOAD:
			temp[0] = ACA_VEND_PRICE_BYTES;
			temp[1] = ACA_VEND_PRICE_PACKET;									//Vend Price Packet 0x72
			
			switch( SQACAMachineStatus.CycleType )								//Cycle Value(Delicate...) 1-9
			{
				case 9:
				temp[2] = SQACAToploadProgramming.VendPrice9[0];
				temp[3] = SQACAToploadProgramming.VendPrice9[1];
				break;
				case 8:
				temp[2] = SQACAToploadProgramming.VendPrice8[0];
				temp[3] = SQACAToploadProgramming.VendPrice8[1];
				break;
				case 7:
				temp[2] = SQACAToploadProgramming.VendPrice7[0];
				temp[3] = SQACAToploadProgramming.VendPrice7[1];
				break;
				case 6:
				temp[2] = SQACAToploadProgramming.VendPrice6[0];
				temp[3] = SQACAToploadProgramming.VendPrice6[1];
				break;
				case 5:
				temp[2] = SQACAToploadProgramming.VendPrice5[0];
				temp[3] = SQACAToploadProgramming.VendPrice5[1];
				break;
				case 4:
				temp[2] = SQACAToploadProgramming.VendPrice4[0];
				temp[3] = SQACAToploadProgramming.VendPrice4[1];
				break;
				case 3:
				temp[2] = SQACAToploadProgramming.VendPrice3[0];
				temp[3] = SQACAToploadProgramming.VendPrice3[1];
				break;
				case 2:
				temp[2] = SQACAToploadProgramming.VendPrice2[0];
				temp[3] = SQACAToploadProgramming.VendPrice2[1];
				break;
				case 1:
				temp[2] = SQACAToploadProgramming.VendPrice1[0];
				temp[3] = SQACAToploadProgramming.VendPrice1[1];
				break;
			}
						
			switch( SQACAMachineStatus.CycleModifier )								//Cycle Modifier A4 Default: 0 = Light, 1 = Medium, 2 = Heavy
			{
				case 3:
				temp[4] = 0;														//Washer only: Cycle Modifier = 3 (Small Load)
				temp[5] = 0;
				break;
				case 2:																//Heavy
				temp[4] = SQACAToploadProgramming.HeavyCycle_VendPrice[0];			//Washer only: Cycle Modifier Key 2 Vend Price (cents)
				temp[5] = SQACAToploadProgramming.HeavyCycle_VendPrice[1];			//Default: 25 (A2) 50 (A4/A3)
				break;
				case 1:																//Medium
				temp[4] = SQACAToploadProgramming.MediumCycle_VendPrice[0];			//Washer only: Cycle Modifier Key 1 Vend Price (cents)
				temp[5] = SQACAToploadProgramming.MediumCycle_VendPrice[1];			//Default: 25 (All Controls)
				break;
				case 0:																//Light
				temp[4] = 0;														//Washer only: Default Cycle Modifier = 0 (Light)
				temp[5] = 0;
				break;
			}
			
			temp[6] = 0;
			temp[7] = 0;															//Dryer only
			temp[8] = 0;															//Top-Off Time in Minutes (Dryer)
			temp[9] = 0;
			break;
			case PROGRAMMING_DATA_FRONTLOAD:
			temp[0] = ACA_VEND_PRICE_BYTES;
			temp[1] = ACA_VEND_PRICE_PACKET;
			switch( SQACAMachineStatus.CycleType )								//Cycle Value 1-9
			{
				case 9:
				temp[2] = SQACAFrontloadProgramming.VendPrice9[0];
				temp[3] = SQACAFrontloadProgramming.VendPrice9[1];
				break;
				case 8:
				temp[2] = SQACAFrontloadProgramming.VendPrice8[0];
				temp[3] = SQACAFrontloadProgramming.VendPrice8[1];
				break;
				case 7:
				temp[2] = SQACAFrontloadProgramming.VendPrice7[0];
				temp[3] = SQACAFrontloadProgramming.VendPrice7[1];
				break;
				case 6:
				temp[2] = SQACAFrontloadProgramming.VendPrice6[0];
				temp[3] = SQACAFrontloadProgramming.VendPrice6[1];
				break;
				case 5:
				temp[2] = SQACAFrontloadProgramming.VendPrice5[0];
				temp[3] = SQACAFrontloadProgramming.VendPrice5[1];
				break;
				case 4:
				temp[2] = SQACAFrontloadProgramming.VendPrice4[0];
				temp[3] = SQACAFrontloadProgramming.VendPrice4[1];
				break;
				case 3:
				temp[2] = SQACAFrontloadProgramming.VendPrice3[0];
				temp[3] = SQACAFrontloadProgramming.VendPrice3[1];
				break;
				case 2:
				temp[2] = SQACAFrontloadProgramming.VendPrice2[0];
				temp[3] = SQACAFrontloadProgramming.VendPrice2[1];
				break;
				case 1:
				temp[2] = SQACAFrontloadProgramming.VendPrice1[0];
				temp[3] = SQACAFrontloadProgramming.VendPrice1[1];
				break;
			}
			temp[4] = SQACAFrontloadProgramming.MediumCycle_VendPrice[0];				//Washer only: Cycle Modifier Key 1 Vend Price (cents)
			temp[5] = SQACAFrontloadProgramming.MediumCycle_VendPrice[1];				//(0x00 for dryers)
			temp[4] = SQACAFrontloadProgramming.HeavyCycle_VendPrice[0];				//Washer only: Cycle Modifier Key 2 Vend Price (cents)
			temp[5] = SQACAFrontloadProgramming.HeavyCycle_VendPrice[1];
			
			temp[6] = 0;
			temp[7] = 0;															//Dryer only
			temp[8] = 0;															//Top-Off Time in Minutes (Dryer)
			temp[9] = 0;															//Top-Off time seconds, dryer only															//Top-Off Seconds (Dryer)
			
			break;
			case PROGRAMMING_DATA_DRYER:											//Program Packet ID Dryer 0x29
			temp[0] = ACA_VEND_PRICE_BYTES;
			temp[1] = ACA_VEND_PRICE_PACKET;
			switch( SQACAMachineStatus.CycleType )									//Cycle Definitions, Cycle Value 1-5
			{
				case 5:																//Delicates Cycle
				temp[2] = SQACADryerProgramming.HeatVendPrice4[0];
				temp[3] = SQACADryerProgramming.HeatVendPrice4[1];
				break;
				case 4:																//default, Medium Temperature Cycle
				temp[2] = SQACADryerProgramming.HeatVendPrice1[0];
				temp[3] = SQACADryerProgramming.HeatVendPrice1[1];
				break;
				case 3:																//No Heat Cycle Vend Price (cents)
				temp[2] = SQACADryerProgramming.NoHeatVendPrice[0];
				temp[3] = SQACADryerProgramming.NoHeatVendPrice[1];
				case 2:																//Low Temperature Cycle
				temp[2] = SQACADryerProgramming.HeatVendPrice3[0];
				temp[3] = SQACADryerProgramming.HeatVendPrice3[1];
				break;
				case 1:																//High Temp Cycle Vend Price (cents)
				temp[2] = SQACADryerProgramming.HeatVendPrice2[0];
				temp[3] = SQACADryerProgramming.HeatVendPrice2[1];
				break;
				
			}
			
			temp[4] = 0;															//Washer only
			temp[5] = 0;
			temp[6] = SQACADryerProgramming.PaymSTopoffPrice[0];					//Dryer only - TopOff Price
			temp[7] = SQACADryerProgramming.PaymSTopoffPrice[1];					//Dryer only
			temp[8] = SQACADryerProgramming.PaymSTopoffMinutes;						//Top-Off Time in Minutes (Dryer)
			temp[9] = 0;															//Top-Off time seconds, dryer only															//Top-Off Seconds (Dryer)
			
			break;
		}
	}
	return ( sendSQDataPacket(temp) );
}

/** @brief The Payment System must perform an Initialization Communication sequence ten seconds after power-up.
 
   If the packet is received properly, the Machine Control will respond with an ACK, and then return with its Machine Control Initialization Packet.
   
 */
bool IsACA(void)
{
	//Initialization Packet = 0x73
	u8 temp[10] = {0x09, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00};
	//Additional Vend Mode Not Supported
	temp[0] = ACA_INIT_REQUEST_BYTES;			//Number of Data Bytes = 9
	temp[1] = ACA_INIT_REQUEST_PACKET;			//Initialization Packet 0x73
	temp[5] = __APP_REVISION__;
	temp[6] = __APP_MINOR__;
	temp[7] = READER_CONTROL_VEND_PRICE;
	temp[8] = VEND_DISPLAY_ENABLE;
	temp[9] = ACA_BAUD_9600;

	if( sendSQDataPacket(temp) )				//send Initialization Packet 0x73
	{
		if ( getSQACAInitData() )				//receive Initialization Packet 0x74
		{
			return true;
		}
	}
	return false;

}

/**
	@brief MACHINE CONTROL INITIALIZATION PACKET
	The Payment System must perform an Initialization Communication sequence ten seconds after power-up.
	The Payment System must honor this request(Initialization Command = 0x73)when it is received.
	If the Vend Price Option Byte contains an invalid value, or a value which does not match that already initialized into the
	control, the control will respond to this entire packet with an <INV> (0x09). If the packet is received properly, the
	Machine Control will respond with an ACK, and then return with its Machine Control Initialization Packet(Initialization Response = 0x74).

	@ingroup pposreader
*/
bool getSQACAInitData()
{
	u8 buf[ACA_INIT_RESPONSE_BYTES] = {0};
	buf[0] = ACA_INIT_RESPONSE_BYTES;

	if ( getSQDataPacket(buf) )					//receive Initialization Packet 0x74
	{
		//do something with the returned init data

		SQACAToploadProgramming.ProductByte[0] = buf[1];
		SQACAToploadProgramming.ProductByte[1] = buf[2];
		SQACAToploadProgramming.ProductByte[2] = buf[3];
		SQACAToploadProgramming.ControlConfig  = buf[6]; //MACHINE CONFIGURATION BYTE
		SQACAToploadProgramming.ControlSN     = buf[16]; // Machine ID Serial Number

		return true;
	}
	return false;
}

/** @brief Sends an ASCIIZ string for transmitting via UART.
 
   @param ucDataBuf to be placed in the serial output buffer and
   then transmitted over the serial port.
 */
bool sendSQDataPacket(u8 *buf)
{
	u8 n, numberOfBytes, BCC, i;
	//static u8 ucTimerCount;

	for(n = 0; n < NUMRETRIES; n++)
	{
		BCC = STX;
		init_rx_buf();
		numberOfBytes = (buf[0] + 1);	//number of bytes sending
		serial_putchar(STX);    // Send STX, start byte

		for (i = 0; i < numberOfBytes; i++)
		{
			serial_putchar(buf[i]);
			BCC = (BCC ^ buf[i]);
		}

		serial_putchar(BCC);    // Send BCC, error checking byte
		//start timer 350ms
		//wait for response

		ucTimerCount = 0;
		sei();
		StartTimerZero();
		while(ucTimerCount < 70)
		{
			if(rx_buf_count())
			break;
			else if( mac_event_pending() ){
				macTask();  //timer has timed out handle timer task.
			}//to cut down on missed calls while waiting for machine to respond
		}
		StopTimerZero();

		if(ucTimerCount < 70)	//a byte has been received.
		{
			i = serial_getchar();
			if(i == ACK)
			return 1;	//Command received successfully
			else if(i == INV)
			return 0;	//Invalid command sent
		}//if not time out leave.
		_delay_ms(3);
	}// Retries
	return false;
}

/**
   @brief Returns a data packet from the serial port.  
   @return Character from serial buffer.
*/
bool getSQDataPacket(u8 *buf)
{
	u8 i,n,BCC;

	//get machine data
	i = serial_getchar();
	if( i == STX )//start byte detected
	{
		i = serial_getchar(); // get data field size.
		BCC = (i ^ STX);

		if ( i > buf[0] )	//protect against buffer overrun.
		{
			return false;
		}

		for(n = 0; n < i; n++)
		{
			buf[n]= serial_getchar();
			BCC = (BCC ^ buf[n]);
		}

		i = serial_getchar();	// get BCC
		if(BCC == i)
		{
			serial_putchar(ACK);
			return 1;
		}
		else
		{
			serial_putchar(NAK);
		}
	}
	return 0;
}

/**
   Callback function, called to examine machine status to determine if the machine is in MACHINE_READY_MODE(0x01) and MIMIC_QUANTUM_LOCK_ACTIVE(0x20)

   @param waitSQStartKey True if Machine Status MACHINE_READY_MODE and MIMIC_QUANTUM_LOCK_ACTIVE bit is set,
   Machine Status = Ready Mode = 0x01
   Machine Status Secondary Modes = 0x20 (Mimic Quantum Lock Cycle Selections Active)
   or false if bits are not set.
   The MACHINE_READY_MODE bit and the MIMIC_QUANTUM_LOCK_ACTIVE bit is set when the machine is not running a cycle, and is waiting for a vend to be
   performed. In this mode of operation, the display shows the currently selected cycle, the full current vend price for that cycle.
   If the full vend amount is entered, the control will go to Start Mode.
   MachineType[0] = 33,34,29 MachineType[1] = 3 = ACA
*/
bool waitSQStartKey(void)
{	
	//if( (SQACAMachineStatus.MachineStatus[0] & MACHINE_READY_MODE) ||
	//((SQACAMachineStatus.MachineStatus[0] == MACHINE_RUN_MODE) && (SQACAMachineStatus.MachineType[0] == PROGRAMMING_DATA_DRYER)) )
	
	if( (SQACAMachineStatus.MachineStatus[0] & MACHINE_READY_MODE) && (SQACAMachineStatus.MachineStatus[1] == MIMIC_QUANTUM_LOCK_ACTIVE) )
	
	{
		return true;
	}
	
	return false;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
bool isSQCycleRunning(void)
{
	//return ( (SQACAMachineStatus.MachineStatus[0] == MACHINE_RUN_MODE) || SQACAMachineStatus.MachineStatus[1] == MACHINE_RUN_MODE ); PPOS
	return ( (SQACAMachineStatus.MachineStatus[0] == MACHINE_RUN_MODE) );
}


						
/** \brief Mimic Quantum Vending Interface Packet
	The PS sends the Mimic Quantum Vending Interface Packet since the user has to press the
	Start Pad before having the account charged. The Start Pad will now be flashing one second on, one second off for
	30 seconds. Mimic Quantum Lock Cycle Selections will now be seen in the next Machine Status packet.
*/
bool SQACAMimicQuantumSequence(u8 cardType)
{
	//If True, the Start Pad will now be flashing one second on, one second off for 30 seconds
	if( (cardType == REGULAR_VEND) && sendSQACAMimicQuantumVending() )
	{		
		return true;
	}
	return false;
}

/**
@brief MIMIC QUANTUM VENDING INTERFACE
   The Payment System should send the Mimic Quantum Vending Interface Packet if they wish to have the user press the
   Start Pad before having their account charged. If the Mimic Quantum Vending Interface – Lock Cycle Selections is
   active, the next Start Pad press will lock cycle selections for 10 seconds. If the full vend amount is received within the
   10 seconds, the control will bypass Start Mode and enter Run Mode.
   The Payment System initiates the Mimic Quantum Vending Interface (user is ready to purchase a cycle).
   The machine control will respond with an ACK. The Start Pad will now be flashing one second on, one second off for
   30 seconds. If the “start mode” audio is enabled, the control will beep one second on, one second off for 10 seconds.
   Mimic Quantum Lock Cycle Selections will now be seen in the next Machine Status packet.

   @ingroup pposreader
*/
bool sendSQACAMimicQuantumVending(void)
{
	u8 temp[4] = {0};	
	
	temp[0] = ACA_A4_MIMIC_QUANTUM_BYTES;						//Number of bytes in Data Field = 3
	temp[1] = ACA_A4_MIMIC_QUANTUM_PACKET;						//Mimic Quantum Interface = 0x64
	temp[2] = ACA_A4_STARTPAD_FLASHING;							//Duration of Start Pad Flashing = 30
	temp[3] = ACA_A4_LOCKCYCLE_ENABLED;							//Lock Cycle Selection enabled = 1
	
	return ( sendSQDataPacket(temp) );
}

/**
@brief  Sends a vending transaction to the Machine Control.
   Once this vend is received by the machine control, the Start Pad LED will no longer flash. As long as the door has been
   closed the entire time and the 10 seconds has not expired, the control will begin the purchased cycle. If either of these
   are not true, the control will enter Start Mode which will prompt the user to press Start again.
   If the packet is received without error, the Machine Control will respond with an <ACK>. The updated vending
   information internal to the control will be updated in the Machine Status Packet in the next Machine Status
   communication.
   If the vend could not be performed as presented in the Vending Payment Packet, as would be the case when the UnAvailable bit is set,
   the Machine Control will respond with an <INV>.
*/
bool sendSQACAVendingTransaction(void)
{
	u8 temp[7] = {0};

	temp[0] = ACA_VENDING_PAYMENT_BYTES;							//Number of bytes in Data Field = 6
	temp[1] = ACA_VENDING_PAYMENT_PACKET;							//Vending Payment = 0x6A
	temp[2] = SQACAMachineStatus.RemainingVend[0];					//Vend Amount
	temp[3] = SQACAMachineStatus.RemainingVend[1];
	temp[4] = SQACAMachineStatus.RemainingVend[0];
	temp[5] = SQACAMachineStatus.RemainingVend[1];

	return ( sendSQDataPacket(temp) );
}

/** @brief Sends a vending transaction to the Machine Control
 
   This function sends this packet to the Machine Control to perform a Top-Off dryer vending transaction.
*/
bool sendSQACATopOffVendingTransaction(void)
{
	u8 temp[7] = {0};

	temp[0] = ACA_VENDING_PAYMENT_BYTES;							//Number of bytes in Data Field = 6
	temp[1] = ACA_VENDING_PAYMENT_PACKET;							//Vending Payment = 0x6A
	temp[2] = SQACADryerProgramming.PaymSTopoffPrice[0];					//TopOff Vend Amount
	temp[3] = SQACADryerProgramming.PaymSTopoffPrice[1];
	temp[4] = SQACADryerProgramming.PaymSTopoffPrice[0];
	temp[5] = SQACADryerProgramming.PaymSTopoffPrice[1];

	return ( sendSQDataPacket(temp) );
}

bool sendSQAddTime(void)
{
	u8 temp[2] = {0};

	temp[0] = ADD_TIME_PACKET_SIZE;
	temp[1] = ADD_TIME_COMMAND;

	return ( sendSQDataPacket(temp) );
}

/** @brief Card Error Communication 
   This function determines that there has been an error condition when reading a card, it may inform the
   user of this error by sending the error code to the Machine Control, so that the Machine Control can display the error
   code. These errors may be caused by inability to read the card due to a damaged card or reader, or by invalid data on
   the card. The Payment System will send the Card Error Packet, and the Machine Control will display the appropriate
   error message.
*/
bool sendSQCardErrorCode(u8 cardErrorCode)
{
	u8 temp[3] = {0};

	temp[0] = CARD_ERROR_PACKET_SIZE;
	temp[1] = CARD_ERROR_PACKET;
	temp[2] = cardErrorCode;

	return ( sendSQDataPacket(temp) );
}

/** @brief Audio Beep Request packet.  
 
   This function turns the control audio signal on or off.
*/
bool sendSQAudioBeepRequest(u8 beepLength)
{
	u8 temp[3] = {0};

	temp[0] = AUDIO_BEEP_REQUEST_SIZE;
	temp[1] = AUDIO_BEEP_REQUEST;
	temp[2] = beepLength;

	return ( sendSQDataPacket(temp) );
}


/** @brief Payment System Display Request. 
   Function to control the Machine Control display.  This function will be used to send text to the MDC 6 position 7 segment display.
   Led1 to led6 are limited to values defined in "display.h"

   Parameter:  u8 led1, u8 led2,u8 led3, u8 led4, u8 led5, u8 led6, u8 duration
 */
bool sendSQDisplayCommand(u8 led1, u8 led2,u8 led3, u8 led4, u8 led5, u8 led6, u8 duration)
{
	u8 temp[9]={0};

	temp[0] = DISPLAY_REQUEST_BYTES;	//Number of actual data bytes to be transferred to the ACA including command data exclude bbc byte
	temp[1] = DISPLAY_REQUEST;		//ACA command to be requested
	temp[2] = led1;					//Left most digit display character 6
	temp[3] = led2;					//Display character 5
	temp[4] = led3;					//Display character 4
	temp[5] = led4;					//Display character 3
	temp[6] = led5;
	temp[7] = led6;
	temp[8] = duration;				//Duration of display in seconds

	return ( sendSQDataPacket(temp) );
}


/** @brief Payment System Display Request. 
   This function will display the current version of reader firmware
   on the ACA display for a minimum of 2 seconds.

   Parameter:  u8 led1, u8 led2,u8 led3, u8 led4, u8 led5, u8 led6, u8 duration
 */
bool displaySQReaderVersion(void)
{
	if( sendSQDisplayCommand(LED_r, msgNumber[__APP_MAJOR__],msgNumber[__APP_MINOR__],msgNumber[__APP_REVISION__], LED_BLANK,LED_BLANK,5) )
	{
		_delay_ms(2000);
		return true;
	}
	return false;
}

/** @brief Payment System Display Request. 
   
 */
bool displaySQCardError(void)
{
	if( sendSQDisplayCommand(LED_C,LED_A,LED_r,LED_d,LED_BLANK,LED_BLANK,4) )
	{
		_delay_ms(2000);
		if( sendSQDisplayCommand(LED_E, LED_r,LED_r,LED_BLANK,LED_BLANK,LED_BLANK,3) )
		_delay_ms(2000);

		return true;
	}
	return false;
}

/**	@brief Power-up Mode.
	The Payment System and Machine Control exchange critical information in the Initialization Communication sequence.
	Ten seconds after power-up he Payment System must perform an Initialization Communication sequence.
	The Payment System terminates the sequence by responding with an ACK and the Machine Status Communication sequences should now be performed regularly.

@ingroup pposreader
*/
void SQACAInitializationSequence(void)
{
	if( IsACA() )
	{
		//set ACA flag
		SQACAMachineStatus.MachineType[1] = ACA_SERIES;
	}
	
	_delay_ms(2);
	SQACAMachineStatusCommSequence();
	deviceStatus.deviceType[0] = SQACAMachineStatus.MachineType[0];
	deviceStatus.deviceType[1] = SQACAMachineStatus.MachineType[1];
}

bool isMachineCycleRunning()
{
	return (isSQCycleRunning() );
}

//Examine machine control with Machine Status
bool waitForMachineStartKey()
{
	return waitSQStartKey();
	_delay_ms(1000); //PPOS 170706 without this delay the Start button will not work	
}

/** @brief Machine Status completed and associated to the network.

   The Machine Status Communication Sequence is completed. The Machine Control will accept the vend price passed in this packet as the current vend
   price to be written to the display and used in a vending transaction. The ucDeviceStateFlag is set to be online.
 */
void SQACAMachineStatusCommSequence(void)
{
	
	if( SQACAMachineStatusSequence() )
	{
		//set machine status ok flag;
		ucDeviceStateFlag = MACHINE_STATUS_ON; //machine status has been done
	}
	
}

/** @brief Programming Data packet.
 
   When a valid Programming Command is sent into the Payment System, the reader will initiate a programming
   sequence by sending the Programming Data packet.
 */
bool getSQReaderMachineSetup(void)
{
	u8 temp[ACA_MAX_PACKET_SIZE] = {0};
	
	switch( deviceStatus.deviceType[0] )
	{
		case PROGRAMMING_DATA_TOPLOAD:
		temp[0] = QTL_PROGRAMMING_DATA_SIZE;					//50 bytes
		temp[1] = deviceStatus.deviceType[0];					//0x21 (TLW Prog)
		temp[2] = SQACAToploadProgramming.ProductByte[0];
		temp[3] = SQACAToploadProgramming.ProductByte[1];
		temp[4] = SQACAToploadProgramming.ProductByte[2];
		temp[5] = SQACAToploadProgramming.ProductByte[3];
		temp[6] = SQACAToploadProgramming.ProductByte[4];
		temp[7] = SQACAToploadProgramming.VendPrice1[0];
		temp[8] = SQACAToploadProgramming.VendPrice1[1];
		temp[9] = SQACAToploadProgramming.VendPrice2[0];
		temp[10] = SQACAToploadProgramming.VendPrice2[1];
		temp[11] = SQACAToploadProgramming.VendPrice3[0];
		temp[12] = SQACAToploadProgramming.VendPrice3[1];
		temp[13] = SQACAToploadProgramming.VendPrice4[0];
		temp[14] = SQACAToploadProgramming.VendPrice4[1];
		temp[15] = SQACAToploadProgramming.VendPrice5[0];
		temp[16] = SQACAToploadProgramming.VendPrice5[1];
		temp[17] = SQACAToploadProgramming.VendPrice6[0];
		temp[18] = SQACAToploadProgramming.VendPrice6[1];
		temp[19] = SQACAToploadProgramming.VendPrice7[0];
		temp[20] = SQACAToploadProgramming.VendPrice7[1];
		temp[21] = SQACAToploadProgramming.VendPrice8[0];
		temp[22] = SQACAToploadProgramming.VendPrice8[1];
		temp[23] = SQACAToploadProgramming.VendPrice9[0];
		temp[24] = SQACAToploadProgramming.VendPrice9[1];
		temp[25] = SQACAToploadProgramming.MediumCycle_VendPrice[0];
		temp[26] = SQACAToploadProgramming.MediumCycle_VendPrice[1];
		temp[27] = SQACAToploadProgramming.HeavyCycle_VendPrice[0];
		temp[28] = SQACAToploadProgramming.HeavyCycle_VendPrice[1];
		temp[29] = SQACAToploadProgramming.HeavyCycle_option;
		temp[30] = SQACAToploadProgramming.MediumCycle_option;
		temp[31] = SQACAToploadProgramming.MediumCycle_extraWashTime;
		temp[32] = SQACAToploadProgramming.MediumCycle_extraRinseTime;
		temp[33] = SQACAToploadProgramming.HeavyCycle_extraWashTime;
		temp[34] = SQACAToploadProgramming.HeavyCycle_extraRinseTime;
		temp[35] = SQACAToploadProgramming.NormalCycle_washAgitateTime;
		temp[36] = SQACAToploadProgramming.NormalCycle_rinseAgitateTime;
		temp[37] = SQACAToploadProgramming.NormalCycle_rinseAgitateTime;
		temp[38] = SQACAToploadProgramming.NormalCycle_finalSpinTime;
		temp[39] = SQACAToploadProgramming.PermPressCycle_washAgitateTime;
		temp[40] = SQACAToploadProgramming.PermPressCycle_extraRinseAgitateTime;
		temp[41] = SQACAToploadProgramming.PermPressCycle_rinseAgitateTime;
		temp[42] = SQACAToploadProgramming.PermPressCycle_finalSpinTime;
		temp[43] = SQACAToploadProgramming.DelicateCycle_washAgitateTime;
		temp[44] = SQACAToploadProgramming.DelicateCycle_extraRinseAgitateTime;
		temp[41] = SQACAToploadProgramming.DelicateCycle_rinseAgitateTime;
		temp[46] = SQACAToploadProgramming.DelicateCycle_finalSpinTime;
		temp[47] = SQACAToploadProgramming.DefaultCycle;
		temp[48] = SQACAToploadProgramming.DefaultCycleModifier;
		temp[49] = SQACAToploadProgramming.WarmRinse;
		temp[50] = SQACAToploadProgramming.AudioSetting;
		break;

		case PROGRAMMING_DATA_FRONTLOAD:
		temp[0] = QTL_PROGRAMMING_DATA_SIZE;					//37 bytes
		temp[1] = deviceStatus.deviceType[0];					//0x22 (FLW Prog)
		temp[2] = SQACAFrontloadProgramming.ProductByte[0];
		temp[3] = SQACAFrontloadProgramming.ProductByte[1];
		temp[4] = SQACAFrontloadProgramming.ProductByte[2];
		temp[5] = SQACAFrontloadProgramming.ProductByte[3];
		temp[6] = SQACAFrontloadProgramming.ProductByte[4];
		temp[7] = SQACAFrontloadProgramming.VendPrice1[0];
		temp[8] = SQACAFrontloadProgramming.VendPrice1[1];
		temp[9] = SQACAFrontloadProgramming.VendPrice2[0];
		temp[10] = SQACAFrontloadProgramming.VendPrice2[1];
		temp[11] = SQACAFrontloadProgramming.VendPrice3[0];
		temp[12] = SQACAFrontloadProgramming.VendPrice3[1];
		temp[13] = SQACAFrontloadProgramming.VendPrice4[0];
		temp[14] = SQACAFrontloadProgramming.VendPrice4[1];
		temp[15] = SQACAFrontloadProgramming.VendPrice5[0];
		temp[16] = SQACAFrontloadProgramming.VendPrice5[1];
		temp[17] = SQACAFrontloadProgramming.VendPrice6[0];
		temp[18] = SQACAFrontloadProgramming.VendPrice6[1];
		temp[19] = SQACAFrontloadProgramming.VendPrice7[0];
		temp[20] = SQACAFrontloadProgramming.VendPrice7[1];
		temp[21] = SQACAFrontloadProgramming.VendPrice8[0];
		temp[22] = SQACAFrontloadProgramming.VendPrice8[1];
		temp[23] = SQACAFrontloadProgramming.VendPrice9[0];
		temp[24] = SQACAFrontloadProgramming.VendPrice9[1];
		temp[25] = SQACAFrontloadProgramming.MediumCycle_VendPrice[0];
		temp[26] = SQACAFrontloadProgramming.MediumCycle_VendPrice[1];
		temp[27] = SQACAFrontloadProgramming.HeavyCycle_VendPrice[0];
		temp[28] = SQACAFrontloadProgramming.HeavyCycle_VendPrice[1];
		temp[29] = SQACAFrontloadProgramming.HeavyCycle_option;
		temp[30] = SQACAFrontloadProgramming.MediumCycle_option;
		temp[31] = SQACAFrontloadProgramming.MediumCycle_extraWashTime;
		temp[32] = SQACAFrontloadProgramming.MediumCycle_extraRinseTime;
		temp[33] = SQACAFrontloadProgramming.HeavyCycle_extraWashTime;
		temp[34] = SQACAFrontloadProgramming.HeavyCycle_extraRinseTime;
		temp[35] = SQACAFrontloadProgramming.DefaultCycle;
		temp[36] = SQACAFrontloadProgramming.DefaultCycleModifier;
		temp[37] = SQACAFrontloadProgramming.AudioSetting;
		break;

		case PROGRAMMING_DATA_DRYER:									//DRYER PROGRAMMING
		temp[0] = QDT_PROGRAMMING_DATA_SIZE;							//37 bytes
		temp[1] = deviceStatus.deviceType[0];							//0x29 for Dryer Prog
		temp[2] = SQACADryerProgramming.ProductByte[0];
		temp[3] = SQACADryerProgramming.ProductByte[1];
		temp[4] = SQACADryerProgramming.ProductByte[2];
		temp[5] = SQACADryerProgramming.ProductByte[3];
		temp[6] = SQACADryerProgramming.ProductByte[4];
		temp[7] = SQACADryerProgramming.HeatVendPrice1[0];				//Heat Vend Price
		temp[8] = SQACADryerProgramming.HeatVendPrice1[1];
		temp[9] = SQACADryerProgramming.NoHeatVendPrice[0];				//No Heat Vend Price
		temp[10] = SQACADryerProgramming.NoHeatVendPrice[1];
		temp[13] = SQACADryerProgramming.PaymSTopoffOn;					//Top-Off On/Off
		temp[11] = SQACADryerProgramming.PaymSTopoffPrice[0];			//Payment System Top-Off Vend Price
		temp[12] = SQACADryerProgramming.PaymSTopoffPrice[1];
		temp[14] = SQACADryerProgramming.PaymSTopoffMinutes;			//Payment System Top-Off Time
		temp[15] = SQACADryerProgramming.PaymSTopoffSeconds;
		temp[16] = SQACADryerProgramming.Coin1TopoffMinutes;			//Coin #1 Top-off Time
		temp[17] = SQACADryerProgramming.Coin1TopoffSeconds;
		temp[18] = SQACADryerProgramming.Coin2TopoffMinutes;			//Coin #2 Top-off Time
		temp[19] = SQACADryerProgramming.Coin2TopoffSeconds;
		temp[20] = SQACADryerProgramming.HeatCycleMinutes;				//Heat Cycle Time
		temp[21] = SQACADryerProgramming.HeatCycleSeconds;
		temp[22] = SQACADryerProgramming.NoHeatCycleMinutes;			//No Heat Cycle Time
		temp[23] = SQACADryerProgramming.NoHeatCycleSeconds;
		temp[24] = SQACADryerProgramming.HighCoolDownTime;				//High Temperature Cool Down Time (Minutes)
		temp[25] = SQACADryerProgramming.MediumCoolDownTime;			//Medium Temperature Cool Down Time (Minutes)
		temp[26] = SQACADryerProgramming.LowCoolDownTime;				//Low Temperature Cool Down Time (Minutes)
		temp[27] = SQACADryerProgramming.DelicateCoolDownTime;			//Delicate Temperature Cool Down Time (Minutes)
		temp[28] = SQACADryerProgramming.HighTempSetting;				//High Temperature
		temp[29] = SQACADryerProgramming.MediumTempSetting;
		temp[30] = SQACADryerProgramming.LowTempSetting;				//Low Temperature
		temp[31] = SQACADryerProgramming.DelicateTempSetting;
		temp[32] = SQACADryerProgramming.DefaultCycle;					//Default Cycle
		temp[33] = SQACADryerProgramming.AudioSetting;					//Audio
		temp[34] = SQACADryerProgramming.AudioEnable1;					//Anti-Wrinkle Audio Enable
		temp[35] = SQACADryerProgramming.AudioEnable2;					//Extended Tumble Audio Enable
		temp[36] = SQACADryerProgramming.DisplaySetting1;				//Fahrenheit / Celsius
		temp[37] = SQACADryerProgramming.DisplaySetting2;				//Minutes / Minutes & Seconds Display
		
		break;
	}
	return ( sendSQDataPacket(temp) );
}

/**
	@brief SHORT AUDIT DATA REQUEST PACKET
	An Audit Collection Command may be sent by the Payment System to retrieve limited Audit information from the
	Machine Control. When audit data is retrieved from the control, there will be no visible change to the display to
	indicate that an audit has taken place. Audit data is collected for each machine type.

	@ingroup pposreader
*/
bool SQACAAuditDataSequence(void)
{
	//ACA Short Audit Request Command Packet.
	u8 temp[2]={0};
	
	temp[0] = AUDIT_DATA_REQUEST_BYTES;	//Number of actual data bytes to be transferred to the MDC including command data exclude bbc byte
	temp[1] = AUDIT_DATA_REQUEST_PACKET;		//send SHORT AUDIT DATA REQUEST PACKET 0x30

	if( sendSQDataPacket(temp) )	//SHORT AUDIT DATA REQUEST PACKET 0x30
	{
		if( getSQACAAuditData() )		//receive SHORT AUDIT DATA PACKET
		{
			
		}
	}
	return false;
}

/**
	@brief SHORT AUDIT DATA PACKET
	See the following sections fordetails on the Audit Data for each machine type.
	
*/
bool getSQACAAuditData(void)
{
	u8 buf[ACA_AUDIT_RESPONSE_BYTES] = {0};
	buf[0] = ACA_AUDIT_RESPONSE_BYTES;											//Number of Data Bytes = 47

	if( getSQDataPacket(buf) )		//Audit Packet 0x31
	{

		SQACAAuditData.ProductByte[0]				= buf[1];			//Product Byte #1
		SQACAAuditData.ProductByte[1]				= buf[2];			//Product Byte #2
		SQACAAuditData.ProductByte[2]				= buf[3];			//Product Byte #3
		SQACAAuditData.ProductByte[3]				= buf[4];
		SQACAAuditData.ProductByte[4]				= buf[5];
		SQACAAuditData.ControlSN[0]					= buf[6];			//Control Serial Number
		SQACAAuditData.ControlSN[1]					= buf[7];
		SQACAAuditData.ControlSN[2]					= buf[8];
		SQACAAuditData.ControlSN[3]					= buf[9];
		SQACAAuditData.ControlSVN					= buf[10];			//Control Software Version
		SQACAAuditData.MachineCyclesCounter[0]		= buf[11];			//Machine Cycles Counter
		SQACAAuditData.MachineCyclesCounter[1]		= buf[12];
		SQACAAuditData.CycleModifierCounter[0]		= buf[13];			//Cycle Modifier Counter
		SQACAAuditData.CycleModifierCounter[1]		= buf[14];
		SQACAAuditData.AvgFillTime[0]				= buf[15];			//Average Fill Time
		SQACAAuditData.AvgFillTime[1]				= buf[16];
		SQACAAuditData.AvgDrainTime[0]				= buf[17];			//Average Drain Time
		SQACAAuditData.AvgDrainTime[1]				= buf[18];
		SQACAAuditData.VendedTotal[0]				= buf[19];			//Vended Total (all payment methods)
		SQACAAuditData.VendedTotal[1]				= buf[20];
		SQACAAuditData.VendedTotal[2]				= buf[21];
		SQACAAuditData.VendedTotal[3]				= buf[22];
		SQACAAuditData.VendedTotalBaseVendPrice[0]	= buf[23];			//Vended Total for Base Vend Price (all payment methods)
		SQACAAuditData.VendedTotalBaseVendPrice[1]	= buf[24];
		SQACAAuditData.VendedTotalBaseVendPrice[2]	= buf[25];
		SQACAAuditData.VendedTotalBaseVendPrice[3]	= buf[26];
		SQACAAuditData.VendedTotalCycleModifiers[0]	= buf[27];			//Vended Total for Cycle Modifiers (all payment methods)
		SQACAAuditData.VendedTotalCycleModifiers[1]	= buf[28];
		SQACAAuditData.VendedTotalCycleModifiers[2]	= buf[29];
		SQACAAuditData.VendedTotalCycleModifiers[3]	= buf[30];
		SQACAAuditData.VendedTotalPS[0]				= buf[31];			//Vended Total (via Payment System)
		SQACAAuditData.VendedTotalPS[1]				= buf[32];
		SQACAAuditData.VendedTotalPS[2]				= buf[33];
		SQACAAuditData.VendedTotalPS[3]				= buf[34];
		SQACAAuditData.VendedTotalPSBaseVendPrice[0]= buf[35];			//Vended Total for Base Vend Price (via Payment System)
		SQACAAuditData.VendedTotalPSBaseVendPrice[1]= buf[36];
		SQACAAuditData.VendedTotalPSBaseVendPrice[2]= buf[37];
		SQACAAuditData.VendedTotalPSBaseVendPrice[3]= buf[38];
		SQACAAuditData.VendedTotalPSCycleModifiers[0]	= buf[39];		//Vended Total for Cycle Modifiers (via Payment System)
		SQACAAuditData.VendedTotalPSCycleModifiers[1]	= buf[40];
		SQACAAuditData.VendedTotalPSCycleModifiers[2]	= buf[41];
		SQACAAuditData.VendedTotalPSCycleModifiers[3]	= buf[42];
		SQACAAuditData.VendedTotalPSDiscounted[0]	= buf[43];			//Discounted Vended Total (from PS)
		SQACAAuditData.VendedTotalPSDiscounted[1]	= buf[44];
		SQACAAuditData.VendedTotalPSDiscounted[2]	= buf[45];
		SQACAAuditData.VendedTotalPSDiscounted[3]	= buf[46];
		return true;

	}
	return false;
}
/** @} */
/** @} */
#endif