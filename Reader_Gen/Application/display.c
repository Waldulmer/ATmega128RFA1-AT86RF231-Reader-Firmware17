/******************************************************************************************************************
 *
 * File Name:  display.c
 *             This is file contains the machine display functions.
 *					
 *
 * Author:     Paul Bastien
 * Date:       07/30/11
 *
 ******************************************************************************************************************** 
 *
 *
 * Revision   r001 original version
 * History:  VER      SCR #    Author         Date       Description
 *           -----    -----    ------         ----       -----------
 *
 **********************************************************************************************************************
 */ 
#include "mac.h"
#include "application.h"
#include "reader.h"
#include "display.h"
#include "SpeedQueen.h"

#include <util/delay.h>

/**
	@addtogroup pposreader
	@{
   @addtogroup machinedisplay Machine Control Display
   @{
   This file implements an interface for the Machine Control Display.
   The Payment System may control the Machine Control display via a Display Request Package.
   The Machine control will respond with an <ACK> to indicate the packet was received, 
   and will display the characters passed in for the specified time.
   The Payment System may write information to the machine control display, 
   and may specify how long the given display is to remain active. This command
   allows the user to specify what will be displayed in each of the digits on 
   the machine control display. The duration of the display is adjustable from 1 to 255 seconds.   
*/

u8 msgTable[] = {

                        LED_n, LED_E, LED_E, LED_d, 
                        LED_S, LED_E, LED_r, LED_BLANK,
                        LED_C, LED_A, LED_r, LED_d,
                        LED_F, LED_U, LED_L, LED_L,
                        LED_P, LED_U, LED_L, LED_L,
                        LED_Y, LED_E, LED_A, LED_r,
                        LED_n, LED_N, LED_o, LED_n,
                        LED_d, LED_A, LED_t, LED_E,
                        LED_H, LED_o, LED_u, LED_r,
                        LED_n, LED_N, LED_i, LED_n,
                        LED_S, LED_E, LED_t, LED_C,
                        LED_BLANK, LED_n, LED_o, LED_BLANK,
                        LED_u, LED_t, LED_d, LED_c,
                        LED_d, LED_o, LED_n, LED_E,
                        LED_b, LED_u, LED_S, LED_Y,
                        LED_BLANK, LED_r, LED_S, LED_t,
                        LED_n, LED_o, LED_S, LED_u,                 
                        LED_U, LED_C, LED_F, LED_L,
                        LED_BLANK, LED_S, LED_u, LED_BLANK,
                        LED_BLANK, LED_d, LED_c, LED_BLANK,
                        LED_P, LED_A, LED_U, LED_S,
                        LED_P, LED_A, LED_S, LED_S,
                        LED_F, LED_A, LED_I, LED_L,
                        LED_BLANK, LED_S, LED_t, LED_BLANK,						
                        LED_BLANK, LED_b, LED_A, LED_L, LED_BLANK, LED_BLANK,
                        LED_BLANK, LED_BLANK, LED_BLANK, LED_BLANK, 

};
                      
// defined messages
u8 msgNumber[] = {
                           LED_0,
                           LED_1,
                           LED_2,
                           LED_3,
                           LED_4,
                           LED_5,
                           LED_6,
                           LED_7,
                           LED_8,
                           LED_9,
                           LED_A,
                           LED_b,
                           LED_C,
                           LED_d,
                           LED_E,
                           LED_F,
                           0        // NUM_BLANK = 0x10
};


#if (DEVICE_CONNECTED == ACA_MACHINE)

/** @brief function to identify text message to send to the machine control display  
   This function will be used to send text to the ACA 6 position 7 segment display. 
   Led1 to led6 are limited to values defined in "display.h"
   @param msg short parameter of the message.
*/
void displayMsg(u8 msg) 
{
	if (msg == CARD_ERR_MSG)
	{
		displaySQCardError();
	}
	else if (msg == SETUP_MSG)		//no communication with the BOW
	{
		sendSQDisplayCommand(LED_n,LED_o,LED_S,LED_U,LED_BLANK,LED_BLANK,5);
	}
	else if (msg == BUSY_MSG)
	{
		sendSQDisplayCommand(LED_b,LED_U,LED_S,LED_Y,LED_BLANK,LED_BLANK,5);
	}
	else if (msg == SERVICE_MSG)
	{
		sendSQDisplayCommand(LED_S,LED_E,LED_r,LED_BLANK,LED_BLANK,LED_BLANK,5);
	}
	else if (msg == BLANK_MSG)
	{
		sendSQDisplayCommand(LED_BLANK,LED_BLANK,LED_BLANK,LED_BLANK,LED_BLANK,LED_BLANK,0);
	}
	else if (msg == CABLE_MSG)		//PPOS Test message
	{
		sendSQDisplayCommand(LED_C,LED_A,LED_b,LED_L,LED_E,LED_BLANK,5);
	}

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
   on the ACA display for a minimum of 5 seconds.

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


/** @brief Payment System Display Request for Account Balance. 
   This function will display the individual account balance 
   on the ACA machine control display for a minimum of 5 seconds.
   Format the Display Balance: "1" is just a character as far as the LCD is concerned. 
   It's not a number. The LCD wants you to send the "code" for each character you want to display.
   The codes for characters "0" through "9" are 48 through 57. So just adding 48 to any single-digit 
   number will give you the code for the character that	corresponds to that digit, f.e. uctempBuf[0]=49. digit 1=uctempBuf[0]-0x30 .

   Parameter:  float balance
 */
bool displaySQBalance(float amount)
{
	 uint8_t * balance = (uint8_t *) &amount;
	u8 temp[9]={0};
			
	temp[0] = DISPLAY_REQUEST_BYTES;	//Number of actual data bytes to be transferred to the ACA including command data exclude bbc byte
	temp[1] = DISPLAY_REQUEST;			//ACA command to be requested
	temp[2] = balance[0];
	temp[3] = balance[1];
	temp[4] = balance[2];
	temp[5] = balance[3];	
	temp[8] = 5;					//Duration of display in seconds
	
	return ( sendSQDataPacket(temp) );
}

/** @brief Payment System Display Request for TopOff vend. 
   This function will display the TopOff vend price purchased after a regular vend 
   on the ACA Dryer display for a minimum of 5 seconds.

   Parameter:  double vend

   maybe I use the above funct instead
 */
bool displaySQTopOffvend(double vend)
{
	if( sendSQDisplayCommand(LED_t, LED_o, LED_P, LED_BLANK, LED_BLANK,LED_BLANK, 5) )
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


#endif
                                                         