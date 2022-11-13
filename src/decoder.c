
#include "decoder.h"
#include "request.h"
#include "messages.h"
#include "hashTable.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


uint64_t decode(const Request *request, The_Hash *theHash ) {
    if (request != NULL) {
        uint64_t i = request->start;
        for(i; i<= request->end; ++i){
            uint8_t *hashed = SHA256((unsigned char *) &i, 8, NULL);
            if(memcmp(hashed,request->hash,SIZE_HASH)==0){
                break;
            }
        }
        printf("Decoded: %" PRIu64 "\n", i);
        theHash->answer =i;
        theHash->hash = request->hash;
        return htobe64(i);
    }
    perror("ERROR: Pointer \"request\" is NULL: ");
    exit(EXIT_FAILURE);
}
