#include <stdio.h>
#include "codecA.c"
#include "codecB.c"

int main(int argc, char **argv)
{
    //check correct usage
    if(argc != 3) {
        printf("usage: %s <codec> <message>\n", argv[0]);
        return 1;
    }

    // init user arguments
    char *func = argv[0];
    char *codec = argv[1];
    char *message = argv[2];

    // check which encode/decode to do
    if(strcmp(codec, "codecA") == 0) {
        if(strcmp(func, "./encode") == 0){
            encodeA(message);
        }
        if(strcmp(func, "./decode") == 0){
            decodeA(message);
        }
    } else if(strcmp(codec, "codecB") == 0) {
        if(strcmp(func, "./encode") == 0){
            encodeB(message);
        }
        if(strcmp(func, "./decode") == 0){
            decodeB(message);
        }
    } else {
        printf("Unknown codec\n");
        return 1;
    }

    //output the result
    printf("%s\n", message);

    return 0;
}
