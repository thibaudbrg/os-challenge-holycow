#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct node {
    int *connfd;
    struct node *next;
} node_t;

void enqueue(int *p_connfd);

int *dequeue(void);

#ifdef __cplusplus
}
#endif
