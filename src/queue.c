#include <stdlib.h>

#include "queue.h" // Include itself for more security

node_t *head = NULL;
node_t *tail = NULL;


void enqueue(int *client_socket) {
    node_t *newnode = malloc(sizeof(node_t));
    newnode->connfd = client_socket;
    newnode->next = NULL;
    if (tail == NULL) {
        head = newnode;
    } else {
        tail->next = newnode;
    }
    tail = newnode;
}

// Returns NULL is the queue is empty
// Returns the pointer to a connfd, if there is one to get
int *dequeue(void) {
    if (head != NULL) {
        int *result = head->connfd;
        node_t *temp = head;
        head = head->next;
        if (head == NULL) {
            tail = NULL;
        }
        free(temp);
        temp = NULL;
        return result;
    }
    return NULL;
}