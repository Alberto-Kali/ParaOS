#include "general_utils.h"
#include <string.h> // Include for memset

char *concat(char dest[], char src[])
{
    int i = 0, j = 0;
    while (dest[i])
        ++i;
    while (src[j])
        dest[i++] = src[j++];
    dest[i] = '\0';
    return dest;
}

char *char_append1(char dest[], char src)
{
    int i = 0;
    while (dest[i])
        ++i;

    dest[i] = src;
    dest[i + 1] = '\0'; // Ensure null-termination
    return dest;
}

char *clear_char_array(char *dest)
{
    memset(dest, 0, sizeof(dest)); // Use memset for efficiency
    return dest;
}

char *deepcopy_char_array(char src[], char dest[])
{
    int i = 0;
    while (src[i]) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0'; // Ensure null-termination
    return dest;
}

void disable_cursor()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

unsigned char *int_to_char(unsigned long value, unsigned char *str, unsigned int base)
{
    unsigned char *ptr = str;
    if (base < 2 || base > 36) {
        *ptr = '\0';
        return str;
    }
    if (value < 0 && base == 10) {
        *ptr++ = '-';
    }
    unsigned char *low = ptr;
    do {
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while (value);
    *ptr-- = '\0';
    while (low < ptr) {
        unsigned char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return str;
}

int is_empty(char inserted_chars[], int run_until)
{
    for (int i = 0; i < run_until; i++) {
        if (inserted_chars[i] == 0) {
            return 1; // Return true if empty
        }
    }
    return 0; // Return false if not empty
}
