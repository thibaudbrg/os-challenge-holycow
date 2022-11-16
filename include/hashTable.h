#ifndef CHALLENGE_HASHTABLE_H
#define CHALLENGE_HASHTABLE_H


#include <stdint.h>
#include "request.h"
#include "messages.h"

typedef struct {
    uint8_t hash[SHA256_DIGEST_LENGTH];
    uint64_t value;
} The_Hash;
void init_table();
uint8_t find_slot (uint8_t *hash);
void insert(uint8_t *hash, uint64_t answer);
uint64_t search(uint8_t *hash);

#endif //CHALLENGE_HASHTABLE_H