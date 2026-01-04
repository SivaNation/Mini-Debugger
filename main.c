#include <stdint.h>
#include "stm32f411xe.h"
#include "debugger.h"

int main(void)
{
    GPIO_init();
    UART_init();

    while (1)
    {
        /* Debugger service */
        if (dbg_pause_req)
        {
            dbg_pause_req = 0;
            dbg_paused = 1;


            /* Print snapshot once (snapshot was already captured by wrapper on '@' IRQ) */
            send_snapshot("HALTED (software pause). Send # to resume\r\n the processor state is r0-r12,sp_active,lr, pc, xpsr,psp, msp,control,primask\r\n");

            /* Pause here: sleep until an interrupt occurs */
            while (dbg_paused)
            {
                __asm volatile ("wfi");     // or __WFI() if you use CMSIS

                if (dbg_resume_req)
                {
                    dbg_resume_req = 0;
                    dbg_paused = 0;

                    send_snapshot("RESUMED\r\n");
                }
            }
        }
      int sum=0,k;
        /* Application code runs only when not paused */
        if (!dbg_paused)
        {
        	for(k=0;k<100;k++)
        		sum+=k;
        }


    }
}
