#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *hash;
    u_int64_t start;
    u_int64_t end;
    uint8_t p;
} Request;

void print_SHA(unsigned const char *SHA);

Request *create_empty_request(void);

void destroy_request(Request *request);

Request *getRequest(int const *p_connfd);

#ifdef __cplusplus
}
#endif