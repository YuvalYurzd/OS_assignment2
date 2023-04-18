#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "stdio.h"



void encodeB(char *str)
{
    for(int i=0; i<strlen(str); i++) {
        str[i] = ((str[i] - ' ' + 3) % 95) + ' ';
    }
    printf("%s\n",str);
}

void decodeB(char *str)
{
    for(int i=0; i<strlen(str); i++) {
        str[i] = ((str[i] - ' ' - 3 + 95) % 95) + ' ';
    }
    printf("%s\n",str);
}