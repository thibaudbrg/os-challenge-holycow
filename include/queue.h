#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct node {
    int *connfd;
    struct node *next;
} node_t;

void enqueue(int *client_socket);

int *dequeue(void);

#ifdef __cplusplus
}
#endif
