#ifndef KEYPAD_H
#define KEYPAD_H

#define KEY_0 0
#define KEY_STAR 10
#define KEY_HASH 12
#define KEY_NONE 0xFF

#include "stm32f4xx.h"
#include "systick.h"
#include <stdint.h>

uint8_t Read_Keypad(void);

#endif