#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include "hashTable.h"
#include "messages.h"

#define MAX 4096
The_Hash hashTable[MAX];
pthread_mutex_t lock;

void init_table() {
    pthread_mutex_init(&lock, NULL);
}

/**
 * A function to find the index in the hash-table corresponding
 * to the struct containing key (i.e the hash )
 * @param key : the hash
 * @return the index in the hash-table corresponding to the struct
 * containing key
 */
uint8_t find_slot(uint8_t key[]) {
    uint8_t i = 0;
    for (int j = 0; j < SHA256_DIGEST_LENGTH; ++j) {
        i += key[j];
    }
    i = i % MAX;
    // We either find an empty slot or we find the index that we are looking for
    while (hashTable[i].value != 0 && memcmp(hashTable[i].hash, key, SHA256_DIGEST_LENGTH * sizeof(uint8_t)) != 0) {
        i = (i + 1) % MAX;
    }
    return i;
}

/**
 * A function that returns the value corresponding to key (i.e hash)
 * @param key : the hash
 * @return the value corresponding to key (i.e hash)
 */
uint64_t search(uint8_t key[]) {
    uint8_t i = find_slot(key);
    if (hashTable[i].value != 0) {
        return hashTable[i].value;
    }
    return 0;
}

/**
 * A function to insert a key (i.e the hash) and its corresponding value
 * @param key : the hash
 * @param value : the value corresponding to the hash
 */
void insert(uint8_t key[], uint64_t value) {
    uint8_t i = find_slot(key);
    // The value already exists in the hash-table
    if (hashTable[i].value != 0 && hashTable[i].value == value) {
        return;
    } else {
        //pthread_mutex_lock(&lock);
        memcpy(hashTable[i].hash, key, SHA256_DIGEST_LENGTH);
        hashTable[i].value = value;
        //pthread_mutex_unlock(&lock);
    }

}
