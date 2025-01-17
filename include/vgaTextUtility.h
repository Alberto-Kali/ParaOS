#pragma once

#include "kernel.h"

unsigned short vga_entry(unsigned char ch, uint8 fore_color, uint8 back_color);

// Clear video buffer array
void clear_vga_buffer(uint16 **buffer, uint8 fore_color, uint8 back_color);

// Initialize VGA buffer
void init_vga(uint8 fore_color, uint8 back_color);

int println(unsigned char *text);

void printlnVGA(unsigned char *msg);

void remove_written_message_before_newline(unsigned char *msg);

// Add the prototype for printchar
void printchar(unsigned char ch); // Added prototype

/*void writechar(unsigned char c, unsigned char forecolour, unsigned char backcolour, int x, int y);
void writeline(unsigned char *msg);
*/
