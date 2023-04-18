#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "stdio.h"


void encodeA(char *str)
{
    for(int i=0; i<strlen(str); i++) {
        if(isupper(str[i])) {
            str[i] = tolower(str[i]);
        } else if(islower(str[i])) {
            str[i] = toupper(str[i]);
        }
    }
    printf("%s\n", str);
}

void decodeA(char *str)
{
    encodeA(str);
}
