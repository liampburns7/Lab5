#include "systick.h"

void delay_ms(uint32_t ms) {
    /*
     * Blocking millisecond delay using SysTick.
     * Assumes a 16 MHz processor clock.
     */

    SysTick->CTRL = 0;          // Disable SysTick
    SysTick->LOAD = 16000 - 1;  // 1 ms @ 16 MHz
    SysTick->VAL  = 0;          // Clear current count
    SysTick->CTRL = 0x5;        // Enable SysTick, processor clock, no interrupts

    for (uint32_t i = 0; i < ms; i++) {
        while (!(SysTick->CTRL & (1U << 16))); // Wait for COUNTFLAG
    }

    SysTick->CTRL = 0;          // Disable SysTick
}