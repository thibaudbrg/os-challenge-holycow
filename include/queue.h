#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct node {
    int *connfd;
    struct node *next;
} node_t;

void enqueue(int *connfd);

int *dequeue(void);

#ifdef __cplusplus
}
#endif
