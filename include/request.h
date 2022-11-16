#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *hash;
    uint64_t start;
    uint64_t end;
    uint8_t p;
} Request;

void print_SHA(unsigned const char *SHA);

Request *create_empty_request(void);

void destroy_request(Request *request);

Request *getRequest(int const *p_connfd);

#ifdef __cplusplus
}
#endif