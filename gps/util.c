#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

// Function to swap two numbers
void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}

// Function to reverse `buffer[i…j]`
char* reverse(char *buffer, int i, int j)
{
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }

    return buffer;
}

// Iterative function to implement `itoa()` function in C
char* itoa(char *buffer, int value, int base)
{
    // invalid input
    if (base < 2 || base > 32) {
        return buffer;
    }

    // consider the absolute value of the number
    int n = abs(value);

    int i = 0;
    while (n)
    {
        int r = n % base;

        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }

        n = n / base;
    }

    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }

    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0'; // null terminate string

    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

unsigned char hex_uppercase(unsigned char ptr)
{
	if(ptr<='f' && ptr>='a') {
		return(ptr-'a')+'A';
	}

	return ptr;
}

int hexasc2num(char *ptr,int len)
{
    int retval=0,val;
	int i;

	for(i=0;i<len;i++) {
	     ptr[i]=hex_uppercase(ptr[i]);
	     val=0;
	     if(ptr[i]>='A' && ptr[i]<='F') {
		     val=(ptr[i]-'A'+10);
		 }
		 else if(ptr[i]>='0' && ptr[i]<='9') {
		     val=(ptr[i]-'0');
		 }
		 retval = (retval<<4)|val;
	}
	return retval;
}

void convert2asc(int num, char *str, int width, int radix)
{
    char tmp[10];
	int sl,i,j;

	itoa(tmp, num, radix);
	sl=strlen(tmp);

	for(i=0;i<width-sl;i++) str[i]='0';
    for(i=width-sl,j=0;i<width;i++,j++) str[i]=hex_uppercase(tmp[j]);
	str[width]=0;
}
