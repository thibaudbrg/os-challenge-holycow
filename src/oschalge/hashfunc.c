#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <stdlib.h>

int main(int argcount, char **args) {

    if (argcount != 2) {
        printf("Usage: %s <string>\n", args[0]);
        return 1;
    }
    printf("INPUT: %s\n", args[1]);
    unsigned char *hash = SHA256(args[1], 8, NULL);
    printf("SHA256: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", hash[i]);}
    printf("\n");
    for (int i = 0; i < 32 ; ++i) {
        printf("%d\n", (uint8_t)hash[i]);
    }


    printf("\n");
    return 0;
}