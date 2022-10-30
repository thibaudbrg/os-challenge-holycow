#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "request.h"

typedef struct node {
    int *connfd;
    Request *request; // Add the whole request including the priority
    struct node *next;
} node_t;

void enqueue(int *connfd);

node_t *dequeue(void);

void destroy_node(node_t *node);

void print_queue(void);

#ifdef __cplusplus
}
#endif
