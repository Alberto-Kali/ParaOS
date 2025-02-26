#include "process.h"
#include "terminal.h"
#include "vgaTextUtility.h"

static int next_pid = 0;
struct pv_process procs[max_procs]; 
struct running_proc_list *first_proc = 0x0;  //first proc = terminal.
struct running_proc_list *cpr = 0x0; //linked list object with current process

extern int foreground_process;

static int running_procs = 0;

running_proc_list create_process(void (*code)(char), char *args) {
    __asm__ volatile ("cli");
    pv_process* process = ((pv_process*) malloc(sizeof(pv_process), 1));
    running_proc_list* pr = ((running_proc_list*) malloc(sizeof(running_proc_list), 1));

    process->pid = ++next_pid;
    process->text_address = (long *)code; // Store the function pointer
    process->status = READY;
    process->eip = 0;
    process->esp = 0;
    process->ebp = 0;
    process->prio = 0;
    if (is_empty(args, 32) > 0) {
        deepcopy_char_array(args, process->args);
    } else {
        process->args = 0x0;
    }

    pr->process = process;

    if (cpr != 0x0 && cpr->next == 0x0) {
        pr->previous = cpr;
        cpr->next = pr;
    } else if (cpr != 0x0) {
        running_proc_list *temp = cpr;
        while (temp->next != 0x0) {
            temp = temp->next;
        }
        temp->next = pr;
        pr->previous = temp;
    } else {
        cpr = pr;
        cpr->next = cpr->previous = 0x0;
    }

    ++running_procs;
    __asm__ volatile ("sti");
    return *pr; 
}

// Function to handle process completion
void complete_process(struct running_proc_list *proc) {
    // Perform cleanup or finalization for the completed process
    // Free memory or resources associated with the process
    --running_procs; // Decrement the count of running processes
    print_msg("kernel~>");
}

// Returned the interrupted or recently created process with the highest priority (i.e. that has waited for the most time), or 0x0 if no queued process found
struct running_proc_list * queued_processes() {
    __asm__ volatile ("cli"); // Disable interrupts
    if (cpr == 0x0) 
        return 0x0;

    struct running_proc_list *temp = &(*cpr); // Keeps the original address even if cpr ends up pointing to something else
    while (temp->previous != 0x0) {
        temp = temp->previous; // leading to the very first process in the chain
    }
    struct running_proc_list *return_me = 0x0; // to be returned

    while (temp != 0x0) {
        if (temp->process->status == WAITING || temp->process->status == READY) {
            if (return_me != 0x0) {
                if (temp->process->prio > return_me->process->prio) {
                    ++(return_me->process->prio); // increase the prio of the rejected process, before rejecting it.
                    return_me = &(*temp);  
                }
            } else {
                return_me = &(*temp);
            }
        }
        temp = temp->next;
    }

    __asm__ volatile ("sti"); // Re-enable interrupts
    return return_me;
}

void force_task_switch() {
    struct running_proc_list *interrupted_proc = &(*cpr);
    struct running_proc_list *que = (struct running_proc_list *)queued_processes(); // first entry

    if (que == 0x0) {
        outb(0x20, 0x20); // reenable interrupts
        return;
    }

    __asm__ volatile ("cli"); // run_until_end_or_task_switch() reenables interrupts again when needed.
    if (interrupted_proc->process->status == RUNNING) { 
        interrupted_proc->process->status = WAITING; 
    }
    cpr = que;
    run_until_end_or_task_switch(que, 0);

    __asm__ volatile ("sti");
        
    return;
}

void run_until_end_or_task_switch(struct running_proc_list *proc, int pos) {
    running_proc_list prev_proc;

    proc->process->status = RUNNING;

    typedef void* function(char *args);
    typedef void* noargsfun();

    outb(0x20, 0x20); // reenable interrupts

    __asm__ __volatile__("sti\n\t"); // CLIed in force_task_switch() before calling
    if (proc->process->args == 0x0) {
        noargsfun* fun = (void *) proc->process->text_address; // This jumps to the beginning of the interrupted code. 
        fun();
    } else {
        function* fun = (void *) proc->process->text_address;
        fun(proc->process->args);
    }

    __asm__ __volatile__("cli\n\t"); // Will STI in force_task_switch() after returning
    proc->process->status = COMPLETED;
    complete_process(proc); // Call the completion function

    // Process list cleanup
    if (proc->previous != 0x0) {
        proc->previous->next = proc->next;
    }
    if (proc->next != 0x0) {
        proc->next->previous = proc->previous;
    }

    // Free memory allocated for the process
    //free(proc->process);
    //free(proc);  #NOT WORKING BCS WE IN KERNEL MODE

    return;
}

void proc_init() {
    __asm__ volatile ("cli");

    cpr = 0x0;
    void (*r_input)(char) = receive_input;

    foreground_process = r_input;

    __asm__ volatile ("sti");

    start_terminal_proc();
}
