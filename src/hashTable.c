#include "hashTable.h"
#include "request.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX 600
The_Hash hashTable[MAX];
pthread_mutex_t lock;

void init_table(){
    pthread_mutex_init(&lock,NULL);
}

uint8_t find_slot(uint8_t key[]){
    uint8_t  i =0;
    for(int j =0; j< SIZE_HASH; ++j){
        i += key[j];
    }
    i = i%MAX;
    while(hashTable[i].value !=0 && memcmp(hashTable[i].hash, key, SIZE_HASH*sizeof(uint8_t)) !=0){
        i = (i+1)% MAX;
    } 
    return i;
}

uint64_t search(uint8_t key[]){
    uint8_t i = find_slot(key);
    if(hashTable[i].value !=0){
        return hashTable[i].value;
    }
    return 0;
}

void insert(uint8_t key[], uint64_t value){
    uint8_t i = find_slot(key);
    if(hashTable[i].value !=0 && hashTable[i].value ==value) {
        return;
    }else{
        //pthread_mutex_lock(&lock);
        memcpy(hashTable[i].hash, key,SIZE_HASH);
        hashTable[i].value = value;
        //pthread_mutex_unlock(&lock);
    }

}
