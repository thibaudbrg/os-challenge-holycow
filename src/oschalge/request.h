#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SIZE_HASH 32
#define SIZE_START 8
#define SIZE_END 8
#define SIZE_OF_BYTE 8

typedef struct {
    uint8_t *hash;
    u_int64_t start;
    u_int64_t end;
    uint8_t p;
} Request;

Request *create_empty_request(void);

void destroy_request(Request *request);

Request *getRequest(const unsigned char *all_bytes, size_t length);

#ifdef __cplusplus
}
#endif