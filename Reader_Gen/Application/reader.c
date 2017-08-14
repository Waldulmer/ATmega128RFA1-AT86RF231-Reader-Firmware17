/*
* reader.c
*
*	This file contains GenReader related functionality, to be used across applications.
* Created: 5/28/2011 3:10:54 PM
*  Author: Paul Bastien
*	Copyright (C) 2011
*/

#include <stdlib.h>
#include "mac.h"
#include "reader.h"
#include "application.h"
#include "ha_timer.h"
#include "hal_avr.h"
#include "eeprom_map.h"
#include "system.h"

/**
@brief Initializes the reader on startup. A function to read data out of eeprom
and sets status flags for the reader and machine
READER_STATE_ADDR
MACHINE_TYPE_ADDR
stores the parameter for how many offline transactions are to save
and calls funtion getSQSetupData(), programming data for the machine
*/
void initReader(void)
{
	halGetEeprom(READER_STATE_ADDR,2,(u8*)&ReaderStateFlag);
	halGetEeprom(MACHINE_TYPE_ADDR, MACHINE_TYPE_SIZE,(u8*)&deviceStatus.deviceType);
	
	ReaderSetup.customerId = 0;
	ReaderSetup.manufacturerId = 1;
	ReaderSetup.locationId = 1;
	//ReaderSetup.MaxOfflineTransaction = MAX_NUM_OFFLINE_TRANS; // to become variable once setup is done.
	ReaderStateFlag.Busy = false;

	//check setup status then set reader state flag
	
	//Init machine variables
	if (ReaderStateFlag.ReaderSetup == READER_SETUP_DONE)
	{
		//a valid reader ID has been detected. check for valid setup
		getSQSetupData();
		ReaderStateFlag.ValidateSetup = VALIDATE_READER_SETUP;
		halGetEeprom(MAXNUM_OFFLINE_ADDR,1,(u8*)&ReaderSetup.maxOfflineTransaction);
		if (ReaderSetup.maxOfflineTransaction)
		{
			ReaderStateFlag.EnableOfflineTransaction = true;
			halGetEeprom(OFFLINE_TRANSACTION_RECORD_ADDR, 1, &ReaderSetup.numOfSavedTransactions);
			if (ReaderSetup.numOfSavedTransactions == 0xff)
			{
				ReaderSetup.numOfSavedTransactions = 0;
				halPutEeprom(OFFLINE_TRANSACTION_RECORD_ADDR, 1, &ReaderSetup.numOfSavedTransactions);
			}
			
		}
		//clear buffers
		memset(ReaderSetup.machineLabel,0,MACHINE_LABEL_SIZE+1);
		memset(ReaderSetup.machineDescription,0,MACHINE_NAME_SIZE+1);
		halGetEeprom(MACHINE_LABEL_ADDR, MACHINE_LABEL_SIZE, (u8*)&ReaderSetup.machineLabel);
		halGetEeprom(MACHINE_DESCRIPTION_ADDR, MACHINE_NAME_SIZE, (u8*)&ReaderSetup.machineDescription);
	}
	else
	{
		//SetDefaultMachineSetup();
	}

	
}

/**
@brief  Function to store data into eeprom.
structTransaction OfflineTransaction defines the data container.

*/
u8 storeOfflineTransaction(u32 cardNum)
{
	u8 return_code = 1;
	// ReaderSetup.numOfSavedTransactions number of off line transaction records stored in EEPROM
	u8 addr[2];
	u16 temp;
	structTransaction OfflineTransaction;
	
	temp  = (u16)OFFLINE_TRANSACTION_ADDR_START;
	addr[1] = temp >> 8;
	addr[0] = temp;
	
	OfflineTransaction.CardId		= cardNum;
	OfflineTransaction.LocationId	= ReaderSetup.locationId;
	OfflineTransaction.MachineId[0] = SQACAMachineStatus.MachineType[0];
	OfflineTransaction.MachineId[1] = SQACAMachineStatus.MachineType[1];
	OfflineTransaction.CycleType	= SQACAMachineStatus.CycleType;
	OfflineTransaction.ManufactureId = ReaderSetup.manufacturerId;
	switch( deviceStatus.deviceType[0] )
	{
		case PROGRAMMING_DATA_TOPLOAD:
		
		switch( SQACAMachineStatus.CycleType )								//user selected Cycle Value 1-9
		{
			case 9:
			OfflineTransaction.vendPrice = SQACAToploadProgramming.VendPrice9[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAToploadProgramming.VendPrice9[1];
			break;
			case 8:
			OfflineTransaction.vendPrice = SQACAToploadProgramming.VendPrice8[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAToploadProgramming.VendPrice8[1];
			break;
			case 7:
			OfflineTransaction.vendPrice = SQACAToploadProgramming.VendPrice7[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAToploadProgramming.VendPrice7[1];
			break;
			case 6:
			OfflineTransaction.vendPrice = SQACAToploadProgramming.VendPrice6[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAToploadProgramming.VendPrice6[1];
			break;
			case 5:
			OfflineTransaction.vendPrice = SQACAToploadProgramming.VendPrice5[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAToploadProgramming.VendPrice5[1];
			break;
			case 4:
			OfflineTransaction.vendPrice = SQACAToploadProgramming.VendPrice4[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAToploadProgramming.VendPrice4[1];
			break;
			case 3:
			OfflineTransaction.vendPrice = SQACAToploadProgramming.VendPrice3[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAToploadProgramming.VendPrice3[1];
			break;
			case 2:
			OfflineTransaction.vendPrice = SQACAToploadProgramming.VendPrice2[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAToploadProgramming.VendPrice2[1];
			break;
			case 1:
			OfflineTransaction.vendPrice = SQACAToploadProgramming.VendPrice1[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAToploadProgramming.VendPrice1[1];
			break;
		}
		case PROGRAMMING_DATA_FRONTLOAD:
		
		switch( SQACAMachineStatus.CycleType )								//user selected Cycle Value 1-9
		{
			case 9:
			OfflineTransaction.vendPrice = SQACAFrontloadProgramming.VendPrice9[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAFrontloadProgramming.VendPrice9[1];
			break;
			case 8:
			OfflineTransaction.vendPrice = SQACAFrontloadProgramming.VendPrice8[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAFrontloadProgramming.VendPrice8[1];
			break;
			case 7:
			OfflineTransaction.vendPrice = SQACAFrontloadProgramming.VendPrice7[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAFrontloadProgramming.VendPrice7[1];
			break;
			case 6:
			OfflineTransaction.vendPrice = SQACAFrontloadProgramming.VendPrice6[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAFrontloadProgramming.VendPrice6[1];
			break;
			case 5:
			OfflineTransaction.vendPrice = SQACAFrontloadProgramming.VendPrice5[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAToploadProgramming.VendPrice5[1];
			break;
			case 4:
			OfflineTransaction.vendPrice = SQACAFrontloadProgramming.VendPrice4[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAFrontloadProgramming.VendPrice4[1];
			break;
			case 3:
			OfflineTransaction.vendPrice = SQACAFrontloadProgramming.VendPrice3[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAFrontloadProgramming.VendPrice3[1];
			break;
			case 2:
			OfflineTransaction.vendPrice = SQACAToploadProgramming.VendPrice2[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAFrontloadProgramming.VendPrice2[1];
			break;
			case 1:
			OfflineTransaction.vendPrice = SQACAFrontloadProgramming.VendPrice1[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAFrontloadProgramming.VendPrice1[1];
			break;
		}
		case PROGRAMMING_DATA_DRYER:
		
		switch( SQACAMachineStatus.CycleType )								//user selected Cycle Value 1-9
		{
			case 5:
			OfflineTransaction.vendPrice = SQACADryerProgramming.HeatVendPrice4[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) | SQACADryerProgramming.HeatVendPrice4[1];
			break;
			case 4:
			OfflineTransaction.vendPrice = SQACADryerProgramming.HeatVendPrice1[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) | SQACADryerProgramming.HeatVendPrice2[1];
			break;
			case 3:
			OfflineTransaction.vendPrice = SQACADryerProgramming.NoHeatVendPrice[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) | SQACADryerProgramming.NoHeatVendPrice[1];
			break;
			case 2:
			OfflineTransaction.vendPrice = SQACADryerProgramming.HeatVendPrice3[0];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) | SQACADryerProgramming.HeatVendPrice3[1];
			break;
			case 1:
			OfflineTransaction.vendPrice = SQACADryerProgramming.HeatVendPrice2[1];
			OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) |SQACAToploadProgramming.VendPrice1[1];
			break;
		}
	}
	//OfflineTransaction.vendPrice = SQACAToploadProgramming.VendPrice1[0];
	//OfflineTransaction.vendPrice = (OfflineTransaction.vendPrice >> 8 ) | SQACAToploadProgramming.VendPrice1[1];
	
	OfflineTransaction.Date[0] = 0x00;
	OfflineTransaction.Date[1] = 0x00;
	OfflineTransaction.Date[2] = 0x00;
	OfflineTransaction.Date[3] = 0x00;
	
	OfflineTransaction.Time[0] = 0x00;
	OfflineTransaction.Time[1] = 0x00;
	OfflineTransaction.Time[2] = 0x00;
	
	OfflineTransaction.isOffline = true;
	//get number of stored records
	halGetEeprom(OFFLINE_TRANSACTION_RECORD_ADDR, 1, &ReaderSetup.numOfSavedTransactions);
	//store structure in the next EEPROM record address
	if ((ReaderSetup.numOfSavedTransactions == 0xff) || (ReaderSetup.numOfSavedTransactions < ReaderSetup.maxOfflineTransaction))
	{
		if (ReaderSetup.numOfSavedTransactions == 0xff || ReaderSetup.numOfSavedTransactions == 0x00)
		{
			ReaderSetup.numOfSavedTransactions = 0;
			//halPutEeprom(OFFLINE_TRANSACTION_RECORD_ADDR,1, &num_of_trans);
			//store first transaction records address.
			halPutEeprom(OFFLINE_TRANSACTION_RECORD_START,2,addr);
		}
		/*		else{
		//start at end of existing records
		halGetEeprom(OFFLINE_TRANSACTION_RECORD_START,2,addr);
		}	*/
		
		halPutEeprom((u8*)(OFFLINE_TRANSACTION_ADDR_START + (ReaderSetup.numOfSavedTransactions * OFFLINE_TRANSACTION_NUM_BYTE)), OFFLINE_TRANSACTION_NUM_BYTE, (u8*)&OfflineTransaction);
		ReaderSetup.numOfSavedTransactions++;
		halPutEeprom((u8*)OFFLINE_TRANSACTION_RECORD_ADDR,1,&ReaderSetup.numOfSavedTransactions);
		if(ReaderStateFlag.OfflineTransactionExist == false)
		{
			ReaderStateFlag.OfflineTransactionExist = true;
			halPutEeprom(READER_STATE_ADDR,2,(u8*)&ReaderStateFlag);
		}
		if (ReaderSetup.numOfSavedTransactions == ReaderSetup.maxOfflineTransaction)
		{
			ReaderStateFlag.MaxNumTransReached = true;
			halPutEeprom(READER_STATE_ADDR,2,(u8*)&ReaderStateFlag);
		}
	}
	else if(ReaderSetup.numOfSavedTransactions >= ReaderSetup.maxOfflineTransaction){
		halGetEeprom(READER_STATE_ADDR, 1, (u8*)&ReaderStateFlag);
		ReaderStateFlag.EnableOfflineTransaction = false; // stop conducting offline transaction, EEPROM is full.
		ReaderStateFlag.MaxNumTransReached = true;
		halPutEeprom(READER_STATE_ADDR,1,(u8*)&ReaderStateFlag);
		return_code = 0;
	}
	return return_code;
}

/**
@brief Build serial transmit buffer


*/
u8 sendStoredTransaction(void)
{
	//get total number of stored off line transactions from EEPROM.
	halGetEeprom(OFFLINE_TRANSACTION_RECORD_ADDR,1,&ReaderSetup.numOfSavedTransactions);
	
	//build buffer of transactions to be transmitted.
	if (ReaderSetup.numOfSavedTransactions > 0 && ReaderSetup.numOfSavedTransactions < 0xFF) // valid number of transactions stored, prepare to send to BOW
	{
		structTransaction record;
		u16 addr;
		
		//send data to BOW
		halGetEeprom(OFFLINE_TRANSACTION_RECORD_START,2,(u8*)&addr);
		halGetEeprom((void*)addr, OFFLINE_TRANSACTION_NUM_BYTE,(u8*)&record); // get transaction record
		
		if( sendBOWCCTransaction(&record) )
		{//update current record address
			addr = addr + OFFLINE_TRANSACTION_NUM_BYTE;
			
			ReaderSetup.numOfSavedTransactions--;
			halPutEeprom(OFFLINE_TRANSACTION_RECORD_START,2,(u8*)&addr);
			halPutEeprom(OFFLINE_TRANSACTION_RECORD_ADDR,1,&ReaderSetup.numOfSavedTransactions);
		}
	}
	if (ReaderSetup.numOfSavedTransactions == 0)
	{
		ReaderStateFlag.OfflineTransactionExist = false;
		ReaderStateFlag.MaxNumTransReached = false;
		halPutEeprom(READER_STATE_ADDR,1,(u8*)&ReaderStateFlag);
	}
	
	return ReaderSetup.numOfSavedTransactions;
}


void getDate(structDate *date)
{
	date->month = 0;
	date->day = 0;
	date->year = 0;
}

void getTime(structTime *time)
{
	time->hours = 0;
	time->minutes = 0;
	time->seconds = 0;
}

/**
@brief  Function to store SetupString into Eeprom for testing purposes.
*/
void defaultSetupString()
{
	u8 n;	
	char nibbleStr[50] = {"00770076007500740073007200710070006F00190503"};
	char *ptr = &nibbleStr[0] ;
	n = asciiStringToNibble(ptr, nibbleStr); //position at eeprom address 0x24
	
	halPutEeprom(MACHINE_SETUP_ADDR, n, (u8*)ptr);
}
/** @} */
/** @} */