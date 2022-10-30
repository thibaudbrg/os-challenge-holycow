#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct node {
    int *connfd;
    int priority; // Add the priority level of each incoming work
    struct node *next;
} node_t;

void enqueue(int *client_socket);

int *dequeue(void);

#ifdef __cplusplus
}
#endif
