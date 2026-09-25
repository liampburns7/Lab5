/*
	Driver for USART code
	Written by: Kaitlynn Hudenko and Joe Berkheiser
*/
#include "stm32f4xx.h"
#include <stdint.h>
#include <string.h>
#ifndef _USART_CODE_
#define _USART_CODE_

void USART_Init(void);
void USART2_write_char (int ch);
void USART2_write(char *line);


#endif
