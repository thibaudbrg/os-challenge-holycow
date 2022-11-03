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

typedef struct {
    node_t *head;
    size_t size;
} Queue;

node_t * createNode(int *p_connfd);

Queue* createQueue(void);

void enqueue(int *p_connfd, Queue* queue);

node_t *dequeue(Queue* queue);

void destroy_node(node_t *node);

void print_queue(Queue* queue);

#ifdef __cplusplus
}
#endif
