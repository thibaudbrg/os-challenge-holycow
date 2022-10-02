#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "hashtable.h" // Include itself for more security

Hash_table *create_hash_table(const Request *request) {
    Hash_table *hash_table = NULL;
    hash_table = malloc(sizeof(Hash_table));
    if (hash_table != NULL) {
        hash_table->length = request->end - request->start;
        hash_table->table = calloc(hash_table->length, sizeof(uint8_t *));
        if (hash_table->table != NULL) {
            for (size_t i = 0; i < hash_table->length; ++i) {
                hash_table->table[i] = calloc(SIZE_HASH, sizeof(uint8_t));
                if (hash_table->table[i] != NULL) {
                    return hash_table;
                }
            }
        }
    }
    return NULL;
}

void destroy_hash_table(Hash_table *hash_table) {
    for (size_t i = 0; i < hash_table->length; ++i) {
        hash_table->table[i] = NULL;
    }
    hash_table->table = NULL;
    free(hash_table->table);
}