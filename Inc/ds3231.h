/*
	Driver for DS3231 RTC
	Written by: Liam Burns
*/

#ifndef DS3231_H
#define DS3231_H

#define SADDR 0x68U
#define YEAR_REG 0x06U
#define MON_REG 0x05U
#define DATE_REG 0x04
#define HOUR_REG 0x02
#define MIN_REG 0x01
#define SEC_REG 0x00
#define TPH_REG 0x11
#define TPL_REG 0x12

#include "stm32f446xx.h"
#include "i2c.h"

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t month;
    uint8_t year;
} DateTime;

void get_datetime(DateTime *dt);
void set_datetime(DateTime dt);

float get_temperature();

uint8_t BCD_To_Decimal(uint8_t bcd);
uint8_t Decimal_To_BCD(uint8_t decimal);

#endif