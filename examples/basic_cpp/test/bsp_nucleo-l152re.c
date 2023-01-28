/*============================================================================
* ET: embedded test; BSP for STM32 NUCLEO-L152RE
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005 Quantum Leaps, <state-machine.com>.
*
* SPDX-License-Identifier: MIT
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
============================================================================*/
#include "et.h" /* ET: embedded test */

#include "stm32l1xx.h"  /* CMSIS-compliant header file for the MCU used */
/* add other drivers if necessary... */

/* Local-scope objects -----------------------------------------------------*/
/* LED pins available on the board (just one user LED LD2--Green on PA.5) */
#define LD2_PIN  5U

/* USART2 pins PA.2 and PA.3 */
#define USART2_TX_PIN 2U
#define USART2_RX_PIN 3U

/* Button pins available on the board (just one user Button B1 on PC.13) */
#define B1_PIN   13U

/* USART2 for ET output */
#define __DIV(__PCLK, __BAUD)       (((__PCLK / 4) *25)/(__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   \
    (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) \
        * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) \
    ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

/*..........................................................................*/
void ET_onInit(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    /* NOTE: SystemInit() has been already called from the startup code
    *  but SystemCoreClock needs to be updated
    */
    SystemCoreClockUpdate();

    /* enable GPIOA clock port for the LED LD2 */
    RCC->AHBENR |= (1U << 0U);

    /* configure LED (PA.5) pin as push-pull output, no pull-up, pull-down */
    GPIOA->MODER   &= ~((3U << 2U*LD2_PIN));
    GPIOA->MODER   |=  ((1U << 2U*LD2_PIN));
    GPIOA->OTYPER  &= ~((1U <<    LD2_PIN));
    GPIOA->OSPEEDR &= ~((3U << 2U*LD2_PIN));
    GPIOA->OSPEEDR |=  ((1U << 2U*LD2_PIN));
    GPIOA->PUPDR   &= ~((3U << 2U*LD2_PIN));

    /* enable peripheral clock for USART2 */
    RCC->AHBENR   |=  (1U <<  0U);   /* Enable GPIOA clock   */
    RCC->APB1ENR  |=  (1U << 17U);   /* Enable USART#2 clock */

    /* Configure PA3 to USART2_RX, PA2 to USART2_TX */
    GPIOA->AFR[0] &= ~((15U << 4U*USART2_RX_PIN) | (15U << 4U*USART2_TX_PIN));
    GPIOA->AFR[0] |=  (( 7U << 4U*USART2_RX_PIN) | ( 7U << 4U*USART2_TX_PIN));
    GPIOA->MODER  &= ~(( 3U << 2U*USART2_RX_PIN) | ( 3U << 2U*USART2_TX_PIN));
    GPIOA->MODER  |=  (( 2U << 2U*USART2_RX_PIN) | ( 2U << 2U*USART2_TX_PIN));

    USART2->BRR  = __USART_BRR(SystemCoreClock, 115200U); /* baud rate */
    USART2->CR3  = 0x0000U;        /* no flow control          */
    USART2->CR2  = 0x0000U;        /* 1 stop bit               */
    USART2->CR1  = ((1U <<  2U) |  /* enable RX                */
                    (1U <<  3U) |  /* enable TX                */
                    //(1U <<  5U) |   /* enable RX interrupt */
                    (0U << 12U) |  /* 1 start bit, 8 data bits */
                    (1U << 13U));  /* enable USART             */
}
/*..........................................................................*/
void ET_onPrintChar(char const ch) {
    while ((USART2->SR & (1U << 7)) == 0U) { /* while TXE not empty */
    }
    USART2->DR = ch; /* put ch into the DR register */
}
/*..........................................................................*/
void ET_onExit(int err) {
    (void)err;
    /* blink the on-board LED2... */
    for (;;) {
        unsigned volatile ctr;
        GPIOA->BSRRL = (1U << LD2_PIN); /* LED2 on */
        for (ctr = 10000U; ctr != 0U; --ctr) {}
        GPIOA->BSRRH = (1U << LD2_PIN); /* LED2 off */
        for (ctr = 20000U; ctr != 0U; --ctr) {}
    }
}

/*..........................................................................*/
/* error handler called from the exception handlers in the startup code */
void Q_onAssert(char const * const module, int const loc) {
    (void)module;
    (void)loc;
    ET_onExit(-1);
}
