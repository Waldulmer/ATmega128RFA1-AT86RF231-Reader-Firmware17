/*
 * i2cbitbang.h
 *
 * Created: 3/22/2012 10:38:04 PM
 *  Author: rocendob
 */ 


#ifndef I2CBITBANG_H_
#define I2CBITBANG_H_

#define SDAPIN  IOPORT_CREATE_PIN(PORTC,0)
#define SCLPIN  IOPORT_CREATE_PIN(PORTC,1)

// I2C Xfer modes
#define XFER_DATA 	0x01
#define XFER_READ	0x02
#define XFER_WRITE	0x03


void i2cSetSDA(void);
void i2cClrSDA(void);
void i2cSetSCL(void);
void i2cClrSCL(void);

unsigned char geti2cPinVal(port_pin_t i2cPin);
void i2cBusInit(void);
void i2c_delay(void);
unsigned char i2c_rx(char ack);
unsigned char i2c_tx(unsigned char data, unsigned char xfer);

void i2c_start(void);
void i2c_stop(void);


#endif /* I2CBITBANG_H_ */