.syntax unified
.thumb

.global USART2_IRQHandler
.extern USART2_IRQHandler_C
.extern capture_snapshot
.extern stptr
.type USART2_IRQHandler, %function
.thumb_func

USART2_IRQHandler:
    push  {lr}                 @ save EXC_RETURN immediately

    tst   lr, #4
    ite   eq
    mrseq r2, msp              @ r2 = exception frame base (MSP)
    mrsne r2, psp              @ r2 = exception frame base (PSP)

    ldr   r0, =stptr           @ arg0 = &stptr
    mov   r1, r2               @ arg1 = frame pointer
    bl    capture_snapshot

    bl    USART2_IRQHandler_C

    pop   {lr}                 @ restore EXC_RETURN
    bx    lr                   @ exception return
