#include "hashTable.h"
#include "request.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX 600

uint64_t  hashTable[MAX] = {0};

uint8_t index_hash (uint8_t *hash){
    uint8_t  i =0;
    for(int j =0; j< SIZE_HASH; ++j){
        i += hash[j];
    }
    return i % MAX;
}

void insert(uint8_t *hash, uint64_t answer){
    int i = index_hash(hash);
    hashTable[i] = answer;
}

uint64_t search(uint8_t *hash){
    return hashTable[index_hash(hash)];
}