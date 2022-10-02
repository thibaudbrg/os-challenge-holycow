#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "request.h"


typedef struct {
    size_t length;
    uint8_t **table;
} Hash_table;

Hash_table *create_hash_table(const Request *request);

void destroy_hash_table(Hash_table *hash_table);

#ifdef __cplusplus
}
#endif