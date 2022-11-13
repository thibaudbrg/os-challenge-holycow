
#ifndef CHALLENGE_DECODER_H
#define CHALLENGE_DECODER_H

#include "request.h"
#include "hashTable.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void print_SHA(const unsigned char *SHA);
int compare(const uint8_t *to_compare, const Request *request);
uint8_t *hash(uint64_t *to_hash);
uint64_t decode(const Request *request, The_Hash *theHash );

#endif //CHALLENGE_DECODER_H
