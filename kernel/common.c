#include "common.h"

void outw(uint16 port, uint16 val)
{
    asm __volatile__ ("outw %%ax, %%dx" : : "d" (port), "a" (val));
}

void outl(uint16 port, uint32 val)
{
    asm volatile("outl %0,%1" : : "a" (val), "dN" (port));
}

uint32 inl(uint16  port)
{
    uint32 val;
    asm volatile("inl %1,%0" : "=a" (val) : "dN" (port));
    return val;
}

uint16 inw(uint16 port)
{
    uint16 ret;
    asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

void buzywait(uint32 time)
{
    // Implement a more efficient waiting mechanism
    for (volatile uint32 i = 0; i < time * 1000; i++);
}

void sleep(uint32 time)
{
    // Implement a more efficient waiting mechanism
    for (volatile uint32 i = 0; i < time * 1000; i++);
}

void memset(uint32 *dest, int val, uint32 size)
{
    int i;
    uint32 *poi = dest;
    for(i=0; i<size; i++)
    {
        *poi = val;
        poi++;
    }
}

void copy(unsigned char *dest, unsigned char *src, int len)
{
    int i;
    for(i=0; i<len; i++) {
        dest[i] = src[i];
    }
}

int strcmp(char *str1, char *str2)
{
    int i=0;
    while(str1[i] != '\0' && str2[i] != '\0' && (str1[i] == str2[i])) {
        i++;
    }
    if(str1[i] == '\0' && str2[i] == '\0') {
        return 0;
    }
    if(str1[i] == '\0' && str2[i] != '\0') {
        return -1;
    }
    if(str1[i] != '\0' && str2[i] == '\0') {
        return 1;
    }
}

int strncmp(char *str1, char *str2, int n)
{
    int i=0;
    while(str1[i] != '\0' && str2[i] != '\0' && (str1[i] == str2[i]) && i < n) {
        i++;
    }
    if(i==n) {
        return 0;
    }
	return 1;
}

int atoi(char *str)
{
	int num=0;
	int i=0;
	while(str[i]!='\0'){
		num=num*10+str[i]-48;
		i++;
	}
	return num;
}
