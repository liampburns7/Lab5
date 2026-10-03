#include "I2C.h"

void I2C_init(void)
{
    RCC->AHB1ENR |= 2;              

    GPIOB->MODER &= ~0x000F0000;
    GPIOB->MODER |=  0x000A0000;    // PB8, PB9 use Alternate Function

    GPIOB->AFR[1] &= ~0x000000FF;
    GPIOB->AFR[1] |=  0x00000044;   // PB8, PB9 I2C SCL SDA

    GPIOB->OTYPER |= 0x00000300;    // Output Open Drain

    RCC->APB1ENR |= 1 << 21;       

    I2C1->CR1 = 0x8000;             // SW Reset I2C1
    I2C1->CR1 &= ~0x8000;           // Out of reset

    I2C1->CR2 = 0x0010;             // Periph Clock @ 16MHz
    I2C1->CCR = 80;                 // Standard Mode, 100kHz
    I2C1->TRISE = 17;               // Max trise

    I2C1->CR1 |= 0x0001;            // Enable I2C1
}

int I2C1_byteWrite(char saddr, char maddr, char data) {
    /* Write information to given memory address on slave
    *
    * Input:
    *   saddr - slave address
    *   maddr - memory address
    *   data  - write data
    * Return:
    *   0 when done
    */
    volatile int tmp;

    while(I2C1->SR2 & 2);       // Wait until bus not busy

    I2C1->CR1 |= 0x100;         // Generate start bit
    while(!(I2C1->SR1 & 1));    // Wait until start flag is set

    I2C1->DR = saddr << 1;      // Txmit slave address
    while(!(I2C1->SR1 & 2));    // Wait until addr flag set
    tmp = I2C1->SR2;            // Clear addr flag

    while(!(I2C1->SR1 & 0x80)); // Wait until data reg empty
    I2C1->DR = maddr;           // Send memory address

    while(!(I2C1->SR1 & 0x80)); // Wait until data reg empty
    I2C1->DR = data;            // Send data

    while(!(I2C1->SR1 & 4));    // Wait until data reg empty
    I2C1->CR1 |= 0x200;         // Generate stop

    return 0;
}

int I2C1_byteRead(char saddr, char maddr, char *data)
{
    volatile int tmp;

    while(I2C1->SR2 & 2);       // Wait until bus not busy

    I2C1->CR1 |= 0x100;         // Generate start bit
    while(!(I2C1->SR1 & 1));    // Wait until start flag is set

    I2C1->DR = saddr << 1;      // Txmit slave address
    while(!(I2C1->SR1 & 2));    // Wait until addr flag set
    tmp = I2C1->SR2;            // Clear addr flag

    while(!(I2C1->SR1 & 0x80)); // Wait until data reg empty
    I2C1->DR = maddr;           // Send memory address

    while(!(I2C1->SR1 & 0x80)); // Wait until data reg empty

    I2C1->CR1 |= 0x100;         // Generate restart
    while(!(I2C1->SR1 & 1));    // Wait until start flag set

    I2C1->DR = (saddr << 1) | 1;    // Txmit slave addr and read
    while(!(I2C1->SR1 & 2));        // Wait until addr flag set

    I2C1->CR1 &= ~0x400;        // DISABLE ack
    tmp = I2C1->SR2;            // clear addr flag

    I2C1->CR1 |= 0x200;         // Generate stop after data recv

    while(!(I2C1->SR1 & 0x40)); // Wait until RXNE flag set
    *data = I2C1->DR;           // Read data from DR

    return 0;
}