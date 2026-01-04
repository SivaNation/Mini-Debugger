/*
 * debugger.h
 *
 *  Created on: Dec 24, 2025
 *      Author: SNEHA
 */

#ifndef DEBUGGER_H_
#define DEBUGGER_H_

#include <stdint.h>

typedef struct processor_state
{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t sp_active;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
    uint32_t psp;
    uint32_t msp;
    uint32_t control;
    uint32_t primask;
} prc_st;

void UART_init(void);
void GPIO_init(void);
void capture_snapshot(prc_st *p, uint32_t *frame);

void send_snapshot(char *p);
uint8_t ascii_dat(uint8_t info);
void UART_transmit(uint8_t byte);

void USART2_IRQHandler_C(void);

/* --- new: debugger control flags --- */
extern volatile int dbg_dump_req;
extern int halted;
extern volatile int dbg_pause_req ;
extern volatile int dbg_resume_req;
extern volatile int dbg_paused ;

#endif /* DEBUGGER_H_ */
