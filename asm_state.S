.syntax unified
.thumb

.global capture_snapshot
.type capture_snapshot, %function
.thumb_func
capture_snapshot:
    /* store R4–R11 (callee-saved registers) into struct */
    STR  r4,[r0,#16]
    STR  r5,[r0,#20]
    STR  r6,[r0,#24]
    STR  r7,[r0,#28]
   /* Store R8–R11 via low temp (r3) (offsets 32..44) */
    MOV     r3, r8
    STR     r3, [r0, #32]
    MOV     r3, r9
    STR     r3, [r0, #36]
    MOV     r3, r10
    STR     r3, [r0, #40]
    MOV     r3, r11
    STR     r3, [r0, #44]


 /* Save sp_active = frame pointer (offset 13*4 = 52) */


    /* Copy stacked R0-R3 (frame[0..3]) into struct r0-r3 (offsets 0,4,8,12) */
    LDR  r3, [r1, #0]
    STR  r3, [r0, #0]
    LDR  r3, [r1, #4]
    STR  r3, [r0, #4]
    LDR  r3, [r1, #8]
    STR  r3, [r0, #8]
    LDR  r3, [r1, #12]
    STR  r3, [r0, #12]
    STR  r1, [r0, #52]

    /* Copy stacked R12, LR, PC, xPSR into struct (r12 at 48, lr at 56, pc at 60, xpsr at 64) */
    LDR  r3, [r1, #16]
    STR  r3, [r0, #48]
    LDR  r3, [r1, #20]
    STR  r3, [r0, #56]
    LDR  r3, [r1, #24]
    STR  r3, [r0, #60]
    LDR  r3, [r1, #28]
    STR  r3, [r0, #64]

    /*  store PSP/MSP/CONTROL/PRIMASK into struct */
    MRS  r3, PSP
    STR  r3, [r0, #68]
    MRS  r3, MSP
    STR  r3, [r0, #72]
    MRS  r3, CONTROL
    STR  r3, [r0, #76]
    MRS  r3, PRIMASK
    STR  r3, [r0, #80]

    BX      lr
