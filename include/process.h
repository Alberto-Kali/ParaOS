#pragma once

#include "cpu.h"
#include "commands.h"

enum process_status {
    BLOCKED,
    READY, // The process is waiting to be assigned to a processor.
    RUNNING,
    INTERRUPTED,
    WAITING, // The process is waiting for some event to occur,
    COMPLETED
}; // POSIX-compliant

typedef struct pv_process {
    int pid;
    int prio;
    enum process_status status;
    long int creation_timestamp;
    long text_address; // Function pointer to code. Was void.
    unsigned int esp, ebp;       // Stack and base pointers.
    unsigned int eip;            // Instruction pointer.
    unsigned long max_address;
    char* args;
} pv_process;

typedef struct running_proc_list {
    struct pv_process *process;
    struct running_proc_list *next;
    struct running_proc_list *previous;
} running_proc_list;

struct running_proc_list create_process(void (*code)(char), char *args);
void run_until_end_or_task_switch(struct running_proc_list *proc, int pos); // Added prototype
void run_process(struct pv_process *pv_process, char args[]);

void force_task_switch();
