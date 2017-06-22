

#include "asf.h"
#include "i2cbitbang.h"

#define i2cSetSDA() ioport_configure_pin(SDAPIN, IOPORT_DIR_INPUT)
#define i2cClrSDA() ioport_configure_pin(SDAPIN, IOPORT_DIR_OUTPUT)
#define i2cSetSCL() ioport_configure_pin(SCLPIN, IOPORT_DIR_INPUT)
#define i2cClrSCL() ioport_configure_pin(SCLPIN, IOPORT_DIR_OUTPUT)

/*
void i2cSetSDA(void) {
    ioport_configure_pin(SDAPIN, IOPORT_DIR_INPUT);
}

void i2cClrSDA(void) {
   ioport_configure_pin(SDAPIN, IOPORT_DIR_OUTPUT);
}

void i2cSetSCL(void) {
   ioport_configure_pin(SCLPIN, IOPORT_DIR_INPUT);
}

void i2cClrSCL(void) {
   ioport_configure_pin(SCLPIN, IOPORT_DIR_OUTPUT);
}

*/
unsigned char geti2cPinVal(port_pin_t i2cPin) {
  
  if (gpio_pin_is_high(i2cPin)) {
     return 1;
  } else {
     return 0;
  }
   
}



void i2c_delay(void) {
  int i;
   while (i++ < 4) {
      asm volatile ("nop");
   } 

/* 
 asm volatile ("nop");
 asm volatile ("nop");
 asm volatile ("nop");
 asm volatile ("nop");
*/ 
}


void i2c_start(void)
 { 
   i2cSetSDA();
   i2c_delay();
   i2cSetSCL(); 
   i2c_delay();
   i2cClrSDA(); 
   i2c_delay();
   i2cClrSCL(); 
   i2c_delay();
 }

 
 
void i2c_stop(void)
 {
   i2cClrSDA(); 
   i2c_delay();
   i2cSetSCL(); 
   i2c_delay();
   i2cSetSDA();
   i2c_delay();
 }
 
 

unsigned char i2c_rx(char ack)
 {
 
 volatile char x, d=0;
 
   i2cSetSDA(); 
 
   for(x=0; x<8; x++) {
       d <<= 1;
       
       do {
           i2cSetSCL(); 
          }
     
       while(geti2cPinVal(SCLPIN) == 0);
       
       i2c_delay();
       
       if(geti2cPinVal(SDAPIN)){
	       d |= 1;
         }
         
       i2cClrSCL(); 
       
       i2c_delay();
   }
   
   
   if(ack) {
       i2cClrSDA();
   } else {
       i2cSetSDA();
   }
   
   i2cSetSCL(); 
   i2c_delay();            
   i2cClrSCL(); 
   i2c_delay();
   i2cSetSDA();
   i2c_delay();
   
   return d;
 }

 
 
unsigned char i2c_tx(unsigned char data, unsigned char xfer)
 {
   
 volatile unsigned char x;
 volatile unsigned char b;
 volatile unsigned char d;
 

  if (xfer == XFER_DATA){
       d = data;
  } else if (xfer == XFER_READ){
      d = data | 0x01;
  } else if (xfer == XFER_WRITE) {
      d = data & 0xFE;
  } else {
      d = data;
  }
  
  
  for(x=0; x<8; x++) {
     
      if(d&0x80) {
        i2cSetSDA();
      } else {
	    i2cClrSDA(); 
      }
        i2c_delay();
        i2cSetSCL(); 
        d <<= 1;
	    i2c_delay();
        i2cClrSCL(); 
    }
   
   i2c_delay();
   i2cSetSDA();
   i2c_delay();
   i2cSetSCL(); 
   //i2c_delay();
   while(!geti2cPinVal(SCLPIN))
   ;
   b = !(geti2cPinVal(SDAPIN)); 
   i2cClrSCL(); 
   i2c_delay();

   return b;
 }
 
 