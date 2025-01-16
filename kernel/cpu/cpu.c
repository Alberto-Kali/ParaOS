#include "cpu.h"

inline void disable_interrupts()
{
    __asm__ volatile ("cli");
}

void halt_cpu(){
    disable_interrupts();
    // Implement a more graceful shutdown or idle state
    while (1) {
        __asm__ volatile ("hlt"); // Halt the CPU until the next interrupt
    }
}
