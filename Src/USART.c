#include "stm32f4xx.h"
//#include "project_configuration.h"
#include "USART.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/***************************************************************
* Brief:  Sets up the USART2 for pin PA2
* param:  None   
* return: None
****************************************************************/

void USART_Init(void)
{
	RCC->AHB1ENR |= 1;
	RCC->APB1ENR |= 0x20000;

	GPIOA->AFR[0] |= 0x0700;
	GPIOA->MODER |= 0x0020;
	
	USART2->BRR = 0x0683;
	USART2->CR1 = 0x0008;
	USART2->CR2 = 0x0000;
	USART2->CR3 = 0x0000;
	USART2->CR1 |= 0x2000;
}
/***************************************************************
* Brief:  waits for Tx to be open then sends one character
* param:  one character 
* return: None
****************************************************************/
void USART2_write_char (int ch)
{
	while(!(USART2->SR & 0x0080)) {}
	USART2->DR = (ch & 0xFF);
}
/***************************************************************
* Brief:  Loops through the characters in the string and sends
* each one through USART. 
* param:  string line 
* return: None
****************************************************************/
void USART2_write(char *line)
{
	/*
	for(uint8_t u8_string_inc = 0; u8_string_inc < 50; u8_string_inc++)
	{
		USART2_write_char(line[u8_string_inc]);
	}
	*/
	    uint8_t i = 0;

    while (line[i] != '\0')
    {
        USART2_write_char(line[i]);
        i++;
    }
}
