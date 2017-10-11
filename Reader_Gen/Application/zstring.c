/*
* zstring.c
*
* Created: 8/13/2017 10:43:08 PM
*  Author: Dummy
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rum_types.h"

/**
@addtogroup serial
@{
@name String (zstring.c)
@{
*/


/**
\brief The PadLeft method right-aligns and pads a string so that its rightmost character is the specified distance from the beginning of the string.
PadLeft return newString objects that can either be padded with empty spaces or with custom characters.
\param string is the string size.
\param paddedLength is the data size.
\param pad is pointer to data to be converted.
*/
char * padLeft(char * string, u8 paddedLength, const char * pad)
{
	size_t stringLength = strlen(string);
	size_t lenpad = strlen(pad);
	u8 i = paddedLength;
	
	if (stringLength >= paddedLength)
	{
		return NULL;//'\0';
	}

	char * padded = (char*)malloc(paddedLength + 1); // allocate memory for new string
	
	for(; paddedLength > stringLength; paddedLength--, padded += lenpad)
	{
		strncpy(padded, pad, lenpad);
	}
	
	strncpy(padded, string, stringLength); /* copy without '\0' */
	padded += stringLength; /* prepare for first append of pad */
	*padded = '\0';

	padded = (padded - i);
	strncpy(string,padded,i);
	
	return padded;
}

/**
\brief Convert data from String to Hexa.
\param ucDataLength is the data size.
\param pucDataBuffer is pointer to data to be converted.
*/
/*
void String_to_hexa_convert ( u8 ucDataLength, u8 * pucDataBuffer )
{
u8 i, ucDataBuffer[ucDataLength * 2];

for (i = 0; i < ucDataLength * 2; i++)
{
ucDataBuffer[i] = pucDataBuffer[i];
}
for (i = 0; i < ucDataLength; i++)
{
ucDataBuffer[2 * i] = Ascii_to_nible ( ucDataBuffer[2 * i] );
ucDataBuffer[(2 * i) + 1] = Ascii_to_nible ( ucDataBuffer[(2 * i) + 1] );
pucDataBuffer[i] = ((ucDataBuffer[2 * i]) << 4) + ucDataBuffer[(2 * i) + 1];
}
}
*/


/**
\brief Convert data from Hexadecimal to String.
\param ucDataLength is the data size.
\param pucDataBuffer is the pointer to the data to be converted.
*/
/**
void Hexa_to_string_convert ( u8 ucDataLength, u8* pucDataBuffer )
{
u8 i, ucDataBuffer[ucDataLength];
for (i = 0; i < ucDataLength; i++)
{
ucDataBuffer[i] = pucDataBuffer[i];
}

for (i = 0; i < ucDataLength; i++)
{
pucDataBuffer[2 * i] = Nible_to_ascii ( ucDataBuffer[i] >> 4 );
pucDataBuffer[(2 * i) + 1] = Nible_to_ascii ( ucDataBuffer[i] );
}
}
#endif // (NODETYPE == COORD)
*/


/**
\brief Function to send result back to PC through serial.
\param ucDataLength is the data size.
\param pucDataBuffer is the pointer to the data to be sent.
*/
/**
#if (NODETYPE == COORD)
void Serial_send_data (u8 ucDataLength, u8* pucDataBuffer)
{
u16 i;
serial_putchar(SOP);    // Send SOP
serial_putchar(ucDataLength);   // Send Length
for (i = 0; i < ucDataLength * 2; i++)
{
serial_putchar(pucDataBuffer[i]);
}
serial_putchar(EOP);    // Send EOP
}
*/

/** @} */
/** @} */




