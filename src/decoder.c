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
int n = 0;


/**
 * The reverse-hashing function
 * @param request : the request received from the client
 * @return the value corresponding to the received hash
 */
uint64_t decode(const Request *request) {
    if (request != NULL) {
        uint64_t answer;
        // Look if the answer already exists in the hash table
        answer = search(request->hash);
        printf("Decoded: %" PRIu64 "\n", answer);
        // If the answer is not in the hash-table, which means
        // it's a new hash and not a previously received one
        if (answer == 0) {
            uint64_t i = request->start;
            for (i; i < request->end; ++i) {
                uint8_t *hashed = SHA256((unsigned char *) &i, 8, NULL);
                if (memcmp(hashed, request->hash, SHA256_DIGEST_LENGTH) == 0) {
                    pthread_mutex_lock(&lock);
                    insert(request->hash, i);
                    pthread_mutex_unlock(&lock);
                    return htobe64(i);
                }
            }
        }
        n = n + 1;
        // printf("     Repetition number: %d\n", n);
        return htobe64(answer);
    }
    //printf("Decoded: %" PRIu64 "\n", i);
    perror("ERROR: Pointer \"request\" is NULL: ");
    exit(EXIT_FAILURE);
}