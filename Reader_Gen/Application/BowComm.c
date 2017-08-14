/*
* BowComm.c
*
* This file contains the interface to the BOW.
*
* Created: 11/22/2013 6:42:27 AM
*  Author: Paul
*/

#include "BowComm.h"

/**
@addtogroup app
@{
@defgroup pposbow BOW Communication
@{
*/
#define lowNibble 0
#define highNibble 1

/**
Application function, sends a data frame to the coordinator
This contains a string payload that is displayed on the coordinator end.
*/
bool sendBOWReaderFirmwareVersion()
{
	char tmp[128]={0};

	if ( constructOutgoingString(FIRMWARE_VERSION, tmp) )
	{
		macDataRequest(DEFAULT_COORD_ADDR, (u8)tmp[0], (u8*)tmp+1);
		return true;
	}
	return false;
}

/**
Application function, sends a data frame to the coordinator
This contains a string payload that is displayed on the coordinator end.
*/
bool sendBOWSetupRequest()							//virgin
{
	char tmp[128] = {0};
	
	constructOutgoingString(REQUEST_SETUP, tmp);
	macDataRequest(DEFAULT_COORD_ADDR, (u8)tmp[0], (u8*)tmp+1);
	
	return true;
}

/**
Application function, sends a data frame to the coordinator
This contains a string payload that is displayed on the coordinator end.
*/
bool sendBOWValidationRequest()
{
	char tmp[25] = {0};
	
	constructOutgoingString(REQUEST_VALIDATION, tmp);
	macDataRequest(DEFAULT_COORD_ADDR, (u8)tmp[0], (u8*)tmp+1);
	
	return true;
}

/**
Application function, sends a data frame to the coordinator
This contains a string payload that is displayed on the coordinator end.
*/
bool sendBOWMachineLabel()
{
	char tmp[35] = {0};
	
	constructOutgoingString(REQUEST_MACHINE_LABEL, tmp);
	macDataRequest(DEFAULT_COORD_ADDR, (u8)tmp[0], (u8*)tmp+1);
	
	return true;
}

/**
Application function, sends a data frame to the coordinator
This contains a string payload that is displayed on the coordinator end.
*/
bool sendBOWMachineDescription()
{
	char tmp[85] = {0};
	
	constructOutgoingString(REQUEST_MACHINE_DESCRIPTION, tmp);
	macDataRequest(DEFAULT_COORD_ADDR, (u8)tmp[0], (u8*)tmp+1);
	
	return true;
}

/**
Application function, sends a data frame to the coordinator
This contains a string payload that is displayed on the coordinator end.
*/
bool sendNumberOfOLT()
{
	char tmp[35] = {0};
	
	constructOutgoingString(REQUEST_NUMBER_OF_OLT, tmp);
	macDataRequest(DEFAULT_COORD_ADDR, (u8)tmp[0], (u8*)tmp+1);
	
	return true;
}

/**
\brief Converting ASCII to Nibble.

param inStr is the ASCII value to be converted.

return outStr is the nibble value with number of characters received.
*/
/*
u8 asciiStringToNibble( char *inStr, char *outStr)
{
	u8 n, k;
	
	n = strlen(inStr);
	k = 0;


	while (k < n)
	{
		*outStr = (Ascii_to_nible ( *(inStr+k) ) * 16)
		+  Ascii_to_nible ( *(inStr+k+1));
		k += 2;
		
		outStr++;
	}
	return n/2;
}
*/

/**
Construct a frame for transmission.
@param data Pointer to the data to be sent.

@param type Type of frame to be sent
*/
bool constructOutgoingString(u8 strType, char *tempStr)
{
	switch(strType)
	{
		case FIRMWARE_VERSION:
		//build firmware version string
		sprintf(tempStr+1, "<FirmwareVersion>"
		"<fw>%s</fw>"
		"<DeviceType>%d%d<Devicetype>"
		,__APP_VERSION_STRING__,deviceStatus.deviceType[0],deviceStatus.deviceType[1]);
		
		break;
		
		case REQUEST_SETUP:
		// Build setup request string
		sprintf(tempStr+1, "<ReaderSetupInfo>"
		"<laundryRoomId>%d</laundryRoomId>"
		"<manufacturerId>%d</manufacturerId>"
		"<code>%d%d</code>",ReaderSetup.locationId, ReaderSetup.manufacturerId, deviceStatus.deviceType[0],deviceStatus.deviceType[1]);
		
		break;
		
		case REQUEST_VALIDATION:
		//build validation request string
		sprintf(tempStr+1, "<ValidationRequest>");
		
		break;
		
		case REQUEST_MACHINE_LABEL:
		//build machine label response string
		
		sprintf(tempStr+1, "<MachineLabel>"
		"<mL>%s</mL>"
		,ReaderSetup.machineLabel);
		
		break;
		
		case REQUEST_MACHINE_DESCRIPTION:
		
		//build machine description response string
		sprintf(tempStr+1, "<MachineDescription>"
		"<mD>%s</mD>"
		,ReaderSetup.machineDescription);
		
		break;
		//number of OfflineTransactions
		case REQUEST_NUMBER_OF_OLT:
		
		halGetEeprom(OFFLINE_TRANSACTION_RECORD_ADDR, 1, &ReaderSetup.numOfSavedTransactions);
		if (ReaderSetup.numOfSavedTransactions == 0xff)
		{
			ReaderSetup.numOfSavedTransactions = 0;
		}
		//build number of offline transactions string
		sprintf(tempStr+1, //"<OLT>"
		"<OLT>%d</OLT>"
		,ReaderSetup.numOfSavedTransactions);
		
		break;
		
		default:
		
		break;
	}
	
	tempStr[0] = strlen(tempStr+1); //get string length then assign to first element
	tempStr[0] = tempStr[0] + 1;    // data size = data byte + size byte
	
	if (tempStr[0] > 127)
	{
		//#error "Outgoing string to large"
		return false;
	}

	return true;
}

/** \ingroup pposbow
\fn char *extractIncomingMessage(const char *dataBuffer)
\brief Locate a substring.

The extractIncomingMessage(const char *dataBuffer) function returns a pointer to the beginning of the substring.
\ or \c NULL if the substring is not found.

Sets also flag READER_SETUP_DONE
*/
u8 extractIncomingMessage(char *dataBuffer)
{
	u8 TemplongAddr[8] = {0};
	u8 MaclongAddr[8]  = {0};
	u16 tmp;
	char *ptr;
	u8 n;
	char nibbleStr[50] = {0};
	u8 i = 0;
	u8 Nibble = 0;
	u8 returnCode = OP;
	static u8 oLTTimerId = 0;
	
	/** @brief Extract BOW Requests from incoming string.
	Incoming Requests with OPcode are:
	1 SetupPackage			APP_INIT
	2 SetupRequest			SETUP_REQUEST
	3 ValidationResponse	VALIDATE_SETUP
	4 GetFirmwareVersion
	5 ResetEEPROM
	6 EnterBootloader
	7 SETD
	8 SetMachineLabel
	9 GetMachineLabel
	10 GetOLT				SEND_OFFLINE_TRANSACTIONS
	11 ShowBalance			SHOW_BALANCE_REQUEST
	12 faultcode
	13 AuditRq				AUDITDATA_REQUEST
*/
	//1
	if(strstr(dataBuffer, "SetupPackage") != NULL)
	{
		//if((ptr = GetParam(dataBuffer,"<SetupString>","<")) != NULL)			PPOS170601
		if((ptr = GetParam(dataBuffer,"<SS>","<")) != NULL)
		{
			n = asciiStringToNibble(ptr, nibbleStr);
			// copy setup data

			halPutEeprom(MACHINE_SETUP_ADDR, n, (u8*)nibbleStr); //write machine setup string to eeprom; Maurice's Fix
			
			ReaderSetup.maxOfflineTransaction = nibbleStr[n-1];  //PPOS170712
			halPutEeprom(MAXNUM_OFFLINE_ADDR, 1, (u8*)&ReaderSetup.maxOfflineTransaction); //write machine setup string to eeprom minus "number of offline transactions"
			
			ReaderStateFlag.ReaderSetup = READER_SETUP_DONE;
			halPutEeprom(READER_STATE_ADDR, 1, (u8*)&ReaderStateFlag);
		}
			
		if((ptr = GetParam((char *)dataBuffer,"<ValidationCode>","<")) != NULL)
		{			
			n = asciiStringToNibble(ptr, nibbleStr);
			// save validation code
			halPutEeprom(VALIDATION_CODE_ADDR, 1, (u8*)&n);
			halPutEeprom(VALIDATION_CODE_ADDR+1, n, (u8*)nibbleStr);
		}		
		
		if((ptr = GetParam((char *)dataBuffer,"<ReaderId>","<")) != NULL)
		{ //1
			tmp = atoi(ptr);
			
			while (tmp> 0)
			{
				switch(Nibble)
				{
					case lowNibble:

					TemplongAddr[i] = (tmp % 16);
					Nibble = highNibble;
					break;

					case highNibble:

					TemplongAddr[i] =  ((tmp % 16)<<4)| TemplongAddr[i];
					i++;
					Nibble = lowNibble;
					break;
				}
				
				tmp = tmp / 16;
			}

			//Update Reader to eeprom in Hex format
			halPutMacAddr(TemplongAddr);

			// Verify write operation
			halGetMacAddr(MaclongAddr);

		}//1


		// Re-associate with COORD using new ReaderID
		if(strcmp((char*)TemplongAddr,(char*)MaclongAddr) == 0 )
		{
			#if (DEBUG)
			Beeps(5);// Beeps 5x
			#endif
			

			ReaderStateFlag.FirstRun = NON_VIRGIN_READER;
			halPutEeprom(READER_STATE_ADDR,1,(u8*)&ReaderStateFlag);

			// re-associate with new readerId
			returnCode = APP_INIT;
		}

	}
	//2
	else if(strstr(dataBuffer, "SetupRequest") != NULL)
	{
		returnCode = SETUP_REQUEST;
	}
	//3
	else if(strstr(dataBuffer, "ValidationResponse") != NULL)
	{
		returnCode = VALIDATE_SETUP;
	}
	//4
	else if(strstr(dataBuffer, "GetFirmwareVersion") != NULL)
	{
		sendBOWReaderFirmwareVersion();
		
	}
	//5
	else if(strstr(dataBuffer, "ResetEEPROM") != NULL)
	{
		unsigned char reset_eeprom[8] = {255,255,255,255,255,255,255,255};
		halPutEeprom(0,8,(u8*)reset_eeprom);
		
		// re-associate
		returnCode = APP_INIT;
	}
	//6
	else if(strstr(dataBuffer, "EnterBootloader") != NULL)
	{
		// Store Active CHANNEL to EEPROM Address 0xFF2
		halPutEeprom((u16*)CHANNEL_ADDR, 1, &macConfig.currentChannel);

		// Store Active PAN_ID to EEPROM Address 0xFF4
		halPutEeprom((u16*)PANID_ADDR, 2, (u8*)&macConfig.panId);

		// Store Active SHORT SOURCE ADDR to EEPROM Address 0xFF6
		halPutEeprom((u16*)SRCADDR_ADDR,2, (u8*)&macConfig.shortAddress);
		
		i = 0x01;
		halPutEeprom((u16*)BOOT_INIT_ADDR,1,(u8*)&i);
		
		// zero out CRC and Application size memory location
		memset(dataBuffer,0,sizeof((char*)dataBuffer));
		halPutEeprom((u16*)APP_CRC,6,(u8*)&dataBuffer);

		
		// Initiate watchdog timer
		cli();
		wdt_enable(WDTO_2S);
		while(1);

	}
	//7
	else if(strstr(dataBuffer, "SetD") != NULL)
	{
		if((ptr = GetParam((char *)dataBuffer,"<SetD>","<")) != NULL)
		{
			if((n=strlen(ptr)) <= MACHINE_NAME_SIZE)
			{
				strcpy((char*)ReaderSetup.machineDescription, ptr);
				n++;//add null terminator to string
				halPutEeprom(MACHINE_DESCRIPTION_ADDR, n, (u8*)&ReaderSetup.machineDescription);
				
				ReaderStateFlag.ReaderNameSetup = SET_READER_FLAG;
				halPutEeprom(READER_STATE_ADDR,1,(u8*)&ReaderStateFlag);
			}
		}
	}
	//8
	else if(strstr(dataBuffer, "GetD") != NULL)
	{
		sendBOWMachineDescription();
		
	}
	//9
	else if(strstr(dataBuffer, "SetMachineLabel") != NULL)
	{
		if((ptr = GetParam((char *)dataBuffer,"<SetMachineLabel>","<")) != NULL)
		{
			if((n=strlen(ptr)) <= MACHINE_LABEL_SIZE)
			{
				strcpy((char*)ReaderSetup.machineLabel, ptr);
				n++;//add null terminator to string
				halPutEeprom(MACHINE_LABEL_ADDR, n, (u8*)&ReaderSetup.machineLabel);
				
				ReaderStateFlag.ReaderLabelSetup = SET_READER_FLAG;
				halPutEeprom(READER_STATE_ADDR,1,(u8*)&ReaderStateFlag);
			}
			
		}
	}
	//10
	else if(strstr(dataBuffer, "GetMachineLabel") != NULL)
	{
		sendBOWMachineLabel();
	}
	//11
	else if (strstr(dataBuffer, "GetOLT") != NULL)
	{
		sendNumberOfOLT();
		
		oLTTimerId = macSetLongAlarm(SERVER_RESPONSE_TIMEOUT,timeoutWaitingForOLTReponse);
		returnCode = SEND_OFFLINE_TRANSACTIONS;
	}
	//12
	else if(strstr(dataBuffer, "ShowBalance") != NULL)
	{
		returnCode = SHOW_BALANCE_REQUEST;
	}
	//13
	else if (strstr(dataBuffer, "faultcode") != NULL)
	{
		char errCode = 0;
		if((ptr = GetParam((char *)dataBuffer,"<faultcode>","<")) != NULL)
		{
			asciiStringToNibble(ptr, &errCode);
			
			switch(errCode)
			{
				case TRANSACTION_SUCCESSFUL:
				if (returnCode == SEND_OFFLINE_TRANSACTIONS)
				{
					macTimerEnd(oLTTimerId);
					if( !sendStoredTransaction() )
					{ //All stored transactions have been sent
						returnCode = IDLE;
					}
					else
					{	//set new timer while waiting for next response
						oLTTimerId = macSetLongAlarm(SERVER_RESPONSE_TIMEOUT,timeoutWaitingForOLTReponse);
					}
				}
				
				break;
				case INVALID_MANU_CODE:
				//do something, machine does not belong to manufacturer
				break;
				case MACHINE_SETUP_NOT_FOUND:
				//continue trying to get setup.
				break;
				case INVALID_SETUP:
				//not sure what this means
				break;
				case INVALID_CYCLE_PRICE:
				//not sure what this means
				break;
				case ACCOUNT_NOT_FOUND:
				//show user error message then continue previous task
				break;
				case INVALID_PIN:
				//show user error message then continue previous task
				break;
				case LAUNDRY_ROOM_NOT_FOUND:
				//continue trying to get setup.
				break;
				case READER_NOT_FOUND:
				//not sure what this means
				break;
				case INVALID_MACHINE_LABEL:
				//not sure what this means
				break;
				case MACHINE_LABEL_NOT_FOUND:
				//not sure what this means
				break;
				case SERIAL_NUM_NOT_FOUND:
				//show user error message then continue previous task
				break;
				case CARD_NOT_ASSIGNED:
				//show user error message then continue previous task
				break;
				case USER_NOT_FOUND:
				//show user error message then continue previous task
				break;
				case USER_NOT_AUTHORIZED:
				//show user error message then continue previous task
				break;
				case INSUFFICIENT_FUNDS:
				//show user error message then continue previous task
				break;
				default:
				break;
			}
		}
		
	}
	//14 new: Audit Data ACA Short Audit Command = 0x30
	else if(strstr(dataBuffer, "AuditRq") != NULL)
	{
		returnCode = AUDITDATA_REQUEST;
	}
	return returnCode;
}

/**
@brief The sendBOWCCTransaction function is used to send a frame over the air to the BOW.
The reader sends this packet frame to perform a vending transaction.

@param data Pointer to the data to be sent.
*/
bool sendBOWCCTransaction(structTransaction *transRecord)
{
	char tmpCardIdString[10] = {0};
	char tempStr[128] = {0};
	
	//send data to BOW
	ultoa(transRecord->CardId,tmpCardIdString,10);
	sprintf((char *)tempStr+1, "<CC>"
	"<sN>%s</sN>"
	"<cP>%.2f</cP>"
	"<locId>%d</locId>"
	"<manufId>%d</manufId>"
	"<code>%d%d</code>"
	"<isOL>%d</isOL>"
	,tmpCardIdString, (double)(transRecord->vendPrice / 100.0), transRecord->LocationId, transRecord->ManufactureId
	, transRecord->MachineId[0] , transRecord->MachineId[1], transRecord->isOffline);
	
	tempStr[0] = strlen(tempStr+1); //get string length then assign to first element
	tempStr[0] = tempStr[0] + 1;    // data size = data byte + size byte
	
	if (tempStr[0] > 127)
	{
		//#error "Outgoing string to large"
		return false;
	}
	
	macDataRequest(DEFAULT_COORD_ADDR,(u8)tempStr[0],(u8*)tempStr+1); //send record via radio
	
	return true;
}

/**
	@brief The sendBOWAuditDataRequest function is used to send a frame over the air to the BOW. 
	The reader sends this packet frame (Audit Data packet 0x30) to transmit Short Audit Data about the vend.
*/
bool sendBOWAuditDataRequest()
{
	char tmp[128] = {0};
	
	constructOutgoingString(REQUEST_AUDITDATA, tmp);
	macDataRequest(DEFAULT_COORD_ADDR, (u8)tmp[0], (u8*)tmp+1);
	
	return true;
}

/** @} */
/** @} */