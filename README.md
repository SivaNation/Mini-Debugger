UART-Triggered Mini Debugger (STM32F411) — README (TXT)
====================================================

One-line description
--------------------
A learning project mini-debugger for STM32F411 that snapshots CPU state and pauses/resumes firmware using single-character UART commands.

What it does
------------
- Send '@' over UART:
  - captures a processor context snapshot (register state)
  - prints the snapshot to the host over UART
  - enters a software halt (low-power wait)
- Send '#' over UART:
  - exits the halt and continues normal execution

Why this design exists
----------------------
1) Cortex-M hardware stacking is incomplete
   On Cortex-M, interrupt entry automatically stacks only:
     r0 r1 r2 r3 r12 LR PC xPSR
   Registers r4–r11 are not stacked by hardware. Capturing “true” r4–r11 requires saving them immediately at interrupt entry.

2) A pure C ISR can distort the snapshot
   A C interrupt handler may run a compiler-generated prologue before user code executes. The prologue can change registers and adjust the stack, which can corrupt a “snapshot taken later.”

3) An assembly ISR wrapper enables correct context capture
   A thin assembly wrapper for USART2_IRQHandler:
   - decides whether MSP or PSP was active at exception entry (via EXC_RETURN in LR)
   - obtains the exception frame pointer
   - captures r4–r11 early (before C code touches them)
   - calls a small C handler that only decodes UART commands and sets request flags

4) EXC_RETURN must be preserved
   In exception context, LR holds EXC_RETURN (a special value used for return-from-interrupt). Function calls (BL) overwrite LR, so EXC_RETURN must be preserved (for example, push {lr} / pop {lr}) before returning with bx lr.

5) UART dumping should not run inside the ISR
   Printing a full register dump can be slow if implemented with blocking UART transmit loops. Doing that inside an ISR can:
   - delay other interrupts
   - risk UART RX overrun
   - create unstable timing
   The stable approach is:
   - ISR sets request flags (fast)
   - main loop performs the dump and pause logic (safe)

Commands
--------
- '@' : snapshot + dump + enter paused state
- '#' : resume from paused state

State machine
-------------
States:
  RUN     : normal application execution
  DUMPING : snapshot is printed once over UART
  PAUSED  : CPU sleeps in a WFI loop until '#' is received

Flow:
  RUN --('@')--> DUMPING --> PAUSED --('#')--> RUN

Notes:
- Snapshot timing is tied to the interrupt (wrapper capture happens immediately at IRQ entry).
- The pause point occurs at the next main-loop service point (software halt is cooperative).

Flags (why request flags exist)
-------------------------------
Two request flags provide a clean handoff from interrupt context to main context:

- dbg_dump_req:
  Set by UART ISR when '@' arrives. Consumed by main to print the snapshot and enter PAUSED state.
- dbg_resume_req:
  Set by UART ISR when '#' arrives. Consumed by main to exit PAUSED state.

Request flags prevent long UART printing inside the ISR and keep interrupt latency predictable.

Hardware wiring (USART2 on STM32F411)
-------------------------------------
Pins:
- PA3 = USART2_RX  <- connect to USB-TTL TX
- PA2 = USART2_TX  -> connect to USB-TTL RX
- GND <-> GND
Logic level:
- 3.3V TTL recommended (avoid 5V on STM32 pins)

Build notes
-----------
- Toolchain: STM32CubeIDE / GCC for ARM
- Ensure the vector table resolves USART2_IRQHandler to the assembly wrapper.
- Ensure ISR wrapper and snapshot assembly files are included in the build (not excluded).

Output expectations
-------------------
Typical address patterns:
- PC often appears as 0x0800xxxx (Flash region)
- sp_active/frame pointer often appears as 0x2000xxxx (SRAM region)

Limitations
-----------
- Software halt pauses at a safe scheduling point (main-loop service), not at an arbitrary instruction like a hardware SWD/JTAG debugger.
- UART resume depends on UART interrupts remaining enabled during the PAUSED state.
