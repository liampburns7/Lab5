/*
	Driver for I2C code
	Written by: Dr. Kandalaft
*/

#ifndef I2C_H
#define I2C_H

#include "stm32f446xx.h"

void I2C_init(void);

uint8_t I2C1_byteWrite(char saddr, char maddr, char data);
uint8_t I2C1_byteRead(char saddr, char maddr, char *data);

#endif