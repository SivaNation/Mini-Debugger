/*
 * debugger.c
 *
 *  Created on: Dec 24, 2025
 *      Author: SNEHA
 */

#include "stm32f411xe.h"
#include "debugger.h"

volatile char cmd;

prc_st stptr;
volatile int dbg_pause_req = 0;
volatile int dbg_resume_req = 0;
volatile int dbg_paused = 0;

/* New: request flag set by ISR, handled in main */
volatile int dbg_dump_req = 0;

void USART2_IRQHandler_C(void)
{
    if (USART2->SR & (1 << 5))          // RXNE
    {
        char c = USART2->DR;            // clears RXNE

        if (c == '@')
        {
            if (!dbg_paused)            // ignore repeated '@' while paused
                dbg_pause_req = 1;
        }
        else if (c == '#')
        {
            dbg_resume_req = 1;         // works even while paused (WFI wakes on UART IRQ)
        }
    }
}


void UART_init(void)
{
    RCC->APB1ENR |= (1 << 17);
    USART2->BRR = (0X68 << 4) | (0x03);
    USART2->CR1 |= (1 << 13) | (1 << 5) | (1 << 3) | (1 << 2);
    USART2->CR2 = 0X00;
    USART2->CR3 = 0X00;
    NVIC_EnableIRQ(USART2_IRQn);
}

void GPIO_init(void)
{
    RCC->AHB1ENR |= (1 << 0);
    GPIOA->MODER &= ~(0x000000F0);
    GPIOA->MODER |= (2 << (2 * 2));
    GPIOA->MODER |= (2 << (3 * 2));
    GPIOA->AFR[0] &= ~(0x0000FF00);
    GPIOA->AFR[0] |= (0x07 << 2 * 4);
    GPIOA->AFR[0] |= (0x07 << 3 * 4);
}

void UART_transmit(uint8_t byte)
{
    USART2->DR = byte;
    while (!(USART2->SR & (1 << 7)));  // TXE
}

uint8_t ascii_dat(uint8_t info)
{
    if (info < 10) return (info + 48);
    else return (info + 55);
}

void send_snapshot(char *p)
{
    uint32_t *ptr = (uint32_t *)&stptr;
    uint32_t val;
    uint8_t nibble;

    while (*p != '\0')
    {
        UART_transmit((uint8_t)*(p++));
    }

    if (dbg_paused == 1)
    {
        int j, i;
        for (i = 0; i < (int)(sizeof(stptr) / sizeof(uint32_t)); i++)
        {
            val = *ptr++;

            UART_transmit('0');
            UART_transmit('x');

            for (j = 7; j >= 0; j--)
            {
                nibble = (uint8_t)((val >> (j * 4)) & 0x0F);
                UART_transmit(ascii_dat(nibble));
            }

            UART_transmit('\r');
            UART_transmit('\n');
        }
    }
}
