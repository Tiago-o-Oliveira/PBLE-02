#include "i2c.h"
#include "julinho.h"

//endereço do dispositivo, deslocado por causa do bit de RW
#define DS1307_CTRL_ID (0x50<<1)
#define I2C_WRITE 0
#define I2C_READ  1


void julinhoInit(void) {
	i2cInit();
}

void julinhoWriteData(unsigned char value, int address) {
	i2cWriteByte(1,0, DS1307_CTRL_ID|I2C_WRITE);
	i2cWriteByte(0,0,address>>8);
    i2cWriteByte(0,0,address);
	i2cWriteByte(0,1,value);
    for(float i = 0;i<500;i++);
}
int julinhoReadData(int address) {
	int result;
	i2cWriteByte(1,0,DS1307_CTRL_ID | I2C_WRITE);
	i2cWriteByte(0,0,address>>8);
    i2cWriteByte(0,0,address);
	i2cWriteByte(1,0, DS1307_CTRL_ID | I2C_READ);
	result = i2cReadByte(1,1 );
	return result;
}
