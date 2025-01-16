#include "terminal.h"
#include "vgaTextUtility.h"
#include "process.h"
#include "commands.h"

extern int foreground_process;
extern int calling_foreground_process;

char command[32];
struct command_pointer cmds[32];
char args[32];
char previous_command;
int previous_command_pointer = 0;
int command_history = 0;
static int inexistent_command = 0;

void command_history_handler() {
    deepcopy_char_array(command, previous_command);
    return;
} 

void show_previous_command() {
    if (previous_command_pointer != 0) {
        // do nothing.
    } else {
        remove_written_message_before_newline(command);
        deepcopy_char_array(previous_command, command);
        print_msg(command);
        ++previous_command_pointer;
    }
    return;
}

char* fetch_args(char inserted_chars[], int cmd_name_ending_position) {
    int i = cmd_name_ending_position, j = 0;
    while (inserted_chars[i] != 0) {
        args[j++] = inserted_chars[i++];
    }
    return args;
}

char *terminal_char_append(char dest[], char src) {
    int i = 0;
    while (dest[i] != 0) {
        ++i;
    }

    if (src != '\b') { // backspace
        dest[i] = src;
    } else {
        dest[--i] = 0x00; // Correctly handle backspace
    }

    dest[i + 1] = '\0'; // Ensure null-termination
    return dest;
}

void find_and_create_process(unsigned char *inserted_chars) {
    int j, i;
    inexistent_command = 1;
    for (i = 0; i < 32; i++) {
        if (cmds[i].command_pointer == 0) { // if undefined command
            break;
        }
        inexistent_command = 0;
        j = 0;

        while (cmds[i].name[j] != 0 && inexistent_command == 0) { 
            if ((int)(cmds[i].name[j] ^ inserted_chars[j]) != 0) {
                inexistent_command = 1; 
            }
            j++;
        }

        if (inexistent_command == 0) {
            void (*cmd)(char) = cmds[i].command_pointer; // Ensure correct function pointer type
            char *args = fetch_args(inserted_chars, j); // get the arguments after the command
            create_process(cmd, args);
            command_history_handler();
            previous_command_pointer = 0;
            return;
        }
    }
    if (inexistent_command != 0) { // the loop has checked all commands and they are nothing like the one given.
        printlnVGA("Command not found.");
    }
    return;
}

void receive_input(char ch) {
    if (ch != '\n' && ch != 24) { // newline or up
        terminal_char_append(command, ch);
        printchar(ch);
    } else if (ch == 24) { // arrow up
        show_previous_command(); // show the last command that ran successfully
    } else if (ch == '\n') { // on enter
        printchar('\n');
        __asm__ __volatile__("cli\n\t");
        find_and_create_process(command);
        clear_char_array(command);
        clear_char_array(args);
        __asm__ __volatile__("sti\n\t");
    }
}

void register_commands() {
    cmds[0].name = "sample";
    cmds[0].command_pointer = sample_command;
    cmds[1].name = "dample";
    cmds[1].command_pointer = dample_command;
    cmds[2].name = "malloc";
    cmds[2].command_pointer = malloc_command;
    cmds[3].name = "clear";
    cmds[3].command_pointer = clear_terminal_command;
    cmds[4].name = "cpuinfo";
    cmds[4].command_pointer = cpu_info;
    cmds[5].name = "infloop";
    cmds[5].command_pointer = inf_loop;
    cmds[6].name = "showval";
    cmds[6].command_pointer = showval;
    cmds[7].name = "help";
    cmds[7].command_pointer = help;
    cmds[8].name = "ascii";
    cmds[8].command_pointer = ascii_art;
}

void start_terminal_independently() {
    register_commands();
    void (*r_input)(char) = receive_input; // function pointer
    foreground_process = (int)r_input;
    printlnVGA("Started terminal!");
    printchar('>');
}

void start_terminal_proc() {
    register_commands();
    printlnVGA("Started terminal!");
    printchar('>');
}
