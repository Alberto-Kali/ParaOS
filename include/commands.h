#pragma once


struct command_pointer{
    char* name;
	int *command_pointer;
};

void ascii_art();
void help();
void halt();
void cpu_info();
void charprint();
void charprint();

void showval();

void inf_loop();