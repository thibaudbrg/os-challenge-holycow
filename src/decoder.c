#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include "decoder.h" // Include itself for more security
#include "messages.h"
#include "request.h"
#include "hashTable.h"

pthread_mutex_t lock;
int n =0;
void init_decode(){
    pthread_mutex_init(&lock,NULL);
}

uint64_t decode(const Request *request) {
    if (request != NULL) {
        uint64_t answer;
        answer = search(request->hash);
        printf("the answer: %" PRIu64 "\n", answer);
        if (answer ==0){
            uint64_t i = request->start;
            for(i; i< request->end; ++i){
                uint8_t *hashed = SHA256((unsigned char *) &i, 8, NULL);
                if(memcmp(hashed,request->hash,SIZE_HASH)==0){
                    pthread_mutex_lock(&lock);
                    insert(request->hash,i);
                    pthread_mutex_unlock(&lock);
                    return htobe64(i);
                }
            }
        }
        n=n+1;
        printf("request repeated %d\n",n);
        return htobe64(answer);
        }
        //printf("Decoded: %" PRIu64 "\n", i);
    perror("ERROR: Pointer \"request\" is NULL: ");
    exit(EXIT_FAILURE);
}

/*int compare(uint8_t const *to_compare, Request const *request) {
    if (to_compare != NULL && request != NULL) {
        if (memcmp(to_compare, request->hash, SHA_DIGEST_LENGTH) == 0) {
            // print_SHA(to_compare);
            return 1;
        }
        return 0;
    }
    perror("ERROR: Pointers \"to_compare\" and/or \"request\" is/are NULL: ");
    exit(EXIT_FAILURE);
}

uint8_t *hash(uint64_t const *to_hash) {
    if (to_hash != NULL) {
        uint8_t *hashed = SHA256((unsigned char *) to_hash, 8, NULL);
        return hashed;
    }
    perror("ERROR: Pointer \"to_hash\" is NULL: ");
    exit(EXIT_FAILURE);
}

uint64_t decode(Request const *request) {
    if (request != NULL) {
        uint64_t i = request->start;
        while (compare(hash(&i), request) != 1 && i < request->end) {
            ++i;
        }
        printf("Decoded: %" PRIu64 "\n", i);
        return i;
    }
    perror("ERROR: Pointer \"request\" is NULL: ");
    exit(EXIT_FAILURE);
}*/