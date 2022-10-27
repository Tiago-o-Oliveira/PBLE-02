#include "i2c.h"
#include "mcc_generated_files/pin_manager.h"
#include <xc.h>

#define bitSet(arg,bit) ((arg) |=  (1<<(bit)))
#define bitClr(arg,bit) ((arg) &= ~(1<<(bit)))

#define SDA_OFF()   bitClr(PORTB,9)
#define SDA()       PORTBbits.RB9
#define SDA_IN()    IO_RB9_SetDigitalInput()
#define SDA_OUT()   IO_RB9_SetDigitalOutput()

#define SCL_OFF()   bitClr(PORTB,8)
#define SCL()       PORTBbits.RB8
#define SCL_IN()    IO_RB8_SetDigitalInput()
#define SCL_OUT()   IO_RB8_SetDigitalOutput()



void i2cInit(void) {

	SDA_IN();
	SCL_IN();
}

// Hardware-specific support functions that MUST be customized:
#define I2CSPEED 4
void I2C_delay(void) {
	volatile int v;
	int i;
    //comment on simulator
	for (i = 0; i < I2CSPEED/2; i++) v;
}

unsigned char read_SCL(void) // Set SCL as input and return current level of line, 0 or 1
{
	SCL_IN();
	return !!SCL();
}
unsigned char read_SDA(void) // Set SDA as input and return current level of line, 0 or 1
{
	SDA_IN();
	return !!SDA();
}
void clear_SCL(void) // Actively drive SCL signal low
{
	SCL_OUT();
	SCL_OFF();
}

void clear_SDA(void) // Actively drive SDA signal low
{
	SDA_OUT();
	SDA_OFF();
}

unsigned char started = 0; // global data
void i2c_start(void) {
	//se j? estiver iniciado, prepara para reenviar o bit de start
	if (started) { 
		read_SDA();
		I2C_delay();
		while (read_SCL() == 0); // Clock stretching
		// Repeated start setup time, minimum 4.7us
		I2C_delay();
	}
	if (read_SDA() == 0) {
		//Houve problema de comunica??o
	}
	// SCL is high, set SDA from 1 to 0.
	clear_SDA();
	I2C_delay();
	clear_SCL();
	started = 1;
}

void i2c_stop(void) {
	// set SDA to 0
	clear_SDA();
	I2C_delay();
	while (read_SCL() == 0); // Clock stretching
	I2C_delay();
	if (read_SDA() == 0) {
		//arbitration_lost();
	}
	I2C_delay();
	started = 0;
}

void i2c_write_bit(unsigned char bits) {
	if (bits) {
		read_SDA();
	} else {
		clear_SDA();
	}
	I2C_delay();
	while (read_SCL() == 0); // Clock stretching
	if (bits && read_SDA() == 0) {
		//Houve problema de comunica??o
	}
	I2C_delay();
	clear_SCL();
	I2C_delay();
}

unsigned char i2c_read_bit(void) {
	unsigned char bits;
	read_SDA();
	I2C_delay();
	while (read_SCL() == 0); // Clock stretching
	bits = read_SDA();
	I2C_delay();
	clear_SCL();
	I2C_delay();
	return bits;
}

// Write a byte to I2C bus. Return 0 if ack by the slave.
unsigned char i2cWriteByte(unsigned char send_start, unsigned char send_stop, unsigned char byte) {
	unsigned char bits;
	unsigned char nack;
	if (send_start) {
		i2c_start();
	}
	for (bits = 0; bits < 8; bits++) {
		i2c_write_bit((byte & 0x80) != 0);
		byte <<= 1;
	}
	nack = i2c_read_bit();
	if (send_stop) {
		i2c_stop();
	}
	return nack;
}

// Read a byte from I2C bus
unsigned char i2cReadByte(unsigned char nack, unsigned char send_stop) {
	unsigned char byte = 0;
	unsigned bits;
	for (bits = 0; bits < 8; bits++) {
		byte = (byte << 1) | i2c_read_bit();
	}
	i2c_write_bit(nack);
	if (send_stop) {
		i2c_stop();
	}
	return byte;
}


