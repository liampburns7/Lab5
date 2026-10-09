#include "ds3231.h"
#include "I2C.h"

void get_datetime(DateTime *dt) {
  uint8_t raw;

  I2C1_byteRead(SADDR, SEC_REG, &raw);
  dt->seconds = BCD_To_Decimal(raw & 0x7F);

  I2C1_byteRead(SADDR, MIN_REG, &raw);
  dt->minutes = BCD_To_Decimal(raw & 0x7F);
  
  I2C1_byteRead(SADDR, HOUR_REG, &raw);
  dt->hours = BCD_To_Decimal(raw & 0x3F);

  I2C1_byteRead(SADDR, DATE_REG, &raw);
  dt->day = BCD_To_Decimal(raw & 0x3F);

  I2C1_byteRead(SADDR, MON_REG, &raw);
  dt->month = BCD_To_Decimal(raw & 0x1F);

  I2C1_byteRead(SADDR, YEAR_REG, &raw);
  dt->year = BCD_To_Decimal(raw);
}

void set_datetime(DateTime dt)
{
  I2C1_byteWrite(SADDR, SEC_REG, Decimal_To_BCD(dt.seconds) & 0x7F);

  I2C1_byteWrite(SADDR, MIN_REG, Decimal_To_BCD(dt.minutes) & 0x7F);

  // 24-hour mode: bit 6 must be 0
  I2C1_byteWrite(SADDR, HOUR_REG, Decimal_To_BCD(dt.hours) & 0x3F);

  I2C1_byteWrite(SADDR, DATE_REG, Decimal_To_BCD(dt.day) & 0x3F);

  // Month in bits 4:0, century bit 7 left clear
  I2C1_byteWrite(SADDR, MON_REG, Decimal_To_BCD(dt.month) & 0x1F);

  I2C1_byteWrite(SADDR, YEAR_REG, Decimal_To_BCD(dt.year));
}

float get_temperature(void)
{
    uint8_t upper;
    uint8_t lower;

    I2C1_byteRead(SADDR, TPH_REG, &upper);
    I2C1_byteRead(SADDR, TPL_REG, &lower);

    int8_t integer_part = (int8_t)upper;
    float fractional_part = (lower >> 6) * 0.25f;

    return integer_part + fractional_part;
}

uint8_t BCD_To_Decimal(uint8_t bcd)
{
  return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

uint8_t Decimal_To_BCD(uint8_t decimal)
{
  return ((decimal / 10) << 4) | (decimal % 10);
}