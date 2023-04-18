#include <string.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include "dlfcn.h"

typedef char* (*pencode)(char *str);
int main(int argc, char *argv[])
{
    if(argc != 3) {
        printf("usage: %s <codec> <message>\n", argv[0]);
        exit(0);
    }

    //codecA 
    if (argv[1] && strcmp(argv[1], "codecA") == 0)
    {
        void* plib = dlopen("./libcodecA.so",RTLD_LAZY);
        if(plib == NULL){
            printf("failed to load library\n");
            exit(0);
        }
        pencode encodeA = (pencode)dlsym(plib,"encodeA");
        if(encodeA == NULL){
            printf("failed to load function from library\n");
            dlclose(plib);
            exit(0);
        }
        encodeA(argv[2]);
        dlclose(plib);
    }

    //codecB
    else if (argv[1] && strcmp(argv[1], "codecB") == 0)
    {
        void* plib = dlopen("./libcodecB.so",RTLD_LAZY);
        if(plib == NULL){
            printf("failed to load library\n");
            exit(0);
        }
        pencode encodeB = (pencode)dlsym(plib,"encodeB");
        if(encodeB == NULL){
            printf("failed to load function from library\n");
            dlclose(plib);
            exit(0);
        }
        encodeB(argv[2]);
        dlclose(plib);
    }

    else
    {
        printf("usage: encode/decode <codec> <message> \n");
    }

    return 0;
}