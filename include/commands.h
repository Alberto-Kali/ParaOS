#pragma once

struct command_pointer {
    char* name;
    void (*command_pointer)(char*); // Updated to use the correct function pointer type
};


void ascii_art();
void help();
void halt();
void cpu_info();
void charprint();
void charprint();
void showval();
void inf_loop();