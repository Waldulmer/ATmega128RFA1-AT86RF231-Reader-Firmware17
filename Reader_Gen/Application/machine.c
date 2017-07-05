
#include "mac.h"

#ifndef COORNODE

#include "reader.h"
#include "serial.h"
#include "machine.h"
#include "avr_timer.h"
#include "timer_enddevice.h"
#include <avr/wdt.h>
#include <util/delay.h>
#include "hal_avr.h"
#include "eeprom_map.h"

#include "mac_event.h"
#include "SpeedQueen.h"

/**
@addtogroup pposreader
@{
Machine Section

*/
u16 calcMachineCRC(u8 *msg, u8 len);
#define MAX_ALLOWED_ARRAY_SIZE  50

#if ( DEVICE_CONNECTED == ACA_MACHINE )


u8 ucTopOffTime;
u8 ucCardErrorCode;

u16 ucCardBalance;
u16 cardId;
double vendPrice;

u8 ucTimerCount;



void WaitForMachine(void)
{
	ucMachineWait = MACHINE_TIME_OUT;
}




void InitTimerZero(void)
{
//	TCCR0B = (1<<CS02) | (1<<CS00);	//set prescaler to 1024
TIFR0 = 2;//1<<TOV0;					//clear TOV0 / clear interrupt
TIMSK0 = 2;//1<<TOIE0;				//enable timer0 overflow interrupt
TCNT0 = 0;//16;
ucTimerCount = 0;
}


void StopTimerZero(void)
{
TCCR0B = 0;//~(1<<CS02) | ~(1<<CS01) | ~(1<<CS00);	//set prescaler to zero
}

/* set timer zero to 5 milisecond resolution and start.*/
void StartTimerZero(void)
{
OCR0A = 78;
TCCR0A = 2;
TCCR0B = (1<<CS02) | (1<<CS00);	//set prescaler to 1024
}

ISR(TIMER0_COMPA_vect)
{
ucTimerCount++;

}


u8 sendMdcPacket(u8 *ucDataBuf)
{
u8 buf_size,i,n;
u8 BCC = 0;

if (ucDataBuf[0] == 0)
return 0;

for(n = 0; n < NUMRETRIES; n++)
{
BCC = STX;
init_rx_buf();
buf_size = (ucDataBuf[0] + 1);	//number of bytes sending
serial_putchar(STX);    // Send STX, start byte


for (i = 0; i < buf_size; i++)
{
serial_putchar(ucDataBuf[i]);
BCC = (BCC ^ ucDataBuf[i]);
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

return 0;
}


#endif // (MACHINE_TYPE)


u16 calcMachineCRC(u8 *msg, u8 len)
{
u8 i;                            //loop counter 1
u8 j;                            //loop counter 2
u8 flag;                         //flag
u16 crc;                         //CRC
u8 tempMsg[MAX_ALLOWED_ARRAY_SIZE];//temp array

crc = 0;                            //clear crc

for(i=0;i<len; i++)                //save into temp array
tempMsg[i] = msg[i];

for(i=0;i<len; i++)                 //loop through msg array
{
for(j=0;j<8; j++)                //loop through bits
{
flag = crc & 0x0001;          //set or reset flag
crc >>= 1;                    //shift crc
if (flag ^ (tempMsg[i] & 0x01)) crc ^= 0xA001;//crc calculation
tempMsg[i] >>= 1;                //shift message
}
}
return (crc);                       //return crc
}
//end of calcMachin
/** @} */
#endif // (NODETYPE == ENDDEVICE)
