#include "keypad.h"

uint8_t Read_Keypad(void)
{
  uint8_t curr_col, row_data, num;

  /* Disable keypad interrupts during scan */
  EXTI->IMR &= ~0x0F;

  /* Iterate through all columns */
  for (curr_col = 0; curr_col < 3; curr_col++) {

    GPIOC->MODER &= ~(0x3F00);                     // All columns Hi-Z

    GPIOC->MODER |=
        (0x1U << ((curr_col + 4) * 2));            // Current column output

    GPIOC->BSRR =
        (0x1U << ((curr_col + 4) + 16));           // Drive column LOW

    delay_ms(10);  // Debounce press

    row_data = GPIOC->IDR & 0x0F;

    if (row_data != 0x0F) break;
  }

  if (curr_col == 3) {
    num = KEY_NONE;
  }
  else {
    if (row_data == 0x0E) num = curr_col + 1;
    if (row_data == 0x0D) num = curr_col + 4;
    if (row_data == 0x0B) num = curr_col + 7;
    if (row_data == 0x07) num = curr_col + 10;

    while ((GPIOC->IDR & 0x0F) != 0x0F);
  }

  if (num == 11) {
    num = 0;
  }

  /* Restore idle condition: PC4-PC6 outputs LOW */
  GPIOC->MODER &= ~(0x3F00);
  GPIOC->MODER |= 0x1500;

  GPIOC->BSRR = (0x7U << 20);

  /* Throw away interrupts caused by scanning */
  EXTI->PR = 0x0F;

  /* Re-enable keypad interrupts */
  EXTI->IMR |= 0x0F;

  return num;
}