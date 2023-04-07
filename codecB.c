#include <string.h>
#include <stdlib.h>
#include <ctype.h>



void encodeB(char *str)
{
    for(int i=0; i<strlen(str); i++) {
        str[i] = ((str[i] - ' ' + 3) % 95) + ' ';
    }
}

void decodeB(char *str)
{
    for(int i=0; i<strlen(str); i++) {
        str[i] = ((str[i] - ' ' - 3 + 95) % 95) + ' ';
    }
}