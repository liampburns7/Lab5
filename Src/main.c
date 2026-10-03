/* 
* EGR326 Lab 5
* Interfacing with a Real Time Clock (RTC) IC Using An I2C Bus
* 
* Date: 10/2/26
* 
* Authors: Liam Burns, Matthew Knorr
*/

#include "stm32f446xx.h"
#include "USART.h"
#include "i2c.h"
#include "keypad.h"

#include <stdbool.h>
#include <stdio.h>

void GPIO_Init(void);
void EXTI_Init(void);

volatile bool kpd_event = 0;

int main(void)
{
  USART_Init();
  I2C_init();
  EXTI_Init();
  GPIO_Init();

  GPIOC->BSRR = (0xF << 16);   // PC0-PC3 LOW

  uint8_t kpd_in, num;
  char line[32];

  USART2_write("Hello, World!");

  while(true) {
    if (kpd_event) {
      kpd_event = false;

      kpd_in = Read_Keypad();

      if (kpd_in != 0) {  // Guard against 0
        num = kpd_in;
      }
    }
  }

  // Address of DS3231: 0x68
}

void GPIO_Init(void) {
  /* GPIO Initialization 
  *  
  * Configures the GPIO pins shown below for their
  * intended use.
  * 
  * Pinout:
  * - PC0-PC3: Keypad Rows
  * - PC4-PC6: Keypad Columns
  */
  RCC->AHB1ENR |= 0x4;        // GPIOC clock enable

  /* PC0-PC3 = inputs, PC4-PC6 = outputs */
  GPIOC->MODER &= ~(0x3FFF);
  GPIOC->MODER |= 0x1500;

  /* Pull-ups on PC0-PC3 */
  GPIOC->PUPDR &= ~(0xFFFF);
  GPIOC->PUPDR |= 0x0055;

  /* Drive PC4-PC6 LOW while idle */
  GPIOC->BSRR = (0x7U << 20);
}


void EXTI_Init(void)
{
  RCC->APB2ENR |= 0x4000;

  /* Route EXTI0-3 to GPIOC */
  SYSCFG->EXTICR[0] &= ~0xFFFFU;
  SYSCFG->EXTICR[0] |=  0x2222U;

  /* Unmask EXTI0-3 */
  EXTI->IMR |= 0x0F;

  /* Falling-edge trigger */
  EXTI->RTSR &= ~0x0F;
  EXTI->FTSR |=  0x0F;

  /* Clear pending flags */
  EXTI->PR = 0x0F;

  /* Enable EXTI0-3 interrupts */
  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_EnableIRQ(EXTI1_IRQn);
  NVIC_EnableIRQ(EXTI2_IRQn);
  NVIC_EnableIRQ(EXTI3_IRQn);
}

void EXTI9_5_IRQHandler(void) {
  /* Keypad rising edge detected */
  if (EXTI->PR & ((1U << 6) | (1U << 5))) {
    EXTI->PR = (1U << 6) | (1U << 5);   // clear EXTI5,6

    // Set global event flag to be handled in main
    kpd_event = true;
  }
}

void EXTI0_IRQHandler(void)
{
    EXTI->PR = (1U << 0);
    kpd_event = true;
}

void EXTI1_IRQHandler(void)
{
    EXTI->PR = (1U << 1);
    kpd_event = true;
}

void EXTI2_IRQHandler(void)
{
    EXTI->PR = (1U << 2);
    kpd_event = true;
}

void EXTI3_IRQHandler(void)
{
    EXTI->PR = (1U << 3);
    kpd_event = true;
}