#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#define SIZE_HASH 32

typedef struct {
    uint8_t *hash;
    uint64_t start;
    uint64_t end;
    uint8_t p;
} Request;


Request *create_empty_request(void);

void destroy_request(Request *request);

Request *getRequest(const unsigned char *all_bytes, size_t length);

#ifdef __cplusplus
}
#endif