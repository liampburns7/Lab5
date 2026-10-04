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
#include "ds3231.h"

#include <stdbool.h>
#include <stdio.h>

typedef enum {
    Choose_Action,
    Prompt_Data,
    Write_RTC,
    Read_RTC,
    Display_Data
} program_state_t;

void GPIO_Init(void);
void EXTI_Init(void);

uint8_t Read_Digit(bool special_allowed, bool echo);
uint16_t Read_Digits(uint8_t num_digits);
uint8_t Prompt_Ranged_Number(uint8_t min, uint8_t max, uint8_t num_digits);

volatile bool kpd_event = 0;
uint8_t last_input = 0;

uint8_t valid_days_by_month[12] = {
    31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31
};

char line[64];
char curr_prompt[64];

program_state_t program_state = Choose_Action;

int main(void)
{
  USART_Init();
  I2C_init();
  GPIO_Init();
  EXTI_Init();

  DateTime rtc;

  while(true) {
    switch (program_state) {
      case Choose_Action:
        while (true) {
          sprintf(curr_prompt, "\n\n\nEnter # to set RTC Data. Enter * to display RTC data: ");
          USART2_write(curr_prompt);
          // Read digit, special allowed
          last_input = Read_Digit(true, true);
          USART2_write("\n");
          
          if (last_input == KEY_STAR) {
            program_state = Read_RTC;
            break;
          }

          if (last_input == KEY_HASH) {
            program_state = Prompt_Data;
            break;
          }

          USART2_write("Invalid Entry.");
        }

        break;

      case Prompt_Data:
        
        // Prompt for month
        sprintf(curr_prompt, "Enter Current Year (2 Digits): ");
        rtc.year = Prompt_Ranged_Number(0, 99, 2);
        USART2_write("\n");

        // Prompt for month
        sprintf(curr_prompt, "Enter Current Month (2 Digits): ");
        rtc.month = Prompt_Ranged_Number(1, 12, 2);
        USART2_write("\n");

        // Prompt for day of month
        sprintf(curr_prompt, "Enter Current Day (2 Digits): ");
        rtc.day = Prompt_Ranged_Number(1, valid_days_by_month[rtc.month - 1], 2);
        USART2_write("\n");

        // Prompt for hour
        sprintf(curr_prompt, "Enter Current Hour (2 Digits): ");
        rtc.hours = Prompt_Ranged_Number(0, 23, 2);
        USART2_write("\n");

        // Prompt for minute
        sprintf(curr_prompt, "Enter Current Minute (2 Digits): ");
        rtc.minutes = Prompt_Ranged_Number(0, 59, 2);
        USART2_write("\n");

        // Prompt for second
        sprintf(curr_prompt, "Enter Current Second (2 Digits): ");
        rtc.seconds = Prompt_Ranged_Number(0, 59, 2);
        USART2_write("\n");

        // Display Confirmation
        sprintf(line, "%02d/%02d/%02d, %02d:%02d:%02d", 
          rtc.month, rtc.day, rtc.year,
          rtc.hours, rtc.minutes, rtc.seconds);
        
        USART2_write("Entered Date: ");
        USART2_write(line);
        USART2_write("\n");
        
        program_state = Write_RTC;

        break;

      case Write_RTC:

        set_datetime(rtc);
        USART2_write("Date + Time Written to RTC.\n");

        program_state = Choose_Action;

        break;

      case Read_RTC:
        
        USART2_write("Retrieving RTC Date + Time Data...\n");
        get_datetime(&rtc);
        
        program_state = Display_Data;

        break;

      case Display_Data:

        sprintf(line, "%02d/%02d/%02d, %02d:%02d:%02d", 
          rtc.month, rtc.day, rtc.year,
          rtc.hours, rtc.minutes, rtc.seconds);
        USART2_write(line);
        USART2_write("\n");

        program_state = Choose_Action;

        break;

      default:
        break;

    }
  }

  // Address of DS3231: 0x68
}

void GPIO_Init(void) 
{
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

uint8_t Read_Digit(bool special_allowed, bool echo) 
{
  uint8_t key;

  do {
    while (!kpd_event);
    kpd_event = false;

    key = Read_Keypad();

  } while ( // Ignore empty reads and disallowed special keys
    key == KEY_NONE ||
    (!special_allowed && (key == KEY_STAR || key == KEY_HASH))
  );

  // Echo back entered digit if echo is true
  if (echo) {
    if (key == KEY_STAR) {
      USART2_write("*");
    }
    else if (key == KEY_HASH) {
      USART2_write("#");
    }
    else {
      sprintf(line, "%d", key);
      USART2_write(line);
    }
  }

  return key;
}

uint16_t Read_Digits(uint8_t num_digits) 
{
  uint16_t num = 0;

  for (int i = 0; i < num_digits; i++) {
    // Prompt for the next digit until num_digits = 0
    // First digit entered -> Most significant digit (base 10)
    num = num * 10 + Read_Digit(false, true); 
  }

  return num;
}

uint8_t Prompt_Ranged_Number(uint8_t min, uint8_t max, uint8_t num_digits) 
{
  uint16_t num;
  bool valid = false;

  do {
    USART2_write(curr_prompt);
    num = Read_Digits(num_digits);

    valid = (num >= min && num <= max);

    if (!valid) {
      USART2_write("\nInvalid Entry!\n");
    }
  } while (!valid);
  
  return num;
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