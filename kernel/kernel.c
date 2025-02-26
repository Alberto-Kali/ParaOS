
#include "kernel.h"
#include "interrupt_service_routines.h"
#include "process.h"
#include "memoryManager.h"
#include "vgaTextUtility.h"
#include "commands.h"
#include "general_utils.h"
#include "timer.h"


/* TODO: Check why this is necessary.
    This code will be placed at the beginning of the object by the linker script   */
 __asm__ (".pushsection .text.start\r\n" \
         "jmp main\r\n" \
         ".popsection\r\n"
         );  


int foreground_process = 0; //the address of the process currently appearing -either for receiving input, or for printing it.
int calling_foreground_process = 0;
volatile long val = 0;
void main()
{
    init_vga(WHITE, BLACK);

    boot_memory_init(); 

    printlnVGA("");

    PIC_remap();
    initialize_pic();
    initialize_idt();
    isr_install();
    irq_install();
    initialize_paging();

    // int l = 1/0; //test for division-by-zero exception
    clear();

    unsigned char StrOS[] = "ParaOS";
    unsigned char myString[] = "Kernel running";
    printlnVGA(StrOS);
    printlnVGA(myString);
    printlnVGA("");
    //start_terminal_independently();
    proc_init();
    
    init_timer(1); //5Hz

    while (1)
        __asm__("hlt\n\t");
}
