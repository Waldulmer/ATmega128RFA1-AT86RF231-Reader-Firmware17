
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

/*static*/ u16 ucCardBalance;// = 07D0;
u16 cardId;
double vendPrice;

u8 ucTimerCount;



void WaitForMachine(void)
{
	ucMachineWait = MACHINE_TIME_OUT;
}

/*
u8 SerialGetMachineData(void)					//PPOS
{
static u8 ucACADataBuf[ACA_MAX_PACKET_SIZE];
static u8 i,n,BCC;


//get machine data
i = serial_getchar();
if( i == STX )//state byte detected
{
i = serial_getchar(); // get data field size.
BCC = (i ^ STX);

for(n = 0; n < i; n++)
{

ucACADataBuf[n]= serial_getchar();
BCC = (BCC ^ ucACADataBuf[n]);
}
i = serial_getchar();	// get BCC
if(BCC == i)
{
switch( ucACADataBuf[0])
{
case ACA_STATUS_RESPONSE_PACKET:
SQACAMachineStatus.MachineType[0]			= (ucACADataBuf[1] + PROGRAMMING_DATA);
SQACAMachineStatus.MachineType[1]			= 1;///temp. fix later, differentiate between MDC and Quantum
//ACAStatus.KeypadData[0]				= ucACADataBuf[2];
SQACAMachineStatus.CycleType				= ucACADataBuf[3];
SQACAMachineStatus.CmdToReader				= ucACADataBuf[4];
SQACAMachineStatus.MachineStatus[0]			= ucACADataBuf[5];
SQACAMachineStatus.MachineStatus[1]			= ucACADataBuf[6];
SQACAMachineStatus.unused1[0]				= ucACADataBuf[7];
SQACAMachineStatus.unused1[1]				= ucACADataBuf[8];
SQACAMachineStatus.RemainingCycleMinutes	= ucACADataBuf[9];
SQACAMachineStatus.RemainingCycleSeconds	= ucACADataBuf[10];
SQACAMachineStatus.RemainingVend[0]			= ucACADataBuf[11];
SQACAMachineStatus.RemainingVend[1]			= ucACADataBuf[12];
SQACAMachineStatus.VendenteredTotal[0]		= ucACADataBuf[13];
SQACAMachineStatus.VendenteredTotal[1]		= ucACADataBuf[14];
SQACAMachineStatus.VendenteredforCycleModifier[0]	= ucACADataBuf[15];
SQACAMachineStatus.VendenteredforCycleModifier[1]	= ucACADataBuf[16];
SQACAMachineStatus.VendenteredforTopOff[0]	= ucACADataBuf[17];
SQACAMachineStatus.VendenteredforTopOff[1]	= ucACADataBuf[18];
SQACAMachineStatus.NumberofCoins1			= ucACADataBuf[19];
SQACAMachineStatus.NumberofCoins2			= ucACADataBuf[20];
SQACAMachineStatus.KeypadData[0]	  		= ucACADataBuf[21];
SQACAMachineStatus.KeypadData[1]  			= ucACADataBuf[22];
SQACAMachineStatus.CurrentKey[0]			= ucACADataBuf[23];
SQACAMachineStatus.CurrentKey[1]			= ucACADataBuf[24];
break;


case ACA_AUDIT_RESPONSE_TLWPACKET:

SQACAAuditData.ProductByte[5] = ucACADataBuf[1];
SQACAAuditData.ControlSVN = ucACADataBuf[2];
SQACAAuditData.MachineCyclesCounter[0] = ucACADataBuf[3];
SQACAAuditData.MachineCyclesCounter[1] = ucACADataBuf[4];
SQACAAuditData.CycleModifierCounter[0] = ucACADataBuf[5];
SQACAAuditData.CycleModifierCounter[1] = ucACADataBuf[6];
SQACAAuditData.AvgFillTime[0] = ucACADataBuf[7];
SQACAAuditData.AvgFillTime[1] = ucACADataBuf[8];
SQACAAuditData.AvgDrainTime[0] = ucACADataBuf[9];
SQACAAuditData.AvgDrainTime[1]= ucACADataBuf[10];
SQACAAuditData.VendedTotal[0] = ucACADataBuf[7];
SQACAAuditData.VendedTotal[1] = ucACADataBuf[8];
SQACAAuditData.VendedTotalBaseVendPrice[0] = ucACADataBuf[9];
SQACAAuditData.VendedTotalBaseVendPrice[1]= ucACADataBuf[10];
SQACAAuditData.VendedTotalCycleModifiers[0] = ucACADataBuf[9];
SQACAAuditData.VendedTotalCycleModifiers[1]= ucACADataBuf[10];
SQACAAuditData.VendedTotalPS[0] = ucACADataBuf[9];
SQACAAuditData.VendedTotalPS[1] = ucACADataBuf[10];
SQACAAuditData.VendedTotalPSBaseVendPrice[0] = ucACADataBuf[9];
SQACAAuditData.VendedTotalPSBaseVendPrice[1]= ucACADataBuf[10];
SQACAAuditData.VendedTotalPSCycleModifiers[0] = ucACADataBuf[9];
SQACAAuditData.VendedTotalPSCycleModifiers[1]= ucACADataBuf[10];
SQACAAuditData.VendedTotalPSDiscounted[0] = ucACADataBuf[9];
SQACAAuditData.VendedTotalPSDiscounted[1]= ucACADataBuf[10];
/*
case ACA_AUDIT_RESPONSE_FLWPACKET:
case ACA_AUDIT_RESPONSE_DRPACKET:
SQACAAuditData.MachineType[0] = ucACADataBuf[1];
SQACAAuditData.ControlSVN = ucACADataBuf[2];
SQACAAuditData.VendPrice[0] = ucACADataBuf[3];
SQACAAuditData.VendPrice[1] = ucACADataBuf[4];
SQACAAuditData.Coin1Value [0] = ucACADataBuf[5];
SQACAAuditData.Coin1Value[1] = ucACADataBuf[6];
SQACAAuditData.Coin2Value [0] = ucACADataBuf[7];
SQACAAuditData.Coin2Value[1] = ucACADataBuf[8];
SQACAAuditData.NumStartPulse[0] = ucACADataBuf[9];
SQACAAuditData.NumStartPulse [1]= ucACADataBuf[10];
SQACAAuditData.CycleConfig = ucACADataBuf[11];
if (ucACADataBuf[0] == AUDIT_DATA_FRONTLOAD || ucACADataBuf[0] == AUDIT_DATA_TOPLOAD )
{
SQACAAuditData.ControlConfig = ucACADataBuf[12];
SQACAAuditData.Coin1Counter[0] = ucACADataBuf[13];
SQACAAuditData.Coin1Counter[1] = ucACADataBuf[14];
SQACAAuditData.Coin2Counter[0] = ucACADataBuf[15];
SQACAAuditData.Coin2Counter[1] = ucACADataBuf[16];
SQACAAuditData.NumCycles[0] = ucACADataBuf[17];
SQACAAuditData.NumCycles[1] = ucACADataBuf[18];
SQACAAuditData.NumStartPulse[0] = ucACADataBuf[19];
SQACAAuditData.NumStartPulse[1] = ucACADataBuf[20];
SQACAAuditData.AvgFillTime[0] = ucACADataBuf[21];
SQACAAuditData.AvgFillTime[1] = ucACADataBuf[22];
SQACAAuditData.AvgDrainTime[0] = ucACADataBuf[23];
SQACAAuditData.AvgDrainTime[1] = ucACADataBuf[24];
}
else if (ucMdcDataBuf[0] == AUDIT_DATA_DRYER)
{
MdcAuditData.CoolDownTime = ucMdcDataBuf[12];
MdcAuditData.Coin1TopoffTime = ucMdcDataBuf[13];
MdcAuditData.Coin2TopoffTime = ucMdcDataBuf[14];
MdcAuditData.HighTempSetting = ucMdcDataBuf[15];
MdcAuditData.MedTempSetting = ucMdcDataBuf[16];
MdcAuditData.LowTempSetting = ucMdcDataBuf[17];
MdcAuditData.DeliTempSetting = ucMdcDataBuf[18];
MdcAuditData.ControlConfig = ucMdcDataBuf[19];
MdcAuditData.Coin1Counter[0] = ucMdcDataBuf[20];
MdcAuditData.Coin1Counter[1] = ucMdcDataBuf[21];
MdcAuditData.Coin2Counter[0] = ucMdcDataBuf[22];
MdcAuditData.Coin2Counter[1] = ucMdcDataBuf[23];
MdcAuditData.NumCycles[0] = ucMdcDataBuf[24];
MdcAuditData.NumCycles[1] = ucMdcDataBuf[25];
MdcAuditData.Coin1TopoffCounter[0] = ucMdcDataBuf[26];
MdcAuditData.Coin1TopoffCounter[1] = ucMdcDataBuf[27];
MdcAuditData.Coin2TopoffCounter[0] = ucMdcDataBuf[28];
MdcAuditData.Coin2TopoffCounter[1] = ucMdcDataBuf[29];
MdcAuditData.NumStartPulse[0] = ucMdcDataBuf[30];
MdcAuditData.NumStartPulse[1] = ucMdcDataBuf[31];
MdcAuditData.NumTopoffStartPulses[0] = ucMdcDataBuf[32];
MdcAuditData.NumTopoffStartPulses[1] = ucMdcDataBuf[33];
}
*/
/*
break;

default:
break;
}

return 1;
}
}
return 0;
}
*/



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
/*
void SetMachineSetupData(void)
{
//get setup data from EEPROM
halGetEeprom(MACHINE_SETUP_ADDR, MACHINE_SETUP_NUM_BYTE, (u8*)&SQACAToploadProgramming);
vendPrice = (SQACAToploadProgramming.VendPrice1[0] * 256 + SQACAToploadProgramming.VendPrice1[1]) / 100.0;
/*
if(SQACAMachineStatus.MachineType[0] == DRYER)
halGetEeprom(MACHINE_SETUP_ADDR_DRYER, DRYER_SETUP_NUM_BYTE, &SQACAToploadProgramming.CoolDownTime);//*/
/*
if(SQACAMachineStatus.MachineType[1] == ACA_SERIES)
halGetEeprom(MACHINE_SETUP_ADDR_QUANTUM, QUANTUM_SETUP_NUM_BYTE, (u8*)&SQACAToploadProgramming);

}

void SaveMachineSetupData(void)
{
}
*/

void SetDefaultMachineSetup(void)
{
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
