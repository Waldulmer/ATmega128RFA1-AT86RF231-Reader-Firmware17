/*
 * display.h
 *
 * Created: 7/30/2011 1:08:00 PM
 *  Author: Paul Bastien
 *	Copyright (C) 2011
 * updated to Seven-Segment Display 170226 
 */ 

//#include "application.h"

#ifndef DISPLAY_H_
#define DISPLAY_H_


void displayMsg(u8 msg);
void displayVersion(void);

//led defines
#define LED_0     0x3f
#define LED_1     0x06
#define LED_2     0x5B
#define LED_3     0x4F
#define LED_4     0x66
#define LED_5     0x6D
#define LED_6     0x7D
#define LED_7     0x07
#define LED_8     0x7F
#define LED_9     0x6F

#define LED_A     0x77
#define LED_B     0xfe
#define LED_b     0x7C
#define LED_C     0x39
#define LED_c     0x58
#define LED_d     0x5E
#define LED_E     0x79
#define LED_F     0x71
#define LED_H     0x76 
#define LED_I     0x30
#define LED_i     0x08
#define LED_L     0x38
#define LED_N     0x88
#define LED_n     0x54
#define LED_o     0x5C
#define LED_P     0x73
#define LED_r     0x50
#define LED_S     0x6D
#define LED_t     0x78
#define LED_U     0x3E
#define LED_u     0x1C
#define LED_Y     0x6E

#define LED_BLANK 0x00
#define LED_DASH  0X80 
#define NUM_BLANK 0x10           // index into msgNumber[] for blank


#define VERSION								LED_r, '__APP_MAJOR__',  '__APP_MINOR__', '__APP_REVISION__', 5		//version number to be displayed for 4 sec.
#define NOSETUP								('n', 'o',  'S', 'u', 1)		//No setup displayed for 1 sec.
#define USER_TRANS_FULL						('U', 'C', 'F', 'L', 1)			//user transaction is full. Collection is required
#define DATA_COLLECTION						(BLANK,d,c,BLANK,2)				//data Collection display dc

// message types

#define CARD_ERR_MSG	1
#define SETUP_MSG		2
#define SERVICE_MSG		3
#define BUSY_MSG		4
#define BLANK_MSG		5
#define PPOS_MSG		6
#define TOPOFF_MSG		7


// message table lookup index
enum{need = 0, ser, card};
	
//external types
extern u8 msgNumber[];
	
#endif /* DISPLAY_H_ */