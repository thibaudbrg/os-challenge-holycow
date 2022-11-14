#ifndef CHALLENGE_HASHTABLE_H
#define CHALLENGE_HASHTABLE_H


#include <stdint.h>
#include "request.h"

typedef struct {
    uint8_t hash[SIZE_HASH];
    uint64_t value;
} The_Hash;

uint8_t index_hash (uint8_t *hash);
void insert(uint8_t *hash, uint64_t answer);
uint64_t search(uint8_t *hash);

#endif //CHALLENGE_HASHTABLE_H